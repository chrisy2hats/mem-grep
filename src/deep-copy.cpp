#include "deep-copy.hpp"

char *deepCopy(const pid_t& pid, const void *start, const size_t& size) {
    char *mem_area = new char[size];

    struct iovec local[1];
    struct iovec remote[1];
    local[0].iov_base = mem_area;
    local[0].iov_len = size;
    remote[0].iov_base = const_cast<void *>(start);
    remote[0].iov_len = size;

    ssize_t nread = 0;
    nread = process_vm_readv(pid, local, 1, remote, 1, 0);
    if (nread != (ssize_t) size) {
        std::cout << "Failed to extract bss section @" << __LINE__ << "\n";
        std::cout << "Only read" << nread << " bytes\n";
        exit(1);
    }
    return mem_area;
}
