#ifndef MEM_GREP_SRC_MAIN_HPP
#define MEM_GREP_SRC_MAIN_HPP

#include "misc/map-parser.hpp"
#include "heap-traversing/bss-searcher.hpp"
#include "misc/prerun-checks.hpp"
#include "heap-traversing/stack-searcher.hpp"
#include "heap-traversing/heap-traverser.hpp"
#include "argument-parsing/argument-parsing.hpp"
#include "filtering/lambda-creator.hpp"
#include "filtering/heap-filter.hpp"


enum ANALYSE_PROGRAM_ERROR {
  FAILED_PRERUN_CHECKS
};

typedef std::variant<std::vector<RemoteHeapPointer>, ANALYSE_PROGRAM_ERROR> AnalysisResultOrErr;

//This function is essentially the real main. It it called directly when using the GUI
//It is called by the real main after doing CLI specific things such as parsing CLI arguments
//This allows for minimal CLI specific backend code
AnalysisResultOrErr AnalyseProgram(const Query& query);

#endif	// MEM_GREP_SRC_MAIN_HPP
