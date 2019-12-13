#ifndef MEMGREP_PRERUN_CHECK
#define MEMGREP_PRERUN_CHECK
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include <fstream>

[[nodiscard]] bool PreRunCheck(void);
#endif
