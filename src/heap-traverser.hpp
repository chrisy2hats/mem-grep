#ifndef MEMGREP_HEAP_TRAVERSER
#define MEMGREP_HEAP_TRAVERSER
#include "map-parser.hpp"
#include "deep-copy.hpp"
#include "malloc-metadata.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

// A pointer to heap memory in the remote process
struct RemoteHeapPointer {
    void *actualAddress;
    void *pointsTo;  // The heap location pointed to
    size_t sizePointedTo=0;
    size_t totalSubPointers=0;
    std::vector<RemoteHeapPointer> containsPointersTo;
};

class HeapTraverser {
private:
    RemoteHeapPointer static followPointer(const struct MAPS_ENTRY &heap, struct RemoteHeapPointer &base, const pid_t &pid);
    [[nodiscard]] bool static addrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd);
    void static printPointer(RemoteHeapPointer p, int indentLevel);
public:
    std::vector<RemoteHeapPointer>  static traverseHeapPointers(const struct MAPS_ENTRY& heap, std::vector<RemoteHeapPointer> heapPointers,const pid_t& pid);
    size_t static heapPointersKnown(const std::vector<RemoteHeapPointer>& basePointers);
    void static printHeap(const std::vector<RemoteHeapPointer>& basePointers);
};

#endif
