#include "heap-traverser.hpp"

using std::cout;
using std::cerr;

HeapTraverser::HeapTraverser(const pid_t pid,const MAPS_ENTRY& heap,const size_t max_heap_obj):
		heap_metadata_(heap),
		pid_(pid),
		max_heap_obj_(max_heap_obj)
{}

HeapTraverser::~HeapTraverser(){
  delete[] heap_copy_;
}

size_t HeapTraverser::CountPointers( const std::vector<RemoteHeapPointer>& basePointers)
{
  size_t total = 0;
  for (const auto &i : basePointers) {
    total += i.total_sub_pointers;
  }
  total += basePointers.size();
  return total;
}

void HeapTraverser::PrintPointer(const RemoteHeapPointer& p, int indent_level/*=0*/)
{
  for (const auto &i : p.contains_pointers_to) {
    for (int j = 0; j < indent_level; j++) {
      cout << "\t";
    }
    cout << "\t" << i.points_to << " : " << i.actual_address << " : "
	 << i.size_pointed_to << ":" << i.total_sub_pointers << "\n";
    HeapTraverser::PrintPointer(i, indent_level + 1);
  }
}

void HeapTraverser::PrintHeap(
		const std::vector<RemoteHeapPointer> &base_pointers)
{
  for (const auto& p : base_pointers) {
    cout << "BASE:" << p.points_to << " : " << p.actual_address << " : "
	 << p.size_pointed_to
	 << " : " << p.total_sub_pointers << "\n";
    HeapTraverser::PrintPointer(p);
  }
}

void HeapTraverser::WorkerThread (std::vector<RemoteHeapPointer>& base_pointers,std::atomic<uint64_t>& shared_index){
	while (shared_index < base_pointers.size()){
	  // We MUST save the value when we increment shared_index and only use the returned value
	  // if we reference shared_index directly it could have been incremented by a different
	  // thread. i.e. "base_pointers.at(shared_index)=..." is unsafe and a race condition but
	  // "base_pointers.at(current_local_index) is safe
	  const size_t current_local_index = shared_index++;

	  base_pointers.at(current_local_index) =
			  HeapTraverser::FollowPointer(base_pointers.at(current_local_index));
	}
}

std::vector<RemoteHeapPointer> HeapTraverser::TraversePointers(std::vector<RemoteHeapPointer> base_pointers){
  if (heap_copy_==nullptr){
    heap_copy_ = RemoteMemory::Copy(pid_,heap_metadata_.start,heap_metadata_.size);
  }
  if (base_pointers.empty()) {
    cerr << "WARNING: HeapTraverser asked to traverse a empty list of pointers.\n";
    return {};
  }
  const unsigned int hardware_thread_count = std::thread::hardware_concurrency();
  std::vector<std::future<void>> thread_futures(hardware_thread_count);

  // std::launch::async requires a lambda or a std::function.
  // It won't accept just a function pointer or the member function
  auto callable_worker = [this] (std::vector<RemoteHeapPointer>& a,std::atomic<uint64_t>& b){
    WorkerThread(a,b);
  };

  // Using std::atomic guantees that 2 threads won't traverse the same base pointer
  // As even if they attempt to increment the index variable at the same time
  // They will get different indexs back
  std::atomic<uint64_t> shared_index(0);

  for (auto& one_threads_future : thread_futures) {
    one_threads_future = std::async(std::launch::async, callable_worker, std::ref(base_pointers),
		    std::ref(shared_index));
  }

  //Wait for all threads to complete
  // Each .wait call will block indefinitely until that thread terminates itself
  for (const auto& future : thread_futures) {
    future.wait();
  }

  return base_pointers;
}

RemoteHeapPointer HeapTraverser::FollowPointer(RemoteHeapPointer& base){
  const char* block_start =(char*)RemoteToLocal(base.points_to);
  void* current_8_bytes;
  std::vector<RemoteHeapPointer> current_level_pointers = {};
  current_level_pointers.reserve(16);
  for (size_t i=0;i<base.size_pointed_to;i+=sizeof(void*)){
    memcpy(&current_8_bytes, block_start +i,sizeof(void*));

    if (IsHeapAddress(current_8_bytes)) {
      void* actual_address = (char*)base.points_to + i;
      const auto pointer_location = (void**)(block_start + i);
      const auto address_pointed_to = (void*)*(size_t*)pointer_location;
      const auto local_address_pointed_to = RemoteToLocal(address_pointed_to);

      const size_t pointed_to_size = GetMallocMetadata(local_address_pointed_to,
		      pid_, max_heap_obj_, false, true);

      if (IsAlreadyVisited(local_address_pointed_to))
	continue;
      else
	SetAlreadyVisited(local_address_pointed_to);

      current_level_pointers.push_back({
                      .actual_address = actual_address,
		      .points_to = address_pointed_to,
		      .size_pointed_to = pointed_to_size,
		      .total_sub_pointers = 0,
		      .contains_pointers_to = {}
      });
    }
  }

  for (auto &j : current_level_pointers) {
    const struct RemoteHeapPointer p = FollowPointer(j);
    base.total_sub_pointers += p.total_sub_pointers;
    base.contains_pointers_to.push_back(p);
  }

  base.total_sub_pointers += current_level_pointers.size();
  return base;
}

[[nodiscard]] inline bool HeapTraverser::IsHeapAddress(const void* address)const{
  const bool is_on_heap = address >= heap_metadata_.start && address <= heap_metadata_.end;
  return is_on_heap;
}

void HeapTraverser::SetAlreadyVisited(const void* address){
  auto size_location = (size_t*) ((char*)address-sizeof(void*));
  assert((char*)size_location == ((char*)address-sizeof(void*)) );
  size_t new_val = *size_location += kAlreadyVisitedFlag;
  *size_location=new_val;
}

bool HeapTraverser::IsAlreadyVisited(const void* address)const{
  const auto size_location = (size_t*) ((char*)address-sizeof(void*));
  const bool already_visited = (*size_location) & kAlreadyVisitedFlag;
  return already_visited;
}


[[nodiscard]] inline void* HeapTraverser::LocalToRemote(const void* local_address) const {
  	const size_t offset = (char*)local_address-heap_copy_;
	void* remote_address = (char*)heap_metadata_.start+offset;
	return remote_address;
}

[[nodiscard]] inline void* HeapTraverser::RemoteToLocal(const void* remote_address) const {
	const size_t offset = (char*)remote_address-(char*)heap_metadata_.start;
	void* local_address = (char*) heap_copy_+offset;
	return local_address;
}
