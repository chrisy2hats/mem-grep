#include <memory>
#include <future>
#include <iostream>
#include <sys/uio.h>


char *deepCopy(pid_t pid, void *start, size_t size);

std::future<void *> deepCopyAsync(pid_t pdi, void *start, size_t size);


