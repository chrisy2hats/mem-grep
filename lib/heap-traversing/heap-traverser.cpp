#include "heap-traverser.hpp"

using std::cerr;
using std::cout;

HeapTraverser::HeapTraverser(const pid_t pid, const MapsEntry& heap, const size_t max_heap_obj) :
		heap_metadata_(heap),
		pid_(pid),
		max_heap_obj_(max_heap_obj),
		visited_storage_(BoolVec(heap_metadata_.size))
{
}

HeapTraverser::~HeapTraverser() {
  delete[] heap_copy_;
}

std::vector<RemoteHeapPointer> HeapTraverser::TraversePointers(
		std::vector<RemoteHeapPointer> base_pointers) {
  if (heap_copy_ == nullptr) {
    //Cleaned up in object this->destructor
    heap_copy_ = RemoteMemory::Copy(pid_, heap_metadata_.start, heap_metadata_.size);
  }
  if (base_pointers.empty()) {
    cerr << "WARNING: HeapTraverser asked to traverse a empty list of pointers.\n";
    return {};
  }
  // As we are traversing a tree our memory accesses are not predictable to the CPU so
  // our threads are encountering many TLB misses and getting swapped out so having threads
  // than cores is, in most scenarios, beneficial for performance.
  constexpr auto kThreadsPerCore = 2;

  const unsigned int hardware_thread_count = std::thread::hardware_concurrency() * kThreadsPerCore;
  std::vector<std::future<void>> thread_futures(hardware_thread_count);

  // std::launch::async requires a lambda or a std::function.
  // It won't accept just a function pointer or the member function
  auto callable_worker = [this](std::vector<RemoteHeapPointer>& a, std::atomic<uint64_t>& b) {
    WorkerThread(a, b);
  };

  // Using std::atomic guantees that 2 threads won't traverse the same base pointer
  // As even if they attempt to increment the index variable at the same time
  // They will get different indexs back
  std::atomic<uint64_t> shared_index(0);

  for (auto& one_threads_future : thread_futures) {
    one_threads_future = std::async(std::launch::async, callable_worker, std::ref(base_pointers),
		    std::ref(shared_index));
  }
  // Wait for all threads to complete
  // Each .wait call will block indefinitely until that thread terminates itself
  for (const auto& future : thread_futures) {
    future.wait();
  }

  return base_pointers;
}

RemoteHeapPointer HeapTraverser::FollowPointer(RemoteHeapPointer& base) {
  const char* const block_start = (char*)RemoteToLocal(base.points_to);
  void* current_8_bytes;

  base.contains_pointers_to.reserve(16);
  for (size_t i = 0; i < base.size_pointed_to; i += sizeof(void*)) {
    memcpy(&current_8_bytes, block_start + i, sizeof(void*));

    if (IsHeapAddress(current_8_bytes)) {

      const auto pointer_location = (void**)(block_start + i);
      const auto address_pointed_to = (void*)*(size_t*)pointer_location;
      const auto local_address_pointed_to = RemoteToLocal(address_pointed_to);

      const size_t pointed_to_size =
		      GetMallocMetadata(local_address_pointed_to, pid_, max_heap_obj_, false, true);

      if (IsAlreadyVisited(address_pointed_to))
	continue;
      else
    SetAlreadyVisited(address_pointed_to);

      if (pointed_to_size == 0 || pointed_to_size > max_heap_obj_) continue;

      void* actual_address = AddToVoid(base.points_to,i);
      const RemoteHeapPointer child_pointer = {actual_address,
		      address_pointed_to,
		      pointed_to_size,
		      0,
		      {}
      };
      base.contains_pointers_to.push_back(child_pointer);
    }
  }
  // This loop could be combined with the loop above however finding all the child pointers
  // then traversing the found child pointers is a more cache friendly approach as we iterate over
  // the memory pointed to by block_start in a predictable way.
  for (auto& ptr : base.contains_pointers_to) {
    ptr = FollowPointer(ptr);
    base.total_sub_pointers += ptr.total_sub_pointers;
  }

  base.total_sub_pointers += base.contains_pointers_to.size();
  return base;
}

void HeapTraverser::WorkerThread(std::vector<RemoteHeapPointer>& base_pointers,
                                 std::atomic<uint64_t>& shared_index) {
  size_t current_local_index = 0;
  while ((current_local_index = shared_index++) < base_pointers.size()) {
    // We MUST save the value when we increment shared_index and only use the returned value
    // if we reference shared_index directly it could have been incremented by a different
    // thread. i.e. "base_pointers.at(shared_index)=..." is unsafe and a race condition but
    // "base_pointers.at(current_local_index) is safe

    base_pointers.at(current_local_index) =
        HeapTraverser::FollowPointer(base_pointers.at(current_local_index));
  }
}

// Checks if the pointer points to an address in the remote processes heap
[[nodiscard]] inline bool HeapTraverser::IsHeapAddress(const void* address) const {
  const bool is_on_heap = address >= heap_metadata_.start && address <= heap_metadata_.end;
  return is_on_heap;
}

// Look in the header file for an explanation
[[nodiscard]] inline size_t HeapTraverser::CalculateBitVecOffset(const void* address) const{
  const size_t heap_offset = (size_t)address - (size_t)heap_metadata_.start;
  const size_t byte_offset = heap_offset/(BITS_IN_A_BYTE*(sizeof(void*)));
  const uint8_t bit_into_byte = (heap_offset/BITS_IN_A_BYTE) % 8;
  return byte_offset + bit_into_byte;
}

void HeapTraverser::SetAlreadyVisited(const void* address) {
  visited_storage_.set_bit(CalculateBitVecOffset(address));
}

[[nodiscard]] bool HeapTraverser::IsAlreadyVisited(const void* address) const {
  return visited_storage_.is_set(CalculateBitVecOffset(address));
}

[[nodiscard]] inline void* HeapTraverser::LocalToRemote(const void* local_address) const {
  const auto offset = (size_t)SubFromVoid(local_address,heap_copy_);
  void* remote_address = AddToVoid(heap_metadata_.start,offset);
  return remote_address;
}

[[nodiscard]] inline void* HeapTraverser::RemoteToLocal(const void* remote_address) const {
  const auto offset = (size_t) SubFromVoid(remote_address,heap_metadata_.start);
  void* local_address = AddToVoid(heap_copy_,offset);
  return local_address;
}
