#ifndef MEMGREP_BSSSEARCHER
#define MEMGREP_BSSSEARCHER
#include <sys/uio.h>
#include <iostream>
#include <cstring>
#include "memory-searcher.hpp"
#include "map-parser.hpp"
#include "deep-copy.hpp"
#include "malloc-metadata.hpp"
#include "heap-traverser.hpp"

class BssSearcher{
private:
    [[nodiscard]] bool AddrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) const;
    const char* actualBssStart_;
    const char* actualBssEnd_;
    const pid_t pid_;

public:

    //Heap start and heap end can't be constructor parameters as they may changed throughout the lifetime of the object.
    //The heap can grow and shrink at any time
    //The caller MAY have a more up to date size of the heap
    //Bss Size and location however won't change during runtime as .bss is a fixed size at compile time of the remote program
    BssSearcher(const char *actualBssStart,const char* actualBssEnd,const pid_t& pid);

    std::vector<RemoteHeapPointer> findHeapPointers(const MAPS_ENTRY& heap);
};
#endif