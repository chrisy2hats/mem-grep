#ifndef MEMGREP_STACK_SEARCHER
#define MEMGREP_STACK_SEARCHER
#include <vector>
#include <limits>
#include "bss-searcher.hpp"
#include "../misc/map-parser.hpp"
#include "../misc/utils.hpp"

class StackSearcher {
 public:
//	 std::vector<RemoteHeapPointer> static findHeapPointers(const pid_t pid,
//			 const MapsEntry &stack, const MapsEntry &heap, size_t frames_to_search = 0);
  // Stack end may change during lifetime of object so shouldn't be in constructor
  // Same for the heap start and end
  StackSearcher(const void *stackStart, const MapsEntry &text, pid_t pid, size_t max_heap_obj);

  // Searching 0 frames means search the entire stack
  std::vector<RemoteHeapPointer> findHeapPointers(
		  const void *current_stack_end, const MapsEntry &heap, size_t frames_to_search = 0) const;
 private:
  const void *stack_start_;
  const void *text_start_;
  const void *text_end_;
  const size_t text_size_;
  const pid_t pid_;
  const size_t max_heap_obj_;
  [[nodiscard]] bool AddressIsInText(const void *address) const;
  [[nodiscard]] bool AddressIsOnHeap(
		  const void *address, const void *heap_start, const void *heap_end) const;
};
#endif	// MEMGREP_STACK_SEARCHER
