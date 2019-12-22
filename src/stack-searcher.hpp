#include <vector>
#include "bss-searcher.hpp"
#include "map-parser.hpp"

class StackSearcher{
private:
    const void* stackStart_;
    const void* textStart_;
    const void* textEnd_;
    const size_t textSize_;
    const pid_t pid_;
    [[nodiscard]] bool AddrIsInText(const void *addr) const;
    [[nodiscard]] bool AddrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) const;

public:
    //Stack end may change during lifetime of object so shouldn't be in constructor
    //Same for the heap start and end
    StackSearcher(const void *stackStart, const MAPS_ENTRY &text,const pid_t pid);

    //Searching 0 frames means search the entire stack
    std::vector<RemoteHeapPointer> findHeapPointers(const void*curStackEnd, const MAPS_ENTRY &heap, size_t framesToSearch=0) const;
};