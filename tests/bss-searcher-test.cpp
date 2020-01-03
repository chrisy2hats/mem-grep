#include <catch2/catch.hpp>
#include "../src/bss-searcher.hpp"
#include "null-structs.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("5 pointers target program"){
    int l_pid;
    const auto targetPath = "./bssHeapPointers";
    if ((l_pid = fork()) == 0) {

        execl(targetPath, "");
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works

        sleep(1);
        std::cout << "Analysing PID:" << l_pid << std::endl;
        auto parser = MapParser(l_pid);
        auto maps = parser.ParseMap();

        struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
        struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;

        bss=parser.getStoredBss();
        heapMetadata=parser.getStoredHeap();
        REQUIRE(heapMetadata != NULL_MAPS_ENTRY);
        REQUIRE(bss != NULL_MAPS_ENTRY);
        REQUIRE(bss.start != nullptr);
        auto b = BssSearcher((char*)bss.start,(char*)bss.end,l_pid,2048);
        auto heapPointers = b.findHeapPointers(heapMetadata);
        REQUIRE(heapPointers.size()==5);

        kill(l_pid, SIGKILL);
    }
}

TEST_CASE("0 pointers target program"){
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

        struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
        struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;

        heapMetadata=parser.getStoredHeap();
        bss=parser.getStoredBss();

        REQUIRE(bss!=NULL_MAPS_ENTRY);
        REQUIRE(heapMetadata!=NULL_MAPS_ENTRY);
      auto b = BssSearcher((char*)bss.start,(char*)bss.end,pid,2048);
        auto heapPointers = b.findHeapPointers(heapMetadata);

        kill(pid, SIGKILL);
        REQUIRE(heapPointers.empty());
    }
}

