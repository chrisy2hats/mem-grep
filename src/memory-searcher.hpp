#ifndef MEMGREP_MEMORY_SEARCHER_HPP
#define MEMGREP_MEMORY_SEARCHER_HPP

#include <iostream>
#include <sys/uio.h>
#include <vector>
#include "deep-copy.hpp"

struct SearchMatch {
  size_t Offset;  // Offset from the start of it's block of memory
  void *MatchAddress;
};

// Implementation must be with signature due to template argument
template <typename T>
std::vector<struct SearchMatch> SearchSection(const void *start, const void *end, const pid_t& pid, T to_find) {
  size_t mem_size = (size_t)end - (size_t)start;
  char *mem_area = DeepCopy(pid,start,mem_size);
  if (mem_area == nullptr){
      std::cout << "DeepCopy returned nullptr\n";
      exit(0);
  }

  auto results = std::vector<struct SearchMatch>();
  size_t offset = 0;
  for (size_t i = 0; i < mem_size; i += sizeof(T)) {
    T current = mem_area[i];
    memcpy(&current,mem_area+i,sizeof(T));

    if (current == to_find) {
      offset = i;
      void *absolute_addr = (char*)start + offset;
      SearchMatch match = {offset, absolute_addr};
      results.push_back(match);
    }
  }

  delete[] mem_area;

  return results;
}

#endif  // MEMGREP_MEMORY_SEARCHER_HPP
