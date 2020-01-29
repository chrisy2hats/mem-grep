#include "stack-searcher.hpp"

using std::cout;

StackSearcher::StackSearcher(const void *stackStart, const MapsEntry &text,
		const pid_t pid, const size_t max_heap_obj) :
		stack_start_(stackStart),
		text_start_(text.start),
		text_end_(text.end),
		text_size_((size_t)SubFromVoid(text.start,text.end)),
		pid_(pid),
		max_heap_obj_(max_heap_obj) {}

// If the address is within the .text part of the target binary
// If an address is within the .text it is executable code which means it is the address of a
// function or a return address
[[nodiscard]] bool StackSearcher::AddressIsInText(const void *address) const {
  const bool IsInText = address >= text_start_ && address <= text_end_;
  return IsInText;
}

[[nodiscard]] bool StackSearcher::AddressIsOnHeap(
		const void *address, const void *heap_start, const void *heap_end) const {
  const bool IsOnHeap = address >= heap_start && address <= heap_end;
  return IsOnHeap;
}

std::vector<RemoteHeapPointer> StackSearcher::findHeapPointers(const void *current_stack_end,
		const MapsEntry &heap, size_t frames_to_search) const {
  // Every program has a stack so these should never be null
  assert(stack_start_ != nullptr);
  assert(current_stack_end != nullptr);

  if (frames_to_search == 0)
    frames_to_search = std::numeric_limits<size_t>::max();

  assert(current_stack_end > stack_start_);
  const auto kCurrentStackSize = (size_t)(SubFromVoid(current_stack_end, stack_start_));
  const char *kStackCopy = RemoteMemory::Copy(pid_, stack_start_, kCurrentStackSize);

  size_t frames_searched = 0;
  auto pointers_to_heap = std::vector<RemoteHeapPointer>();
  for (size_t i = 0; i < kCurrentStackSize; i += (sizeof(void *))) {
    size_t current = 0;
    memcpy(&current, (char *)kStackCopy + i, sizeof(void *));
    if (current == 0) {
      continue;
    }

    /*If our target programs source has a function that has a pointer to a heap object
    Like
	int* x = new int;
    then pointer_location is &x and address_pointed_to is x
    */
    const auto pointer_location = (void **)AddToVoid(kStackCopy, i);
    const auto address_pointed_to = (void *)*pointer_location;

    if (AddressIsOnHeap(address_pointed_to, heap.start, heap.end)) {
      size_t size_pointed_to = 0;

      // We have to handle pointers to the first byte of the heap differently
      // If we try and access the byte before the pointer to get the size via GetMallocMetadata
      // We may be accessing memory that doesn't belong to the same segment which can cause this
      // program to SEGFAULT as that memory may not belong to the PID specified to process_vm_readv
      if (address_pointed_to != heap.start) {
	size_pointed_to = GetMallocMetadata(address_pointed_to, pid_, max_heap_obj_);
	if (size_pointed_to == 0 || size_pointed_to > max_heap_obj_)
	  continue;
      }
      void *actual_address = AddToVoid(stack_start_, i);
      const RemoteHeapPointer heap_pointer = {.actual_address = actual_address,
		      .points_to = address_pointed_to,
		      .size_pointed_to = size_pointed_to,
		      .contains_pointers_to = {},
		      .total_sub_pointers = 0};

      pointers_to_heap.push_back(heap_pointer);

    } else if (AddressIsInText(address_pointed_to)) {
      frames_searched++;
      if (frames_searched > frames_to_search) {
	break;
      }
    }
  }
  delete[] kStackCopy;
  return pointers_to_heap;
}
