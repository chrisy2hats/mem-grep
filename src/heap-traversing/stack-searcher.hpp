#ifndef MEMGREP_STACK_SEARCHER
#define MEMGREP_STACK_SEARCHER
#include <vector>
#include "bss-searcher.hpp"
#include "../misc/map-parser.hpp"

class StackSearcher {
 public:
  // Stack end may change during lifetime of object so shouldn't be in constructor
  // Same for the heap start and end
  StackSearcher(const void *stackStart, const MapsEntry &text, pid_t pid, size_t max_heap_obj);

  // Searching 0 frames means search the entire stack
  std::vector<RemoteHeapPointer> findHeapPointers(
		  const void *curStackEnd, const MapsEntry &heap, size_t framesToSearch = 0) const;
 private:
  const void *stackStart_;
  const void *textStart_;
  const void *textEnd_;
  const size_t textSize_;
  const pid_t pid_;
  const size_t max_heap_obj_;
  [[nodiscard]] bool AddrIsInText(const void *addr) const;
  [[nodiscard]] bool AddrIsOnHeap(
		  const void *addr, const void *heapStart, const void *heapEnd) const;
};
#endif	// MEMGREP_STACK_SEARCHER
