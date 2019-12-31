#include "malloc-metadata.hpp"

using std::cout;

[[nodiscard]] size_t
getMallocMetaData(const void *heapAddr, const pid_t pid, const size_t max_size, const bool printWarnings/*=false*/) {
    // Gives us a 1 byte heap array containing the 1 byte before heapAddr
    char *sizePtr = DeepCopy(
		    pid, (char *)heapAddr - sizeof(void *), sizeof(void *));
    size_t size = *sizePtr;
    delete[] sizePtr;

    // The 3 least significant bits of the size are flags not the actual size
    // glibc can do this as all allocations are a multiple of 8 so the 3 least
    // significant bits are unused So a 32 bit allocation would be 39 if all flags
    // are set We floor the size to the nearest 8 to get the actual size of the block
    const auto flags = size % 8;

    //Drop the 3 least significant bits. They are flags not the allocated size
    size = (size / 8) * 8;
    if (printWarnings) {
        if (size == 0) {
            cout << "WARNING: malloc'd size reported as 0 for address: " << heapAddr << '\n';
        }
        if (size % 8 != 0) {
            cout << "WARNING: malloc'd size not reported as a multiple of 8 for address: " << heapAddr << '\n';
        }

        // Minimum heap chunk in glibc is 4*(sizeof(void*))
        const auto MINIMUM_CHUNK_SIZE = 4 * sizeof(void *);
        if (size < MINIMUM_CHUNK_SIZE) {
            cout
                    << "WARNING: malloc'd size reported as less than the minimum glibc should allocate, 4*sizeof(void*). Reported size: "
                    << size << " at address: " << heapAddr << '\n';
        }
    }
    if (size > max_size) {
      if (printWarnings){
	cout << "WARNING: malloc'd size reported as more than the limit. Reported size:" << size << " limit:"
	     << max_size << '\n';
      }
        size = 0;
    }

    return size;
}
