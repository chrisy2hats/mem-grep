#include "map-parser.hpp"
#include "bss-searcher.hpp"
#include "prerun-checks.hpp"
#include "memory-searcher.cpp"
#include "stack-searcher.hpp"
#include "heap-traverser.hpp"
#include "argument-parsing.hpp"

int main(int argc, char **argv) {
    const struct cliArgs userArgs = ArgumentParser::parseArguments(argc, argv);

    if (!PreRunCheck()) {
        std::cerr << "Prerun checks failed. mem-grep will now exit. Look at above output for info\n";
        exit(1);
    }

    auto parser = MapParser(userArgs.pid);
    const auto entries = parser.ParseMap();

    MAPS_ENTRY stack = parser.getStoredStack();
    MAPS_ENTRY heapMetadata = parser.getStoredHeap();
    MAPS_ENTRY bss = parser.getStoredBss();
    MAPS_ENTRY data = parser.getStoredData();
    MAPS_ENTRY text = parser.getStoredText();

    if (userArgs.SearchBss) {
        const auto bssSearcher = BssSearcher((char *) bss.start, (char *) bss.end, userArgs.pid);
        const auto heapPointersInBss = bssSearcher.findHeapPointers(heapMetadata);

        std::cout << "Found " << heapPointersInBss.size() << "pointers to the heap from the .bss section\n";
        if (userArgs.TraverseBssPointers) {
            const auto traversed = HeapTraverser::traverseHeapPointers(heapMetadata, heapPointersInBss, userArgs.pid);
            std::cout << "From " << heapPointersInBss.size() << " a further "
                      << HeapTraverser::heapPointersKnown(traversed) - heapPointersInBss.size()
                      << " heap pointers where found by traversing\n";
            HeapTraverser::printHeap(traversed);
        }
    }
    if (userArgs.SearchStack) {
        const auto stackSearcher = StackSearcher(stack.start, text, userArgs.pid);
        const auto heapPointersOnStack = stackSearcher.findHeapPointers(stack.end, heapMetadata,
                                                                        userArgs.StackFramesToSearch);
        std::cout << "Found " << heapPointersOnStack.size() << " pointers to the heap on the stack\n";
        if (userArgs.TraverseStackPointers) {

            const auto deepStackPointers = HeapTraverser::traverseHeapPointers(heapMetadata, heapPointersOnStack,
                                                                               userArgs.pid);
            std::cout << "From " << heapPointersOnStack.size() << " a further "
                      << HeapTraverser::heapPointersKnown(deepStackPointers) - heapPointersOnStack.size()
                      << " heap pointers where found by traversing\n";
            HeapTraverser::printHeap(deepStackPointers);
        }
    }

    std::cout << "mem-grep finished without an error.\n";
    return 0;
}
