#ifndef MEMGREP_DEEP_COPY
#define MEMGREP_DEEP_COPY
#include <memory>
#include <future>
#include <iostream>
#include <sys/uio.h>

char *deepCopy(const pid_t& pid, const void *start, const size_t& size);
#endif
