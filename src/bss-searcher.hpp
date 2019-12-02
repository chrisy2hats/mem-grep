#ifndef MEMGREP_BSSSEARCHER
#define MEMGREP_BSSSEARCHER
#include <sys/uio.h>
#include <iostream>
#include <cstring>
#include "memory-searcher.hpp"

void findBssHeapPointers(char* block, size_t block_size, char* actualBSSAddr, void* heapStart, void*heapEnd);
#endif