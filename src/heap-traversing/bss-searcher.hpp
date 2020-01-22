#ifndef MEMGREP_BSSSEARCHER
#define MEMGREP_BSSSEARCHER
#include "../misc/map-parser.hpp"
#include "../misc/remote-memory.hpp"
#include "../misc/malloc-metadata.hpp"
#include "../misc/utils.hpp"
#include "heap-traverser.hpp"

#include <sys/uio.h>
#include <iostream>
#include <cstring>
#include <cassert>

// Iterate over the BSS section to find values which when treated as an address would point
// to the remote processes heap
class BssSearcher {
  public:
  [[nodiscard]] std::vector<RemoteHeapPointer> FindHeapPointers(const MAPS_ENTRY &heap) const;
  // Heap start and heap end can't be constructor parameters as they may changed throughout the
  // lifetime of the object. The heap can grow and shrink at any time The caller MAY have a more up
  // to date size of the heap Bss Size and location however won't change during runtime as .bss is a
  // fixed size at compile time of the remote program
  BssSearcher(const MAPS_ENTRY &bss_metadata, const pid_t &pid, size_t max_heap_obj);

  private:
  [[nodiscard]] inline bool AddressIsOnHeap( const void *address,const MAPS_ENTRY& heap_metadata) const;
  const pid_t pid_;
  const size_t max_heap_obj_;
  const MAPS_ENTRY bss_metadata_;
};
#endif