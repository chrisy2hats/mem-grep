#ifndef MEMGREP_REGION_SCANNER
#define MEMGREP_REGION_SCANNER
#include <vector>
#include <limits>
#include "../misc/map-parser.hpp"
#include "../misc/utils.hpp"
#include "../misc/structs.hpp"
#include "../misc/remote-memory.hpp"
#include "../misc/malloc-metadata.hpp"

// Non-recursively scan a region of memory, i.e. .bss or the stack, for pointers to the heap
class RegionScanner {
  public:
  static std::vector<RemoteHeapPointer> FindHeapPointers(pid_t pid, const MapsEntry& heap,
		  const MapsEntry& scanned_region, size_t max_heap_obj);
};
#endif	// MEMGREP_REGION_SCANNER
