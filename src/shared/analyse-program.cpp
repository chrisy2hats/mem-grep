#include "analyse-program.hpp"

using std::cout;
using std::cerr;

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
  const MapsEntry text = parser.getStoredText();

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
		    StackSearcher(stack.start, text, query.pid, query.max_heap_obj_size);
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
  AllMatches.insert(AllMatches.cend(), StackMatches.cbegin(), StackMatches.cend());
  assert(AllMatches.size() == BssMatches.size() + StackMatches.size());
  cout << "After filtering " << AllMatches.size() << " matches where found\n";

  // If all matches point to the same place
  bool same_address = true;
  void* first_points_to = nullptr;
  for (const auto& i : AllMatches) {
    if (first_points_to == nullptr) {
      first_points_to = i.points_to;
    }
    if (i.points_to != first_points_to) {
      same_address = false;
      break;
    }
  }
  if (same_address) {
    cout << "All matches point to the same location\n";
  }

  if (!AllMatches.empty() && same_address) {
    size_t expected_write = 0;
    for (const auto& i : query.subsitutions) {
      expected_write += std::visit(ValidTypesVisitor{}, i.from);
    }
    size_t bytes_written = RemoteMemory::Substitute(query.pid, AllMatches[0], query.subsitutions);
    if (expected_write != bytes_written) {
      cerr << "Write mismatch. Expected " << expected_write << " but only wrote " << bytes_written
	   << "\n";
    } else {
      cout << "Substitutions written out successfully to ptr at " << AllMatches[0].actual_address
	   << ":" << AllMatches[0].points_to << "\n";
    }
  } else {
    return AMBIGUOUS_RESULT;
  }
  return AllMatches;
}
