#include "main.hpp"

using std::cout;
using std::cerr;

//CLI Main If run via GUI AnalyseProgram is called directly
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
	{},
	{}
  };
  const AnalysisResultOrErr analysis_result = AnalyseProgram(query);

  if (analysis_result.index() == 0) {
    cout << "mem-grep finished without an error.\n";
    return 0;
  } else {
    const auto error_number = std::get<ANALYSE_PROGRAM_ERROR>(analysis_result);
    cerr << "Analyse program returned an error " << error_number << "\n";
    return error_number;
  }
}

AnalysisResultOrErr AnalyseProgram(const Query& query) {

  if (!CanRun()) {
    cerr << "Prerun checks failed. mem-grep will now exit. Look at above output for info\n";
    return FAILED_PRERUN_CHECKS;
  }

  auto parser = MapParser(query.pid);
  const auto entries = parser.ParseMap();

  const MapsEntry stack = parser.getStoredStack();
  const MapsEntry heapMetadata = parser.getStoredHeap();
  const MapsEntry bss = parser.getStoredBss();
  const MapsEntry data = parser.getStoredData();
  const std::vector<MapsEntry> text = parser.getStoredText();

  const auto kFilterLambda = LambdaCreator::GetFilterLambda(query.pid, query);
  std::vector<RemoteHeapPointer> BssMatches;
  if (query.search_bss) {
    const auto bssSearcher = BssSearcher(bss, query.pid, query.max_heap_obj_size);
    const auto heapPointersInBss = bssSearcher.FindHeapPointers(heapMetadata);

    cout << "Found " << heapPointersInBss.size() << "pointers to the heap from the .bss section\n";
    if (query.traverse_bss_pointers) {
      auto traverser = HeapTraverser(query.pid, heapMetadata, query.max_heap_obj_size);
      const std::vector<RemoteHeapPointer> traversed =
		      traverser.TraversePointers(heapPointersInBss);
      cout << "From " << heapPointersInBss.size() << " a further "
	   << HeapTraverser::CountPointers(traversed) - heapPointersInBss.size()
	   << " heap pointers where found by traversing\n";
      BssMatches = HeapFilter::FlattenAndFilter(traversed, kFilterLambda);
    }
  }
  std::vector<RemoteHeapPointer> StackMatches;
  if (query.search_stack) {
    const auto stackSearcher =
		    StackSearcher(stack.start, text[0], query.pid, query.max_heap_obj_size);
    const auto heapPointersOnStack = stackSearcher.findHeapPointers(
		    stack.end, heapMetadata, query.stack_frames_to_search);
    cout << "Found " << heapPointersOnStack.size() << " pointers to the heap on the stack\n";
    if (query.traverse_stack_pointers) {
      auto traverser = HeapTraverser(query.pid, heapMetadata, query.max_heap_obj_size);

      const auto deepStackPointers = traverser.TraversePointers(heapPointersOnStack);
      cout << "From " << heapPointersOnStack.size() << " a further "
	   << HeapTraverser::CountPointers(deepStackPointers) - heapPointersOnStack.size()
	   << " heap pointers where found by traversing\n";
      StackMatches = HeapFilter::FlattenAndFilter(deepStackPointers, kFilterLambda);
    }
  }

  std::vector<RemoteHeapPointer> AllMatches = BssMatches;
  BssMatches.insert(BssMatches.cend(), StackMatches.cbegin(), StackMatches.cend());
  assert(AllMatches.size() == BssMatches.size() + StackMatches.size());

  return AllMatches;
}
