#ifndef MEMGREP_BSSSEARCHER
#define MEMGREP_BSSSEARCHER
#include <sys/uio.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include "map-parser.hpp"
#include "remote-memory.hpp"
#include "malloc-metadata.hpp"
#include "utils.hpp"
#include "heap-traverser.hpp"

class BssSearcher{
private:
    [[nodiscard]] bool AddrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) const;
    const char* actualBssStart_;
    const char* actualBssEnd_;
    const pid_t pid_;
    const size_t max_heap_obj_;

public:

    //Heap start and heap end can't be constructor parameters as they may changed throughout the lifetime of the object.
    //The heap can grow and shrink at any time
    //The caller MAY have a more up to date size of the heap
    //Bss Size and location however won't change during runtime as .bss is a fixed size at compile time of the remote program
    BssSearcher(const char *actualBssStart,const char* actualBssEnd,const pid_t& pid,size_t max_heap_obj);

    [[nodiscard]] std::vector<RemoteHeapPointer> findHeapPointers(const MAPS_ENTRY& heap) const;
};
#endif