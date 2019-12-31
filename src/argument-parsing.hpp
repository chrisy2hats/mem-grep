#ifndef MEMGREP_ARGUMENT_PARSING
#define MEMGREP_ARGUMENT_PARSING
#include <iostream>
#include <string>
#include <sys/types.h>
#include <cstring>

struct cliArgs{
    bool SearchBss=false;
    bool TraverseBssPointers=false;
    bool SearchStack=false;
    bool TraverseStackPointers=false;
    pid_t pid=false;
    size_t StackFramesToSearch=0;
    size_t max_heap_obj_size=0;
};

class ArgumentParser{
private:
    pid_t static pidof(const std::string& program);
    pid_t static pid(const std::string& pidStr);

    //256MB
    const static size_t DEFAULT_MAX_HEAP_OBJ_SIZE_ = 268435456;
public:
    struct cliArgs static parseArguments(int argc,char** argv);
};

#endif