#ifndef DAILY_MEMORY_SEARCHER_HPP
#define DAILY_MEMORY_SEARCHER_HPP

#include <iostream>
#include <sys/uio.h>
#include <vector>

struct SearchMatch {
    size_t Offset; // Offset from the start of it's block of memory
    void *MatchAddress;
};

//Implementation must be with signature due to template argument
template<typename T>
std::vector<struct SearchMatch> SearchSection(void *start, void *const end, pid_t pid, T to_find) {

    size_t mem_size = (size_t) end - (size_t) start;
    char *mem_area = new char[mem_size];

    struct iovec local[1];
    struct iovec remote[1];
    local[0].iov_base = mem_area;
    local[0].iov_len = mem_size;
    remote[0].iov_base = start;
    remote[0].iov_len = mem_size;

    ssize_t nread = 0;
    nread = process_vm_readv(pid, local, 1, remote, 1, 0);
    if (nread != (ssize_t) mem_size) {
        switch (errno) {
            case EFAULT:
                std::cout << "EFAULT when reading from " << pid << std::endl;
                break;
            case EPERM:
                std::cout << "EPERM when reading from " << pid << std::endl;
                break;
        }
        exit(1);
    }

    auto results = std::vector<struct SearchMatch>();
    size_t offset = 0;
    for (size_t i = 0; i < mem_size; i += sizeof(T)) {

        T current = mem_area[i];
        if (current == to_find) {
            offset = i;
            void *absolute_addr = static_cast<char *>(start) + offset;
            SearchMatch match = {offset, absolute_addr};
            results.push_back(match);
        }
    }

    delete[] mem_area;

    return results;
}

#endif //DAILY_MEMORY_SEARCHER_HPP
