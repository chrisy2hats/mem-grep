#include "map-parser.hpp"
#include "memory-searcher.hpp"
#include <cassert>

int main(){
        auto pid = 11004;
        auto stack = ParseMap(pid);

        assert(stack.file_path == "[stack]");
        std::cout << "Stack address found at:" << stack.start << std::endl;

        SearchSection(stack.start,stack.end,pid);
}
