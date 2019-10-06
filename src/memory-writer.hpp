#include <iostream>
#include <sys/uio.h>
#include <vector>


//Implementation must be with signature due to template argument
template <typename T>
void EditAddress(void *start, T newValue,pid_t pid) {
    struct iovec local[1];
    struct iovec remote[1];
    local[0].iov_base = &newValue;
    local[0].iov_len = sizeof(T);
    remote[0].iov_base = start;
    remote[0].iov_len = sizeof(T);

    std::cout << "Trying to write " << remote[0].iov_len << " bytes to address: " << local[0].iov_base << std::endl;
    ssize_t nwrite = process_vm_writev(pid, local, 1, remote, 1, 0);
    if (nwrite != (ssize_t) local[0].iov_len) {
        std::cout << "writev failed: " << errno << std::endl;
        switch (errno) {
            case EFAULT:
                std::cout << "EFAULT";
                break;
            case EPERM:
                std::cout << "EPRERM";
                break;
        }
        abort();
    } else {
        std::cout << "nwrite worked!" << std::endl;
    }
}
