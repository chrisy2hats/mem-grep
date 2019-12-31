#ifndef MEMGREP_MALLOC_METADATA
#define MEMGREP_MALLOC_METADATA
#include <iostream>
#include "deep-copy.hpp"


[[nodiscard]] size_t getMallocMetaData(const void *heapAddr, pid_t pid,size_t max_size,bool printWarnings=false);
#endif
