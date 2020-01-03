#include <catch2/catch.hpp>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include "../src/memory-searcher.hpp"
#include "../src/map-parser.hpp"
#include "null-structs.hpp"

TEST_CASE("Search for NULL"){
    const pid_t self = getpid();
    const auto kilobyte = 1000;
    char* memArea = new char[kilobyte];
    bzero(memArea,kilobyte);
    auto results = SearchSection(nullptr,nullptr,self,NULL);
    REQUIRE(results.empty());
    delete[] memArea;
}

TEST_CASE("Search for non existent value"){
    const pid_t self = getpid();
    const auto kilobyte = 1000;
    char* memArea = new char[kilobyte];
    bzero(memArea,kilobyte);
    uint32_t toFind = 1;
    auto results = SearchSection(memArea,memArea+kilobyte,self,toFind);
    REQUIRE(results.empty());
    delete[] memArea;
}

TEST_CASE("Search for float"){
    const pid_t self = getpid();
    const auto kilobyte = 1000;
    char* memArea = new char[kilobyte];
    bzero(memArea,kilobyte);
    float toFind = 3.14;
    memcpy(memArea+200,&toFind,sizeof(float));
    auto results = SearchSection(memArea,memArea+kilobyte,self,toFind);
    REQUIRE(results.size() == 1);
    delete[] memArea;
}


TEST_CASE("Find Value on heap") {
    int pid;
    if ((pid = fork()) == 0) {
        const auto targetPath = "./runUntilManipulatedHeap";
        execl(targetPath, "");
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works

        sleep(1);
        std::cout << "Analysing PID:" << pid << std::endl;

        auto parser = MapParser(pid);
        auto maps = parser.ParseMap();

        struct MAPS_ENTRY heap = parser.getStoredHeap();

        REQUIRE(heap.start != NULL_MAPS_ENTRY.start);
        REQUIRE(heap.end != NULL_MAPS_ENTRY.end);
        const uint32_t to_find = 127127;
        void *start = (void *) heap.start;
        void *end = (void *) heap.end;
        auto results = SearchSection(start, end, pid, to_find);

        REQUIRE(results.size() == 1);
        kill(pid, SIGKILL);
    }
}


TEST_CASE("Find Value on stack") {
    int pid;
    if ((pid = fork()) == 0) {
        const auto targetPath = "./runUntilManipulatedStack";
        execl(targetPath, "");
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works
        sleep(1);

        std::cout << "Analysing PID:" << pid << std::endl;
        auto parser = MapParser(pid);
        auto maps = parser.ParseMap();
        struct MAPS_ENTRY stack;
        for (const auto &i : maps) {
            if (i.file_path == "[stack]") {
                stack = i;
            }
        }

        REQUIRE(stack.start != NULL_MAPS_ENTRY.start);
        REQUIRE(stack.end != NULL_MAPS_ENTRY.end);
        const uint32_t to_find = 127127;
        void *start = (void *) stack.start;
        void *end = (void *) stack.end;
        auto results = SearchSection(start, end, pid, to_find);

        REQUIRE(results.size() == 1);
        kill(pid, SIGKILL);
    }
}
