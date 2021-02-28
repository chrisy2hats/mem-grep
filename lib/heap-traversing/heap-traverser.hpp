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

  // During traversal the same memory address must not be visited twice as otherwise
  // We may end up in an infinite loop due to memory address pointing to each other
  // i.e. a circular linked list in the remote process
  // We would recursively follow this forever until we stack overflow and segfault
  // We avoid cycles by storing memory address we have already visited
  
  // We store visited addresses using a BitVec that stores one bit for every sizeof(void*) bytes.
  // So for x86_64 that is one bit for every 8 bytes which is 1/64th of the size of the heap
  // We can store one bit per 8 bytes not for every byte as every sensible malloc implementation out there
  // will only return aligned pointers even for malloc(x where x < sizeof(void*))
  
  // So for instance, if the heap starts at memory address 1000 and we have a pointer to 
  // 1808 then when we visit the object at 1800 we will set the bit 
  // 808/64 + (808/8)%8
  // 12 +  1 = The 13th bit of the vector

  [[nodiscard]] inline size_t CalculateBitVecOffset(const void* address) const;
  void SetAlreadyVisited(const void* address);
  [[nodiscard]] bool IsAlreadyVisited(const void* address) const;

  [[nodiscard]] inline void* LocalToRemote(const void* local_address) const;
  [[nodiscard]] inline void* RemoteToLocal(const void* remote_address) const;

  const MapsEntry heap_metadata_;
  const pid_t pid_;
  const size_t max_heap_obj_;
  
  static constexpr auto BITS_IN_A_BYTE=8;

  
  // We can't use BitVec from <bitvec> as the size must be known at compile time
  using BitVector = std::vector<bool>;
  
  // Store which memory addresses within the heap have already been visited
  BitVector visited_storage_;

  char* heap_copy_ = nullptr;
};

#endif
