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
    const struct cliArgs userArgs = ArgumentParser::parseArguments(argc, argv);

    if (!PreRunCheck()) {
        cerr << "Prerun checks failed. mem-grep will now exit. Look at above output for info\n";
        exit(1);
    }

    auto parser = MapParser(userArgs.pid);
    const auto entries = parser.ParseMap();

    MAPS_ENTRY stack = parser.getStoredStack();
    MAPS_ENTRY heapMetadata = parser.getStoredHeap();
    MAPS_ENTRY bss = parser.getStoredBss();
    MAPS_ENTRY data = parser.getStoredData();
    std::vector<MAPS_ENTRY> text = parser.getStoredText();

    if (userArgs.SearchBss) {
        const auto bssSearcher = BssSearcher((char *) bss.start, (char *) bss.end, userArgs.pid,userArgs.max_heap_obj_size);
        const auto heapPointersInBss = bssSearcher.findHeapPointers(heapMetadata);

        cout << "Found " << heapPointersInBss.size() << "pointers to the heap from the .bss section\n";
        if (userArgs.TraverseBssPointers) {
            auto traverser = HeapTraverser(userArgs.pid, heapMetadata, userArgs.max_heap_obj_size);
	    const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(heapPointersInBss);
	  cout << "From " << heapPointersInBss.size() << " a further "
                      << HeapTraverser::CountPointers(traversed) - heapPointersInBss.size()
                      << " heap pointers where found by traversing\n";
//	    HeapTraverser::PrintHeap(traversed);

	  auto Is1008 =[](const RemoteHeapPointer p) -> bool{
	    return p.size_pointed_to==1008;
	  };

	  auto IsValidSize =[](const RemoteHeapPointer p) -> bool{
	    return p.size_pointed_to >31 && p.size_pointed_to < 8192;
	  };
//
		cout << "filtering\n";
	    for (const RemoteHeapPointer& base_pointer : traversed){
//	      auto y = flat_filter(base_pointer,IsValidSize);
//	      std::cout << "y :" << y.size() << "\n";
	    }
        }
    }
//    if (userArgs.SearchStack) {
//        const auto stackSearcher = StackSearcher(stack.start, text[0], userArgs.pid,userArgs.max_heap_obj_size);
//        const auto heapPointersOnStack = stackSearcher.findHeapPointers(stack.end, heapMetadata,
//                                                                        userArgs.StackFramesToSearch);
//        cout << "Found " << heapPointersOnStack.size() << " pointers to the heap on the stack\n";
//        if (userArgs.TraverseStackPointers) {
//	  auto traverser = HeapTraverser(userArgs.pid, heapMetadata, userArgs.max_heap_obj_size);
//
//	  const auto deepStackPointers = traverser.TraversePointers(heapPointersOnStack);
//            cout << "From " << heapPointersOnStack.size() << " a further "
//                      << HeapTraverser::CountPointers(deepStackPointers) - heapPointersOnStack.size()
//                      << " heap pointers where found by traversing\n";
//	    HeapTraverser::PrintHeap(deepStackPointers);
//        }
//    }
//
    cout << "mem-grep finished without an error.\n";
    return 0;
}
