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
            struct RemoteHeapPointer result = {actualAddr, addressPointedTo,sizePointedTo};
            matches.push_back(result);

            onHeapCount++;
        } else {
            offHeapCount++;
        }
    }
    return matches;
}

/*  From the pointers found in the .bss read the data pointed to
 *  Attempt to findout what it points to
 *  Also search for additional pointers to follow
 *  NOTE it is assumed that stucts are packed. This means every pointer will be
 * 8 bytes aligned A process MAY use unpacked structs however this is unlikely
 * as it is worse for performance while saving a few bytes of memory
 */

std::vector<RemoteHeapPointer> BssSearcher::traverseHeapPointers(const struct MAPS_ENTRY& heap, std::vector<RemoteHeapPointer> heapPointers) {

    void *current;
    for (const RemoteHeapPointer& i : heapPointers) {
        std::vector<RemoteHeapPointer> currentLayerPointers;

        // Get a deep copy of that area pointed to
        char *blockPointedTo = deepCopy(pid_, i.pointsTo, i.sizePointedTo);

        // Traverse it for additional pointers. Push to pointers and recurse call
        for (size_t j = 0; j < i.sizePointedTo; j += (sizeof(void *))) {
            memcpy(&current, blockPointedTo + j, sizeof(void *));

            if (AddrIsOnHeap(current, heap.start, heap.end)) {
                void *actualAddr = (char *)i.pointsTo + j;
                const auto pointerLocation = (void **)(blockPointedTo + j);
                const auto addressPointedTo = (void *)*pointerLocation;
                const size_t pointedToSize = getMallocMetaData(addressPointedTo, pid_);

                const struct RemoteHeapPointer p = {actualAddr, addressPointedTo,pointedToSize};
                currentLayerPointers.push_back(p);
            }
        }
        auto lowerPointers = traverseHeapPointers(heap,currentLayerPointers);
        heapPointers.insert(heapPointers.end(), lowerPointers.begin(), lowerPointers.end());
        delete[] blockPointedTo;
    }
    return heapPointers;
}

