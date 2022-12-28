#ifndef MEMGREP_STACK_SEARCHER
#define MEMGREP_STACK_SEARCHER
#include <vector>
#include <limits>
#include "bss-searcher.hpp"
#include "../misc/map-parser.hpp"
#include "../misc/utils.hpp"

class StackSearcher {
  public:
  // Search the stack for pointers to the heap
  // Searching 0 frames means search the entire stack
  static std::vector<RemoteHeapPointer> findHeapPointers(pid_t pid, const ParsedMaps& maps,
		  size_t max_heap_obj, size_t frames_to_search = 0);
};
#endif	// MEMGREP_STACK_SEARCHER
