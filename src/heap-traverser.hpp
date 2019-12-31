#ifndef MEMGREP_HEAP_TRAVERSER
#define MEMGREP_HEAP_TRAVERSER
#include "map-parser.hpp"
#include "deep-copy.hpp"
#include "malloc-metadata.hpp"
#include "utils.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

class HeapTraverser
{
  public:
  std::vector<RemoteHeapPointer> static TraverseHeapPointers( const struct MAPS_ENTRY& heap, std::vector<RemoteHeapPointer> base_pointers, const pid_t& pid, const size_t max_heap_obj);
  size_t static CountHeapPointers(const std::vector<RemoteHeapPointer>& basePointers);
  void static PrintHeap(const std::vector<RemoteHeapPointer>& base_pointers);

  protected:
  private:
  RemoteHeapPointer static FollowPointer(const struct MAPS_ENTRY& heap, struct RemoteHeapPointer& base, const pid_t& pid, const size_t max_heap_obj, std::vector<void*>& already_visited);
  [[nodiscard]] bool static AddressIsOnHeap(const void* address, const void* heap_start, const void* heap_end);
  void static PrintPointer(const RemoteHeapPointer& p, int indent_level = 0);
};
#endif
