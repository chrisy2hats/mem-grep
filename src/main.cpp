#include "misc/map-parser.hpp"
#include "heap-traversing/bss-searcher.hpp"
#include "misc/prerun-checks.hpp"
#include "heap-traversing/stack-searcher.hpp"
#include "heap-traversing/heap-traverser.hpp"
#include "argument-parsing/argument-parsing.hpp"
#include "misc/utils.hpp"

using std::cout;
using std::cerr;

int main(int argc, char **argv) {

    const struct CLIArgs userArgs = ArgumentParser::parseArguments(argc, argv);

    if (!CanRun()) {
        cerr << "Prerun checks failed. mem-grep will now exit. Look at above output for info\n";
        exit(1);
    }

    auto parser = MapParser(userArgs.pid);
    const auto entries = parser.ParseMap();

    const MapsEntry stack = parser.getStoredStack();
    const MapsEntry heapMetadata = parser.getStoredHeap();
    const MapsEntry bss = parser.getStoredBss();
    const MapsEntry data = parser.getStoredData();
    const std::vector<MapsEntry> text = parser.getStoredText();

  std::vector<RemoteHeapPointer> BssMatches;
  if (userArgs.search_bss) {
        const auto bssSearcher = BssSearcher(bss, userArgs.pid,userArgs.max_heap_obj_size);
        const auto heapPointersInBss = bssSearcher.FindHeapPointers(heapMetadata);

        cout << "Found " << heapPointersInBss.size() << "pointers to the heap from the .bss section\n";
        if (userArgs.traverse_bss_pointers) {
            auto traverser = HeapTraverser(userArgs.pid, heapMetadata, userArgs.max_heap_obj_size);
	    const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(heapPointersInBss);
	  cout << "From " << heapPointersInBss.size() << " a further "
                      << HeapTraverser::CountPointers(traversed) - heapPointersInBss.size()
                      << " heap pointers where found by traversing\n";
	    HeapTraverser::PrintHeap(traversed);
	    }
  }
    if (userArgs.search_stack) {
        const auto stackSearcher = StackSearcher(stack.start, text[0], userArgs.pid,userArgs.max_heap_obj_size);
        const auto heapPointersOnStack = stackSearcher.findHeapPointers(stack.end, heapMetadata,
                                                                        userArgs.stack_frames_to_search);
        cout << "Found " << heapPointersOnStack.size() << " pointers to the heap on the stack\n";
        if (userArgs.traverse_stack_pointers) {
	  auto traverser = HeapTraverser(userArgs.pid, heapMetadata, userArgs.max_heap_obj_size);

	  const auto deepStackPointers = traverser.TraversePointers(heapPointersOnStack);
            cout << "From " << heapPointersOnStack.size() << " a further "
                      << HeapTraverser::CountPointers(deepStackPointers) - heapPointersOnStack.size()
                      << " heap pointers where found by traversing\n";
	    HeapTraverser::PrintHeap(deepStackPointers);
        }
    }

    cout << "mem-grep finished without an error.\n";
    return 0;
}
