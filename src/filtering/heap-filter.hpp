#ifndef MEMGREP_HEAP_FILTER
#define MEMGREP_HEAP_FILTER
#include "../misc/structs.hpp"
#include <future>

// Class providing functions to filter the extracted heap by any given condition similar to
// std::remove_if or filter functions from functional languages.

// This class doesn't provide the code which performs the comparisons to match or not match
// individual RemoteHeapPointers it only contains the code to walk the heap data structure.
// The comparision code is passed in as a callable i.e. a lambda
class HeapFilter {
 public:
  // All RemoteHeapPointers that meet the criteria specified are returned in one single vector.
  // The hierarchy of the heap is lost
  // This function uses all available threads the hardware has
  static std::vector<RemoteHeapPointer> FlattenAndFilter(
		  const std::vector<RemoteHeapPointer>& base_pointers,
		  const std::function<bool(const RemoteHeapPointer&)>& functor);

  // Same as the above function except it only uses a single thread.
  // It may be faster if operating on a very small heap
  static std::vector<RemoteHeapPointer> FlattenAndFilterSingleThreaded(
		  const std::vector<RemoteHeapPointer>& base_pointers,
		  const std::function<bool(const RemoteHeapPointer&)>& functor);

 private:
  // Recursively follow a single pointer finding matches
  // The function is void as the matches are inserted in the vector passed in
  // This avoids creating a vector for every call to the function and returning it
  static void FilterPointer(const RemoteHeapPointer& ptr,
		  const std::function<bool(const RemoteHeapPointer&)>& functor,
		  std::vector<RemoteHeapPointer>& matches);

  // The function passed to the child worker threads. It simply goes through the base
  // pointers running FilterPointer on them
  static std::vector<RemoteHeapPointer> WorkerThread(
		  const std::vector<RemoteHeapPointer>& base_pointers,
		  std::atomic<uint64_t>& shared_index,
		  const std::function<bool(const RemoteHeapPointer&)>& functor);
};
#endif	// MEMGREP_HEAP_FILTER
