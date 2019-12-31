#ifndef MEMGREP_DEEP_COPY
#define MEMGREP_DEEP_COPY
#include <iostream>
#include <sys/uio.h>
#include <cassert>
#include <cstring>

// It is on the calling function to sanitise the inputs to this function.
// If asked to copy a section of 1024GB this function will try and do this
// Error checking in this function is nigh impossible as it is used in a wide variety of contexts
// For instance if asked to copy the entire stack or heap it may be copying many MBs or even GBs
// If the function fails for any reason a nullptr will be returned.
// On success a non nullptr will be returned to the entire data section requested
// It is on the caller to run delete[] on the returned pointer when it is done with the memory

char* DeepCopy(const pid_t& pid, const void* start, const size_t& size);
#endif
