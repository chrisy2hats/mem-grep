#ifndef MEMGREP_REMOTE_MEMORY
#define MEMGREP_REMOTE_MEMORY
#include <iostream>
#include <vector>
#include <cassert>
#include <sys/uio.h>
#include <cstring>

struct SearchMatch {
  size_t Offset;  // Offset from the start of it's block of memory
  void* MatchAddress;
};

class RemoteMemory {
  public:
  // It is on the calling function to sanitise the inputs to Copy
  // If asked to copy a section of 1024GB this function will try and do this
  // Error checking in this function is nigh impossible as it is used in a wide
  // variety of contexts For instance if asked to copy the entire stack or heap
  // it may be copying many MBs or even GBs If the function fails for any reason
  // a nullptr will be returned. On success a non nullptr will be returned to
  // the entire data section requested It is on the caller to run delete[] on
  // the returned pointer when it is done with the memory assuming nullptr isn't
  // returned
  static char* Copy( const pid_t& pid, const void* start, const size_t& size);

  // Implementation of templated function must be with definition
  template <typename T>
  static ssize_t Write(pid_t pid, void* start, size_t, T new_value) {
    struct iovec local[1];
    struct iovec remote[1];
    local[0].iov_base = &new_value;
    local[0].iov_len = sizeof(T);
    remote[0].iov_base = start;
    remote[0].iov_len = sizeof(T);

    std::cout << "Trying to write " << remote[0].iov_len
	      << " bytes to address: " << local[0].iov_base << '\n';
    ssize_t nwrite = process_vm_writev(pid, local, 1, remote, 1, 0);
    if (nwrite != (ssize_t)local[0].iov_len) {
      std::cout << "writev failed: " << errno << "\n";
      std::cout << "Error string:" << strerror(errno) << "\n";
      abort();
    } else {
      std::cout << "nwrite successful\n";
    }
  }
  template <typename T>
  static std::vector<SearchMatch> Search(
		  pid_t pid, void* start, void* end, T to_find) {
    size_t mem_size = (size_t)end - (size_t)start;
    const char* mem_area = RemoteMemory::Copy(pid, start, mem_size);
    if (mem_area == nullptr) {
      std::cerr << "RemoteMemory::Copy returned nullptr\n";
      delete[] mem_area;
      return {};
    }

    auto results = std::vector<struct SearchMatch>();
    size_t offset = 0;
    for (size_t i = 0; i < mem_size; i += sizeof(T)) {
      T current = mem_area[i];
      memcpy(&current, mem_area + i, sizeof(T));

      if (current == to_find) {
	offset = i;
	void* absolute_address = (char*)start + offset;
	SearchMatch match = {offset, absolute_address};
	results.push_back(match);
      }
    }
    delete[] mem_area;
    return results;
  }

  template <typename T>
  static bool Contains( pid_t pid, void* start, void* end, T to_find) {
    size_t mem_size = (size_t)end - (size_t)start;
    const char* mem_area = RemoteMemory::Copy(pid, start, mem_size);
    if (mem_area == nullptr) {
      std::cerr << "RemoteMemory::Copy returned nullptr\n";
      delete[] mem_area;
      return false;
    }

    for (size_t i = 0; i < mem_size; i += sizeof(T)) {
      T current = mem_area[i];
      memcpy(&current, mem_area + i, sizeof(T));

      if (current == to_find) {
        delete[] mem_area;
        return true;
      }
    }
    delete[] mem_area;
    return false;
  }
};
#endif	// MEMGREP_REMOTE_MEMORY
