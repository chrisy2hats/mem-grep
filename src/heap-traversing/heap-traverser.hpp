#ifndef MEMGREP_HEAP_TRAVERSER
#define MEMGREP_HEAP_TRAVERSER
#include "../misc/map-parser.hpp"
#include "../misc/remote-memory.hpp"
#include "../misc/malloc-metadata.hpp"
#include "../misc/utils.hpp"

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <future>

class HeapTraverser {
  public:
  std::vector<RemoteHeapPointer> TraversePointers(std::vector<RemoteHeapPointer> base_pointers);
  size_t static CountPointers(const std::vector<RemoteHeapPointer>& base_pointers);
  HeapTraverser(const pid_t pid, const MapsEntry& heap, const size_t max_heap_obj);
  ~HeapTraverser();

  // TODO move out to non heap-traversal specific RemoteHeapPointer print function in utils.cpp
  void static PrintPointer(const RemoteHeapPointer& p, int indent_level = 0);
  void static PrintHeap(const std::vector<RemoteHeapPointer>& base_pointers);

  private:
  RemoteHeapPointer FollowPointer(RemoteHeapPointer& base);
  void WorkerThread(std::vector<RemoteHeapPointer>& base_pointers,
		  std::atomic<uint64_t>& shared_index);
  [[nodiscard]] inline bool IsHeapAddress(const void* address) const;

  // During traversal the same same memory address must not be visited twice as otherwise
  // We may end up in an infinite loop due to memory address pointing to each other
  // i.e. a circular linked list in the remote process
  // We would recursively follow this forever until we stack overflow and segfault
  // We avoid cycles by marking memory address in our heap copy as we visit them
  // so we know not to analyse that address again

  // The address marking process works as follows
  // sizeof(void*) is 8 bytes which allows for a memory size
  // of up to 2**64 but no current x86_64 processor support more than 2**48 of
  // memory. According to https://www.amd.com/system/files/TechDocs/24593.pdf page 131
  // This means we can use 2 to 22 most significant bits as flags for ourself
  // without touching the actual size of the heap object
  // We can utilse one of these bits to indicate that this address has already been visited
  // And so should not be visited again
  // The 2nd most significant bit is being utilised for this flag.
  // This is explained in more detail with diagrams in the documentation here:
  // TODO add link to project documentation
  constexpr static size_t kAlreadyVisitedFlag =
		  0b0100000000000000000000000000000000000000000000000000000000000000;

  static void SetAlreadyVisited(const void* address);
  [[nodiscard]] bool IsAlreadyVisited(const void* address) const;

  [[nodiscard]] inline void* LocalToRemote(const void* local_address) const;
  [[nodiscard]] inline void* RemoteToLocal(const void* remote_address) const;

  const MapsEntry heap_metadata_;
  const pid_t pid_;
  const size_t max_heap_obj_;

  char* heap_copy_ = nullptr;
};

#endif
