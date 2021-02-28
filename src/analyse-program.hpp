#ifndef MEM_GREP_SRC_ANALYSE_PROGRAM_HPP_
#define MEM_GREP_SRC_ANALYSE_PROGRAM_HPP_

#include "../lib/misc/map-parser.hpp"
#include "../lib/heap-traversing/bss-searcher.hpp"
#include "../lib/misc/prerun-checks.hpp"
#include "../lib/heap-traversing/stack-searcher.hpp"
#include "../lib/heap-traversing/heap-traverser.hpp"
#include "../lib/filtering/lambda-creator.hpp"
#include "../lib/filtering/heap-filter.hpp"

constexpr auto ANALYSE_PROGRAM_ERROR_LENGTH = 1;
enum ANALYSE_PROGRAM_ERROR {
  FAILED_PRERUN_CHECKS
};

constexpr std::array<const char*,ANALYSE_PROGRAM_ERROR_LENGTH>  ANALYSE_PROGRAM_ERROR_STR = {
		"Failed to run prerun checks. Please refer to https://github.com/ChrisFoster4/mem-grep/blob/master/docs/SystemConfiguration.md"
};

using AnalysisResultOrErr = std::variant<std::vector<RemoteHeapPointer>, ANALYSE_PROGRAM_ERROR>;

//This function is essentially the real main. It it called directly when using the GUI
//It is called by the real main after doing CLI specific things such as parsing CLI arguments
//This allows for minimal CLI specific backend code
AnalysisResultOrErr AnalyseProgram(const Query& query);

#endif	// MEM_GREP_SRC_ANALYSE_PROGRAM_HPP_
