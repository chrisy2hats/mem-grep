#include "memory-searcher.hpp"

using std::cout;
using std::endl;

void SearchSection(const void *const start, const void *const end, pid_t pid) {
        cout << "Searching from" << start << " to: " << end << endl;
        cout << std::hex << start << " : " << std::hex << end << endl;

        struct iovec local[1];
        struct iovec remote[1];

        size_t mem_size = (size_t) end - (size_t) start;

        char *mem_area = new char[mem_size];
        cout << "Allocating " << mem_size << " for the stack" << endl;
        ssize_t nread = 0;

        local[0].iov_base = mem_area;
        local[0].iov_len = mem_size;
        remote[0].iov_base = const_cast<void *>(start);
        remote[0].iov_len = mem_size;

        const uint32_t to_find = 213213;
        nread = process_vm_readv(pid, local, 2, remote, 1, 0);
        if (nread != (ssize_t) mem_size) {
                std::cout << "errno:" << errno << std::endl;
                exit(1);
        } else {
                size_t offset = 0;
                std::cout << "Loopin" << std::endl;
                for (size_t i = 0; i < mem_size; i += (sizeof(uint32_t))) {
                        auto current = (uint32_t *) mem_area + i;
                        if (*current == to_find) {
                                cout << "FOUND! at" << std::hex << current << endl;
                                cout << "Offset from start of block=" << i << endl;
                                offset = i;
                                break;
                        }
                }
                uint32_t replacement = 888;
                local[0].iov_base = &replacement;
                local[0].iov_len = sizeof(uint32_t);
                remote[0].iov_base = (uint32_t *) start + offset;
                remote[0].iov_len = sizeof(uint32_t);

                std::cout << "Trying to write " << remote[0].iov_len << " bytes to address: "
                          << local[0].iov_base << std::endl;
                ssize_t nwrite = process_vm_writev(pid, local, 2, remote, 1, 0);
                if (nwrite != (ssize_t) local[0].iov_len) {
                        std::cout << "writev failed: " << errno << std::endl;
                        switch (errno) {
                                case EFAULT:
                                        cout << "EFAULT";
                                        break;
                                case EPERM:
                                        std::cout << "EPRERM";
                                        break;
                        }
                } else {
                        std::cout << "nwrite worked!" << std::endl;
                }
        }
        delete[] mem_area;
}
