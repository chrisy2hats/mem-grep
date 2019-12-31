#include "heap-traverser.hpp"

using std::cout;
using std::endl;

std::vector<RemoteHeapPointer> HeapTraverser::TraverseHeapPointers( const struct MAPS_ENTRY &heap,
		std::vector<RemoteHeapPointer> base_pointers, const pid_t &pid,
		const size_t max_heap_obj)
{
  std::vector<void *> already_visited = {};
  for (auto& pointer : base_pointers) {
    pointer = HeapTraverser::FollowPointer(heap, pointer, pid, max_heap_obj, already_visited);
  }

  return base_pointers;
}

RemoteHeapPointer HeapTraverser::FollowPointer(const struct MAPS_ENTRY &heap,
		struct RemoteHeapPointer &base, const pid_t &pid,
		const size_t max_heap_obj, std::vector<void *> &already_visited)
{
  const char *block_pointed_to = DeepCopy(pid, base.pointsTo, base.sizePointedTo);
  void *current = nullptr;
  std::vector<RemoteHeapPointer> current_level_pointers = {};
  for (size_t j = 0; j < base.sizePointedTo; j += (sizeof(void *))) {
    memcpy(&current, block_pointed_to + j, sizeof(void *));

    if (AddressIsOnHeap(current, heap.start, heap.end)) {
      void* actual_address = (char *) base.pointsTo + j;
      const auto pointer_location = (void **)(block_pointed_to + j);
      const auto address_pointed_to = (void *)*pointer_location;
      const size_t pointed_to_size = GetMallocMetadata(address_pointed_to, pid, max_heap_obj);

      // Checking if a pointer is already visited is required as otherwise if two pointers
      // Point to each other (even indirectly) then this function will get stuck in an infinite loop
      // Until we get a stack overflow and the program crashes!
      if (std::find(already_visited.begin(), already_visited.end(), address_pointed_to) != already_visited.end()) {
	continue;
      }
      already_visited.push_back(address_pointed_to);

      struct RemoteHeapPointer p = {actual_address, address_pointed_to, pointed_to_size, 0, {}};
      current_level_pointers.push_back(p);
    }
  }
  delete[] block_pointed_to;
  for (auto &j : current_level_pointers) {
    const struct RemoteHeapPointer p = FollowPointer(heap, j, pid, max_heap_obj, already_visited);
    base.totalSubPointers += p.totalSubPointers;
    base.containsPointersTo.push_back(p);
  }
  base.totalSubPointers += current_level_pointers.size();
  return base;
}

[[nodiscard]] bool HeapTraverser::AddressIsOnHeap(
		const void *address, const void *heap_start, const void *heap_end)
{
  const bool is_on_heap = address >= heap_start && address <= heap_end;
  return is_on_heap;
}

size_t HeapTraverser::CountHeapPointers(
		const std::vector<RemoteHeapPointer> &basePointers)
{
  auto total = 0;
  for (const auto &i : basePointers) {
    total += i.totalSubPointers;
  }
  total += basePointers.size();
  return total;
}

void HeapTraverser::PrintPointer(const RemoteHeapPointer& p, int indent_level/*=0*/)
{
  for (const auto &i : p.containsPointersTo) {
    if (i.totalSubPointers != 0 && i.sizePointedTo > 64) {
      for (int j = 0; j < indent_level; j++) {
	std::cout << "\t";
      }
      std::cout << "\t" << i.pointsTo << " : " << i.actualAddress << " : "
		<< i.sizePointedTo << ":" << i.totalSubPointers << "\n";
    }
    HeapTraverser::PrintPointer(i, indent_level + 1);
  }
}

void HeapTraverser::PrintHeap(
		const std::vector<RemoteHeapPointer> &base_pointers)
{
  for (const auto& p : base_pointers) {
    if (p.sizePointedTo < 64) {
      if (p.totalSubPointers != 0) {
	std::cout << "BASE:" << p.pointsTo << " : " << p.actualAddress << " : "
		  << p.sizePointedTo << " : " << p.totalSubPointers << "\n";
      }
    }
    HeapTraverser::PrintPointer(p);
  }
}
