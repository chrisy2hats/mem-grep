#include <catch2/catch.hpp>
#include "../src/bss-searcher.hpp"
#include "null-structs.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("5 pointers target program"){
    int pid;
    if ((pid = fork()) == 0) {
        const auto targetPath = "./bssHeapPointers";

        execl(targetPath, NULL);
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works

        sleep(1);
        std::cout << "Analysing PID:" << pid << std::endl;
        auto maps = ParseMap(pid);

        struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
        struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;

        for (const auto &entry : maps) {
            if (entry.file_path == "[heap]") {
                heapMetadata = entry;
            }
            if (entry.file_path == ".bss") {
                bss = entry;
            }
        }
        REQUIRE(heapMetadata.start != NULL_MAPS_ENTRY.start);
        REQUIRE(heapMetadata.end != NULL_MAPS_ENTRY.end);
        REQUIRE(heapMetadata.file_path != NULL_MAPS_ENTRY.file_path);

        REQUIRE(bss.start != NULL_MAPS_ENTRY.start);
        REQUIRE(bss.end != NULL_MAPS_ENTRY.end);
        REQUIRE(bss.file_path != NULL_MAPS_ENTRY.file_path);
        auto b = BssSearcher((char*)bss.start,(char*)bss.end,pid);
        auto heapPointers = b.findHeapPointers(heapMetadata);
        REQUIRE(heapPointers.size()==5);

        kill(pid, SIGKILL);
    }
}

TEST_CASE("0 pointers target program"){
    int pid;
    if ((pid = fork()) == 0) {
        const auto targetPath = "./runUntilManipulatedStack";
        execl(targetPath, NULL);
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works

        sleep(1);
        std::cout << "Analysing PID:" << pid << std::endl;
        auto maps = ParseMap(pid);

        struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
        struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;

        for (const auto &entry : maps) {
            if (entry.file_path == "[heap]") {
                heapMetadata = entry;
            }
            if (entry.file_path == ".bss") {
                bss = entry;
            }
        }
        auto b = BssSearcher((char*)bss.start,(char*)bss.end,pid);
        auto heapPointers = b.findHeapPointers(heapMetadata);

        kill(pid, SIGKILL);
        REQUIRE(heapPointers.empty());
    }
}

