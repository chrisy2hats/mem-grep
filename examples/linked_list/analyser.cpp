#include "../../lib/heap-traversing/stack-searcher.hpp"
#include "../../lib/misc/prerun-checks.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
  if (!CanRun()) {
    exit(2);
  }

  if (argc != 2) {
    std::cout << "Please provide the target programs PID" << std::endl;
    exit(2);
  }

  const pid_t pid = stoi(std::string(argv[1]));
  std::cout << "Analysing " << pid << "\n";

  auto parser = MapParser(pid);
  auto entries = parser.ParseMap();
  const MapsEntry stack = parser.getStoredStack();
  const MapsEntry text = parser.getStoredText();
  const MapsEntry heap = parser.getStoredHeap();
  const MapsEntry bss = parser.getStoredBss();

  auto stackSearch = StackSearcher(stack.start, text, pid, 10000);
  std::vector<RemoteHeapPointer> stack_to_heap_pointers =
		  stackSearch.findHeapPointers(stack.end, heap, 10000);

  std::cout << "Found " << stack_to_heap_pointers.size()
	    << " pointers from the stack to the heap\n";

  auto heap_traverser = HeapTraverser(pid, parser.getStoredHeap(), 250000);

  auto traversed = heap_traverser.TraversePointers(stack_to_heap_pointers);

  RemoteHeapPointer ll_head;
  size_t most_subpointers = 0;

  for (const auto& base_pointer : traversed) {
    if (base_pointer.total_sub_pointers > most_subpointers) {
      most_subpointers = base_pointer.total_sub_pointers;
      ll_head = base_pointer;
    }
  }

  auto value_substitutions = std::vector<Substitution>{Substitution{.from = 0, .to = 100000}};
  std::cout << "Modifiy data in head of linked list malloc'd at " << ll_head.points_to << "\n";
  RemoteMemory::Substitute(pid, ll_head, value_substitutions);

  return 0;
}
