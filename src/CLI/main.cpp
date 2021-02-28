#include "../analyse-program.hpp"
#include "argument-parsing/argument-parsing.hpp"

using std::cout;
using std::cerr;

//CLI Main
//If run via GUI AnalyseProgram is called directly
int main(int argc, char** argv){
  const CLIArgs userArgs = ArgumentParser::parseArguments(argc, argv);
  const Query query = {
	userArgs.pid,
	userArgs.search_bss,
	userArgs.search_stack,
	userArgs.traverse_bss_pointers,
	userArgs.traverse_stack_pointers,
	userArgs.stack_frames_to_search,
	userArgs.max_heap_obj_size,

	//CLI parsing for these arguments is still TODO
	0,
	0,
	0,
	0,
	0,
	0,
	{},
	{}
  };
  const AnalysisResultOrErr analysis_result = AnalyseProgram(query);

  if (analysis_result.index() == 0) {
    cout << "mem-grep finished without an error.\n";
    return 0;
  } else {
    const auto error_number = std::get<ANALYSE_PROGRAM_ERROR>(analysis_result);
    cerr << "Analyse program returned an error: " << ANALYSE_PROGRAM_ERROR_STR[error_number] << "\n";
    return error_number;
  }
}
