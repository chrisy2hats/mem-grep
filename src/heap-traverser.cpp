#include "heap-traverser.hpp"

using std::cout;
using std::endl;


std::vector<RemoteHeapPointer>
HeapTraverser::traverseHeapPointers(const struct MAPS_ENTRY &heap, std::vector<RemoteHeapPointer> basePointers,
                                    const pid_t &pid) {
    for (auto &pointer: basePointers) {
        pointer = HeapTraverser::followPointer(heap, pointer, pid);
    }

    return basePointers;
}

RemoteHeapPointer
HeapTraverser::followPointer(const struct MAPS_ENTRY &heap, struct RemoteHeapPointer &base, const pid_t &pid) {

    void *current;
    std::vector<RemoteHeapPointer> currentLevelPointers = {};
    for (size_t j = 0; j < base.sizePointedTo; j += (sizeof(void *))) {
        char *blockPointedTo = deepCopy(pid, base.pointsTo, base.sizePointedTo);
        memcpy(&current, blockPointedTo + j, sizeof(void *));

        if (addrIsOnHeap(current, heap.start, heap.end)) {
            void *actualAddr = (char *) base.pointsTo + j;
            const auto pointerLocation = (void **) (blockPointedTo + j);
            const auto addressPointedTo = (void *) *pointerLocation;
            const size_t pointedToSize = getMallocMetaData(addressPointedTo, pid);

            const struct RemoteHeapPointer p = {actualAddr, addressPointedTo, pointedToSize, {}};
            currentLevelPointers.push_back(p);
        }
        delete[] blockPointedTo;
    }
    for (auto &j : currentLevelPointers) {
        const struct RemoteHeapPointer p = followPointer(heap, j, pid);
        base.totalSubPointers += p.totalSubPointers;
        base.containsPointersTo.push_back(p);
    }

    base.totalSubPointers += currentLevelPointers.size();
    std::cout << "Returning base which has " << base.containsPointersTo.size() << " pointers\n";
    return base;
}

[[nodiscard]] bool HeapTraverser::addrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) {
    const bool IsOnHeap = addr >= heapStart && addr <= heapEnd;
    return IsOnHeap;
}

size_t HeapTraverser::heapPointersKnown(const std::vector<RemoteHeapPointer> &basePointers) {
    auto total = 0;
    for (const auto &i : basePointers) {
        total += i.totalSubPointers;
    }
    total+=basePointers.size();
    return total;
}


void HeapTraverser::printPointer(RemoteHeapPointer p, int indentLevel) {
    for (const auto &i : p.containsPointersTo) {
        for (int j = 0; j < indentLevel; j++) {
            std::cout << "\t";
        }
        std::cout << "\t" << i.pointsTo << " : " << i.actualAddress << " : " << i.sizePointedTo << ":"
                  << i.totalSubPointers << "\n";
        HeapTraverser::printPointer(i, indentLevel + 1);
    }
}

void HeapTraverser::printHeap(const std::vector<RemoteHeapPointer> &basePointers) {
    for (const auto &p: basePointers) {
        std::cout << "BASE:" << p.pointsTo << " : " << p.actualAddress << " : " << p.sizePointedTo << " : "
                  << p.totalSubPointers << "\n";
        HeapTraverser::printPointer(p, 0);
    }
}
