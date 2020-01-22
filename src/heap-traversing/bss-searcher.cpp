#include "bss-searcher.hpp"

using std::cout;
using std::cerr;

BssSearcher::BssSearcher(const MAPS_ENTRY& bss_metadata,const pid_t& pid,const size_t max_heap_obj)
        : pid_(pid),
        max_heap_obj_(max_heap_obj),
        bss_metadata_(bss_metadata) {
  if (bss_metadata_.start == nullptr || bss_metadata_.end == nullptr) {
    cerr << "WARNING: BssSearcher initialised with a nullptr. All searches will return 0 results\n";
  }
}

[[nodiscard]] std::vector<RemoteHeapPointer> BssSearcher::FindHeapPointers(const MAPS_ENTRY &heap_metadata) const{
  if (bss_metadata_.start == nullptr || bss_metadata_.end == nullptr){
    // No need to print error message as the constructor will have notified if any parameter is null
    return {};
  }

  const char* bss_copy = RemoteMemory::Copy(pid_, bss_metadata_.start, bss_metadata_.size);
  assert(bss_copy !=nullptr);

  std::vector<RemoteHeapPointer> matches;
  for (size_t i = 0; i < bss_metadata_.size; i += (sizeof(void *))) {
    size_t current = 0;
    memcpy(&current, bss_copy + i, sizeof(void *));

    // If our target programs source has, in global scope, "int* i = new int;"
    // kPointerLocation will be &i
    // And kAddressPointedTo will be i (The memory address potentially on the heap
    // Except within our deepcopy of the processes .bss section not the
    // actual .bss section
    const auto kPointerLocation = (void **) (bss_copy + i);
    const auto kAddressPointedTo = (void *) *kPointerLocation;

    if (AddressIsOnHeap(kAddressPointedTo, heap_metadata)) {
      void *actual_address = (void*) ((char*)bss_metadata_.start + i);
      const size_t kSizePointedTo = GetMallocMetadata(kAddressPointedTo, pid_, max_heap_obj_);

      const struct RemoteHeapPointer matching_pointer = {
		      .actual_address = actual_address,
		      .points_to = kAddressPointedTo,
		      .size_pointed_to = kSizePointedTo,
		      .total_sub_pointers = 0,
		      .contains_pointers_to = {}
      };
      matches.push_back(matching_pointer);
    }
  }
  delete[] bss_copy;
  return matches;
}

[[nodiscard]] inline bool BssSearcher::AddressIsOnHeap(const void *address,const MAPS_ENTRY& heap_metadata) const {
    const bool IsOnHeap = address >= heap_metadata.start && address <= heap_metadata.end;
    return IsOnHeap;
}
