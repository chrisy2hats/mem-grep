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
    total += i.totalSubPointers;
  }
  total += basePointers.size();
  return total;
}

void HeapTraverser::PrintPointer(const RemoteHeapPointer& p, int indent_level/*=0*/)
{
  for (const auto &i : p.containsPointersTo) {
    for (int j = 0; j < indent_level; j++) {
      cout << "\t";
    }
    cout << "\t" << i.pointsTo << " : " << i.actualAddress << " : "
	 << i.sizePointedTo << ":" << i.totalSubPointers << "\n";
    HeapTraverser::PrintPointer(i, indent_level + 1);
  }
}

void HeapTraverser::PrintHeap(
		const std::vector<RemoteHeapPointer> &base_pointers)
{
  for (const auto& p : base_pointers) {
    cout << "BASE:" << p.pointsTo << " : " << p.actualAddress << " : "
	 << p.sizePointedTo << " : " << p.totalSubPointers << "\n";
    HeapTraverser::PrintPointer(p);
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
  for (auto& pointer : base_pointers) {
    pointer = HeapTraverser::FollowPointer(pointer);
  }

  return base_pointers;
}

RemoteHeapPointer HeapTraverser::FollowPointer(RemoteHeapPointer& base){
  const char* block_start =(char*)RemoteToLocal(base.pointsTo);
  void* current_8_bytes;
  std::vector<RemoteHeapPointer> current_level_pointers = {};
  current_level_pointers.reserve(100);
  for (size_t i=0;i<base.sizePointedTo;i+=sizeof(void*)){
    memcpy(&current_8_bytes, block_start +i,sizeof(void*));

    if (IsHeapAddress(current_8_bytes)) {
      void* actual_address = (char*)base.pointsTo + i;
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
                      .actualAddress = actual_address,
		      .pointsTo = address_pointed_to,
		      .sizePointedTo = pointed_to_size,
		      .totalSubPointers = 0,
		      .containsPointersTo = {}
      });
    }
  }

  for (auto &j : current_level_pointers) {
    const struct RemoteHeapPointer p = FollowPointer(j);
    base.totalSubPointers += p.totalSubPointers;
    base.containsPointersTo.push_back(p);
  }

  base.totalSubPointers += current_level_pointers.size();
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
