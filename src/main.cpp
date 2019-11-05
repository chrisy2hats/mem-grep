#include "map-parser.hpp"
#include "memory-searcher.hpp"
#include "memory-writer.hpp"
#include <cassert>

int main(int argc,char** argv) {
	auto pid=0;
	if (argc == 3){
		if (strcmp("--pid",argv[1]) == 0){
			pid=std::stoul(argv[2]);
			std::cout << "PID parsed as:" << pid << std::endl;
		}else{
		    std::cout << "ERR: 1st arg isn't --pid it is" << argv[1] << std::endl;
			exit(1);
		}
	}else{
	    std::cout << "PID not provided.";
	    exit(1);
	}
    uint32_t to_find=127;

    auto entries = ParseMap(pid);

    MAPS_ENTRY stack = {};
    MAPS_ENTRY heap = {};
    for (const auto &entry : entries) {
//        std::cout << entry.file_path << std::endl;
        if (entry.file_path == "[stack]") {
            stack = entry;
        }
        if (entry.file_path == "[heap]") {
            heap = entry;
        }
    }

    std::cout << "SEARCHING STACK NOW" << std::endl;
    auto stack_results = SearchSection(stack.start, stack.end, pid, to_find);

    for (auto r : stack_results){
        std::cout << r.MatchAddress << std::endl;
    }

    std::cout << "SEARCHING HEAP NOW" << std::endl;
    auto heap_results = SearchSection(heap.start, heap.end, pid, to_find);
    for (auto r : heap_results) {
        std::cout << r.MatchAddress << std::endl;
        EditAddress(r.MatchAddress,to_find+100,pid);
    }
    std::cout << "EOM" << std::endl;
}
