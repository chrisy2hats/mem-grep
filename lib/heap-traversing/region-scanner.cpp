#include "region-scanner.hpp"


std::vector<RemoteHeapPointer> RegionScanner::FindHeapPointers(pid_t pid, const MapsEntry &heap,
		const MapsEntry &scanned_region, size_t max_heap_obj) {
  const RemoteObject kRegionCopy =
		  RemoteMemory::RemoteCopy(pid, scanned_region.start, scanned_region.size);

  auto pointers_to_heap = std::vector<RemoteHeapPointer>();
  for (size_t i = 0; i < scanned_region.size; i += (sizeof(void *))) {
    size_t current = 0;
    memcpy(&current, (char *)kRegionCopy.data + i, sizeof(void *));
    if (current == 0) {
      continue;
    }

    /*If our target programs source has a function that has a pointer to a heap object
    Like
	int* x = new int;
    then pointer_location is &x and address_pointed_to is x
    */
    const auto pointer_location = (void **)AddToVoid(kRegionCopy.data, i);
    const auto address_pointed_to = (void *)*pointer_location;

    if (heap.contains_addr(address_pointed_to)) {
      size_t size_pointed_to = 0;

      // We have to handle pointers to the first byte of the heap differently
      // If we try and access the byte before the pointer to get the size via GetMallocMetadata
      // We may be accessing memory that doesn't belong to the same segment which can cause this
      // program to SEGFAULT as that memory may not belong to the PID specified to process_vm_readv
      if (address_pointed_to != heap.start) {
	size_pointed_to = GetMallocMetadata(address_pointed_to, pid, max_heap_obj);
	if (size_pointed_to == 0 || size_pointed_to > max_heap_obj)
	  continue;
      }
      void *const actual_address = AddToVoid(scanned_region.start, i);
      const RemoteHeapPointer heap_pointer = {
		      actual_address, address_pointed_to, size_pointed_to, 0, {}};

      pointers_to_heap.push_back(heap_pointer);
    }
  }
  return pointers_to_heap;
}
