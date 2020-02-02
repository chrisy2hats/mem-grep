#ifndef MEMGREP_REMOTE_MEMORY
#define MEMGREP_REMOTE_MEMORY
#include <iostream>
#include <vector>
#include <cassert>
#include <sys/uio.h>
#include <cstring>
#include "structs.hpp"
#include "utils.hpp"

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
  static char* Copy(const pid_t& pid, const void* start, const size_t& size);
  template <typename T>
  static ssize_t Write(pid_t pid, void* start, T new_value);

  static bool Contains(
  		const pid_t pid, const RemoteHeapPointer& ptr, const std::vector<ValidTypes>& contains);
  static ssize_t Substitute(
		  const pid_t pid, const RemoteHeapPointer& ptr, const Substitutions& subsitutions);

 private:
  static constexpr ssize_t kNotFoundOffset = -1;

  template <typename T>
  static ssize_t FindFirst(const char* start, const size_t size, ValidTypes to_find) ;
  static ssize_t FindFirstJumpTable(
		  const RemoteHeapPointer& ptr, const char* const ptr_copy, ValidTypes to_find);
};
#endif	// MEMGREP_REMOTE_MEMORY
