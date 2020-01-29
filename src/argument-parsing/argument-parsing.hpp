#ifndef MEMGREP_ARGUMENT_PARSING
#define MEMGREP_ARGUMENT_PARSING
#include <iostream>
#include <string>
#include <sys/types.h>
#include <cstring>
#include "../misc/utils.hpp"

struct CLIArgs {
  bool search_bss = false;
  bool traverse_bss_pointers = false;
  bool search_stack = false;
  bool traverse_stack_pointers = false;
  pid_t pid = false;
  size_t stack_frames_to_search = 0;
  size_t max_heap_obj_size = 0;
};

class ArgumentParser {
  private:
  pid_t static pid(const std::string& pidStr);

  // 8KB
  const static size_t DEFAULT_MAX_HEAP_OBJ_SIZE_ = 8192;

  public:
  struct CLIArgs static parseArguments(int argc, char** argv);
};

#endif
