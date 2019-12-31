#ifndef MEMGREP_MALLOC_METADATA
#define MEMGREP_MALLOC_METADATA
#include <iostream>
#include "deep-copy.hpp"


[[nodiscard]] size_t GetMallocMetadata(const void *heap_address, pid_t pid, size_t max_size, bool print_warnings =false);
#endif
