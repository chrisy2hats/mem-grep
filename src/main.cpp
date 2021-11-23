#include "../../lib/heap-traversing/stack-searcher.hpp"
#include "../../lib/heap-traversing/bss-searcher.hpp"
#include "../../lib/heap-traversing/heap-traverser.hpp"
#include "../../lib/filtering/heap-filter.hpp"
#include <iostream>

int main(int argc, char** argv){

  const auto pid = 17992;
  const auto SearchStack = true;
  const auto SearchBss = true;

  auto parser = MapParser(pid);
  auto entries = parser.ParseMap();
  const MapsEntry stack = parser.getStoredStack();
  const MapsEntry text = parser.getStoredText();
  const MapsEntry heap = parser.getStoredHeap();
  const MapsEntry bss = parser.getStoredBss();

  // TODO pass this via the args
  // Is this a helpful option? as the objects arent returned over the C++/Python
  // border so it doesnt matter if they are large
  const auto max_object_size = 2500000;

  auto stack_to_heap_pointers = std::vector<RemoteHeapPointer>();
  if (SearchStack) {
    auto stackSearch = StackSearcher(stack.start, text, pid, 10000);
    stack_to_heap_pointers =
		    stackSearch.findHeapPointers(stack.end, heap, 10000);
  }

  auto bss_to_heap_pointers = std::vector<RemoteHeapPointer>();
  if (SearchBss) {
    auto bssSearcher = BssSearcher(bss, pid, 25000);
    bss_to_heap_pointers = bssSearcher.FindHeapPointers(heap);
  }

  // Merge the pointers found in the bss and the stack
  stack_to_heap_pointers.reserve(stack_to_heap_pointers.size() +
				 bss_to_heap_pointers.size());
  stack_to_heap_pointers.insert(stack_to_heap_pointers.end(),
		  bss_to_heap_pointers.begin(),
		  bss_to_heap_pointers.end());

	std::cout << "Traversing\n";

    const auto only_flatten = [&](const RemoteHeapPointer &ptr) -> bool {
      return true;
    };

    auto heap_traverser = HeapTraverser(pid, parser.getStoredHeap(),
		    250000);

    auto traversed = heap_traverser.TraversePointers(stack_to_heap_pointers);
//	std::cout << "flattening\n";
//    auto x = HeapFilter::FlattenAndFilter(traversed, only_flatten);
//    std::cout << "X:" << x.size() << "\n";
  return 0;

  //std::cout << "base:\n" << stack_to_heap_pointers.size() << " : " <<  stack_to_heap_pointers << " END:\n";

//  return stack_to_heap_pointers;
}
