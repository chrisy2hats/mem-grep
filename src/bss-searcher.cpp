#include "bss-searcher.hpp"

using std::cout;
using std::endl;

BssSearcher::BssSearcher(const char *bssStart, const size_t &bssSize, const char *actualBssStart,const pid_t& pid)
        : pid_(pid)
        {
    bssSize_ = bssSize;
    bssStart_ = bssStart;
    actualBssStart_ = actualBssStart;
}

[[nodiscard]] bool BssSearcher::AddrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) const {
    const bool IsOnHeap = addr >= heapStart && addr <= heapEnd;
    return IsOnHeap;
}

std::vector<RemoteHeapPointer> BssSearcher::findHeapPointers(const MAPS_ENTRY &heap) {
    auto matches = std::vector<RemoteHeapPointer>();
    size_t zeroCount = 0, onHeapCount = 0, offHeapCount = 0;
    for (size_t i = 0; i < bssSize_; i += (sizeof(void *))) {
        size_t current = 0;
        memcpy(&current, bssStart_ + i, sizeof(void *));
        if (current == 0) {
            zeroCount++;
            continue;
        }

        // If our target programs source has, in global scope, "int* i = new int;"
        // pointerLocation will be &i
        // And addressPointerTo will be i (The memory address potentially on the heap
        // Except within our deepcopy of the processes .bss section not the
        // actual .bss section
        auto pointerLocation = (void **) (bssStart_ + i);
        auto addressPointedTo = (void *) *pointerLocation;

        if (AddrIsOnHeap(addressPointedTo, heap.start, heap.end)) {
            void *actualAddr =(void*) (actualBssStart_ + i);
            size_t sizePointedTo = getMallocMetaData(addressPointedTo,pid_);
            cout << "---------------------------\n";
            cout << "Global pointer to heap memory found\n";
            cout << "Pointer memory is at: " << actualAddr << "\n";
            cout << "Which points to : " << addressPointedTo << "\n";
            cout << "Pointer found at .bss offset:" << i << "\n";
            cout << "Which points to block of size:" << sizePointedTo << "\n";
            cout << "---------------------------\n";
            const struct RemoteHeapPointer result = {actualAddr, addressPointedTo,sizePointedTo};
            matches.push_back(result);

            onHeapCount++;
        } else {
            offHeapCount++;
        }
    }
    return matches;
}
