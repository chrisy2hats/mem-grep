#ifndef MEMGREP_HEAP_FILTER
#define MEMGREP_HEAP_FILTER
#include "../misc/utils.hpp"

class HeapFilter{
 public:
  // All RemoteHeapPointers that meet the criteria specified are returned in one single vector.
  // The hierarchy of the heap is lost
  static std::vector<RemoteHeapPointer> FlattenAndFilter(const std::vector<RemoteHeapPointer>& base_pointers,
  		const std::function<bool(const RemoteHeapPointer&)>& functor);

 private:
	static std::vector<RemoteHeapPointer> filter_pointer(const RemoteHeapPointer& ptr,
			const std::function<bool(const RemoteHeapPointer&)>& functor,
			std::vector<RemoteHeapPointer>& matches);
};
#endif // MEMGREP_HEAP_FILTER
