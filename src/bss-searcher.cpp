#include "bss-searcher.hpp"

using std::cout;
using std::endl;

[[nodiscard]] inline bool AddrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) {
    const bool IsOnHeap = addr >= heapStart && addr <= heapEnd;
    return IsOnHeap;
}

void findBssHeapPointers(char *block, size_t block_size, char *actualBSSAddr, void *heapStart, void *heapEnd) {
    auto matches = std::vector<SearchMatch>();
    size_t zeroCount = 0, onHeapCount = 0, offHeapCount = 0;
    for (size_t i = 0; i < block_size; i += (sizeof(void *))) {
        size_t current = 0;
        memcpy(&current, block + i, sizeof(void *));
        if (current == 0) {
            zeroCount++;
            continue;
        }

        //If our target programs source has, in global scope, "int* i = new int;"
        //pointerLocation will be &i
        //And addressPointerTo will be i (The memory address potentially on the heap)
        //Except within our deepcopy of the processes .bss section not the actual .bss section
        auto pointerLocation = (void **) (block + i);
        auto addressPointedTo = (void *) *pointerLocation;

        if (AddrIsOnHeap(addressPointedTo, heapStart, heapEnd)) {
            const void *actualAddr = actualBSSAddr + i;
            cout << "---------------------------\n";
            cout << "Global pointer to heap memory found\n";
            cout << "Pointer memory is at: " << actualAddr << "\n";
            cout << "Which points to : " << addressPointedTo << "\n";
            cout << "Pointer found at .bss offset:" << i << "\n";
            cout << "---------------------------\n";
            onHeapCount++;
        } else {
            offHeapCount++;
        };
    }

    cout << "Of " << block_size / sizeof(void *) << " locations checked\n" << zeroCount <<
              " where 0 (NULL/nullptr)\n" << onHeapCount << " locations point to the heap\n" << " and "
              << offHeapCount << " non zero values that point elsewhere\n";
}