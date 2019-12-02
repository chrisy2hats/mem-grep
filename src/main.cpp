#include "map-parser.hpp"
//#include "memory-searcher.hpp"
#include "bss-searcher.hpp"
#include "deep-copy.hpp"

int main(int argc,char** argv) {
	auto pid=0;
	std::cout << "argc:" << argc << "\n";
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

    uint8_t to_find = 125;


    auto entries = ParseMap(pid);

    MAPS_ENTRY stack = {};
    MAPS_ENTRY heap = {};
    MAPS_ENTRY bss = {};
    for (const auto &entry : entries) {
        if (entry.file_path == "[stack]") {
            stack = entry;
        }
        if (entry.file_path == "[heap]") {
            heap = entry;
        }
        if (entry.file_path == ".bss"){
            bss=entry;
        }
    }

//    std::cout << "SEARCHING STACK NOW" << std::endl;
//    auto stack_results = SearchSection(stack.start, stack.end, pid, to_find);
//    std::cout << "Found:" << stack_results.size() << " 0s";
//    std::string x = "asda";
//    auto stack_results = SearchSection(stack.start, stack.end, pid, x);
//    char str[] = "foobar";
//    auto sstack_results = SearchSection(stack.start, stack.end, pid, str);

//    for (auto r : stack_results) {
//        std::cout << r.MatchAddress << std::endl;
//    }
//
//    std::cout << "SEARCHING HEAP NOW from:" << heap.start << " to :" << heap.end << std::endl;
//    auto heap_results = SearchSection(heap.start, heap.end, pid, to_find);
//
//    std::cout << "Found:" << heap_results.size() << " 0s";
//    for (auto r : heap_results) {
//        std::cout << r.MatchAddress << std::endl;
////        EditAddress(r.MatchAddress, to_find-50 , pid);
//    }
//    foobar(entries, pid);

    const auto bssSize = (char*)bss.end-(char*)bss.start;
    const auto pageSize=4096;
    if (bssSize == pageSize){
        std::cout << "Bss is only 1 page" << std::endl;
    }

    void* bssStart = bss.start;
    std::cout << "Extracting bss\n";
    char* bssSection = deepCopy(pid,bssStart,bssSize);
    findBssHeapPointers(bssSection, bssSize, (char *) bssStart, heap.start, heap.end);
    std::cout << "EOM" << std::endl;

}
