#ifndef MEMGREP_MALLOC_METADATA
#define MEMGREP_MALLOC_METADATA
#include <iostream>
#include "deep-copy.hpp"


[[nodiscard]] size_t getMallocMetaData(const void *heapAddr, const pid_t pid,const bool printWarnings=false);
#endif
