#include "malloc-metadata.hpp"

using std::cerr;
using std::cout;

[[nodiscard]] size_t GetMallocMetadata(const void *heap_address, const pid_t pid,
		const size_t max_size, const bool print_warnings /*=false*/,
		const bool is_local_address /*=false*/) {
  if (heap_address == nullptr) {
    return 0;
  }
  // The size is stored in the 8 bytes preceding the start of the object
  const size_t *size_location = (size_t *)((char *)heap_address - sizeof(void *));
  size_t size = 0;
  if (!is_local_address) {
    // Gives us a 1 byte heap array containing the 1 byte before heap_address
    char *size_ptr = RemoteMemory::Copy(pid, size_location, sizeof(void *));
    size = *(reinterpret_cast<size_t *>(size_ptr));
    memcpy(&size, size_ptr, sizeof(void *));
    delete[] size_ptr;
  } else {
    // We are getting the size of an object in a deep copy we already have.
    size = *size_location;
  }

  // The 3 least significant bits of the size are flags not the actual size
  // glibc can do this as all allocations are a multiple of 8 so the 3 least
  // significant bits are unused So a 32 bit allocation would be 39 if all flags
  // are set We floor the size to the nearest 8 to get the actual size of the block
  const auto flags = size % 8;

  // Drop the 3 least significant bits. They are flags not the allocated size
  size = (size / 8) * 8;

  if (print_warnings) {
    if (size == 0) {
      cerr << "WARNING: malloc'd size reported as 0 for address: " << heap_address << '\n';
    }
    if (size % 8 != 0) {
      cerr << "WARNING: malloc'd size not reported as a multiple of 8 for address: " << heap_address
	   << '\n';
    }

    // Minimum heap chunk in glibc is 4 * (sizeof(void*))
    // Even when the target program runs "new char[8]" a 32 byte allocation is made
    const auto kMinimumChunkSize = 4 * sizeof(void *);
    if (size < kMinimumChunkSize) {
      cerr << "WARNING: malloc'd size reported as less than the minimum glibc should allocate, "
	      "4*sizeof(void*). Reported size: "
	   << size << " at address: " << heap_address << '\n';
    }
  }
  if (size > max_size) {
    if (print_warnings) {
      cerr << "WARNING: malloc'd size reported as more than the limit. Reported size:" << size
	   << " limit:" << max_size << '\n';
    }
    size = 0;
  }

  return size;
}
