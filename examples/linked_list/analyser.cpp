#include "../../lib/heap-traversing/region-scanner.hpp"
#include "../../lib/heap-traversing/heap-traverser.hpp"
#include "../../lib/misc/prerun-checks.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
  if (!CanRun()) {
    exit(2);
  }

  if (argc != 2) {
    std::cout << "Please provide the target programs PID" << std::endl;
    exit(2);
  }

  const pid_t pid = stoi(std::string(argv[1]));
  std::cout << "Analysing " << pid << "\n";

  ParsedMaps parsed_maps = MapParser::ParseMap(pid);

  std::vector<RemoteHeapPointer> stack_to_heap_pointers = RegionScanner::FindHeapPointers(pid,parsed_maps.heap, parsed_maps.stack, 1024*1024);

  auto heap_traverser = HeapTraverser(pid, parsed_maps.heap, 250000);
  auto traversed = heap_traverser.TraversePointers(stack_to_heap_pointers);

  RemoteHeapPointer ll_head;
  size_t most_sub_pointers = 0;

  for (const auto& base_pointer : traversed) {
	if (base_pointer.total_sub_pointers > most_sub_pointers){
	  most_sub_pointers = base_pointer.total_sub_pointers;
	  ll_head = base_pointer;
	}
  }

  auto value_substitutions = std::vector<Substitution>{Substitution{.from = 42, .to = 100000}};
  std::cout << "Modifiy data in head of linked list malloc'd at " << ll_head.points_to << "\n";
  RemoteMemory::Substitute(pid, ll_head, value_substitutions);

  return 0;
}
