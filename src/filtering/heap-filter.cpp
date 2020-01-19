#include "heap-filter.hpp"

std::vector<RemoteHeapPointer> HeapFilter::FlattenAndFilter(
		const std::vector<RemoteHeapPointer>& base_pointers,
		const std::function<bool(const RemoteHeapPointer&)>& functor) {

  std::vector<RemoteHeapPointer> matches = {};

  for (const auto& ptr : base_pointers) {
    matches = filter_pointer(ptr, functor, matches);
  }
  return matches;
}

std::vector<RemoteHeapPointer> HeapFilter::filter_pointer(const RemoteHeapPointer& ptr,
		const std::function<bool(const RemoteHeapPointer&)>& functor,
		std::vector<RemoteHeapPointer>& matches) {

  if (functor(ptr))
    matches.push_back(ptr);

  for (const RemoteHeapPointer& i : ptr.contains_pointers_to) {
    matches = filter_pointer(i, functor, matches);
  }
  return matches;
}
