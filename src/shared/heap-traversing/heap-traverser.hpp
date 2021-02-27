#ifndef MEMGREP_HEAP_TRAVERSER
#define MEMGREP_HEAP_TRAVERSER
#include "../misc/map-parser.hpp"
#include "../misc/remote-memory.hpp"
#include "../misc/malloc-metadata.hpp"
#include "../misc/structs.hpp"
#include "../misc/utils.hpp"

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <future>

class HeapTraverser {
 public:
  std::vector<RemoteHeapPointer> TraversePointers(std::vector<RemoteHeapPointer> base_pointers);
  HeapTraverser(const pid_t pid, const MapsEntry& heap, const size_t max_heap_obj);
  ~HeapTraverser();

  //Don't allow copying
  //If copying is needed in the future be careful with the raw ptr to heap_copy_
  HeapTraverser operator=(const HeapTraverser&) = delete;
  HeapTraverser(const HeapTraverser&) = delete;

 private:
  RemoteHeapPointer FollowPointer(RemoteHeapPointer& base);
  void WorkerThread(std::vector<RemoteHeapPointer>& base_pointers,
		  std::atomic<uint64_t>& shared_index);
  [[nodiscard]] inline bool IsHeapAddress(const void* address) const;

  // During traversal the same same memory address must not be visited twice as otherwise
  // We may end up in an infinite loop due to memory address pointing to each other
  // i.e. a circular linked list in the remote process
  // We would recursively follow this forever until we stack overflow and segfault
  // We avoid cycles by storing memory address we have already visited

  void SetAlreadyVisited(const void* address);
  [[nodiscard]] bool IsAlreadyVisited(const void* address) const;

  [[nodiscard]] inline void* LocalToRemote(const void* local_address) const;
  [[nodiscard]] inline void* RemoteToLocal(const void* remote_address) const;

  const MapsEntry heap_metadata_;
  const pid_t pid_;
  const size_t max_heap_obj_;

  // Store which memory addresses within the heap have already been visited
  // TODO reduce the size of this by storing only every MINIMUM_OBJECT_SIZE. Check all objects are aligned to minimum object size
  // If not we can still reduce it as it is unlikely that a malloc implementation will return unaligned pointers so we probably only need to store
  // if one in every 8 bytes is visited
  using BitVector = std::vector<bool>;
  BitVector visited_storage_;

  char* heap_copy_ = nullptr;
};

#endif
