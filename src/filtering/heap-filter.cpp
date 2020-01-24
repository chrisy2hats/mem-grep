#include "heap-filter.hpp"

std::vector<RemoteHeapPointer> HeapFilter::FlattenAndFilter(
		const std::vector<RemoteHeapPointer>& base_pointers,
		const std::function<bool(const RemoteHeapPointer&)>& functor) {
  const unsigned int hardware_thread_count = std::thread::hardware_concurrency();
  std::vector<std::future<std::vector<RemoteHeapPointer>>> thread_futures(hardware_thread_count);
  std::vector<RemoteHeapPointer> all_thread_matches = {};

  auto callable_worker =
		  [](const std::vector<RemoteHeapPointer>& base_pointers,
				  std::atomic<uint64_t>& shared_index,
				  const std::function<bool(const RemoteHeapPointer&)>& functor)
		  -> std::vector<RemoteHeapPointer> {
    return WorkerThread(base_pointers, shared_index, functor);
  };

  std::atomic<uint64_t> shared_index(0);
  for (auto& one_threads_future : thread_futures) {
    one_threads_future = std::async(std::launch::async, callable_worker, std::ref(base_pointers),
		    std::ref(shared_index), functor);
  }
  for (auto& future : thread_futures) {
    const std::vector<RemoteHeapPointer> futures_matches = future.get();
    all_thread_matches.insert(
		    all_thread_matches.end(), futures_matches.begin(), futures_matches.end());
  }

  return all_thread_matches;
}

std::vector<RemoteHeapPointer> HeapFilter::FlattenAndFilterSingleThreaded(
		const std::vector<RemoteHeapPointer>& base_pointers,
		const std::function<bool(const RemoteHeapPointer&)>& functor) {
  std::vector<RemoteHeapPointer> matches = {};

  for (const auto& ptr : base_pointers) {
    FilterPointer(ptr, functor, matches);
  }
  return matches;
}

void HeapFilter::FilterPointer(const RemoteHeapPointer& ptr,
		const std::function<bool(const RemoteHeapPointer&)>& functor,
		std::vector<RemoteHeapPointer>& matches) {
  if (functor(ptr))
    matches.push_back(ptr);

  for (const RemoteHeapPointer& i : ptr.contains_pointers_to) {
    FilterPointer(i, functor, matches);
  }
}

std::vector<RemoteHeapPointer> HeapFilter::WorkerThread(
		const std::vector<RemoteHeapPointer>& base_pointers,
		std::atomic<uint64_t>& shared_index,
		const std::function<bool(const RemoteHeapPointer&)>& functor) {
  std::vector<RemoteHeapPointer> thread_local_matches = {};

  // We must not reference shared_index in the loop as it could have changed in a different thread.
  // i.e. "base_pointers.at(shared_index)=..." is unsafe and a race condition but
  // "base_pointers.at(current_local_index) is safe
  size_t current_local_index = 0;
  while ((current_local_index = shared_index++) < base_pointers.size()) {
    FilterPointer(base_pointers.at(current_local_index), functor, thread_local_matches);
  }
  return thread_local_matches;
}
