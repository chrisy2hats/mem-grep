#include <catch2/catch.hpp>
#include "../src/bss-searcher.hpp"
#include "null-structs.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("5 pointers target program"){
    int pid5;
    const auto targetPath = "./bssHeapPointers";
    if ((pid5 = fork()) == 0) {

        execl(targetPath, "");
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works

        sleep(1);
        std::cout << "Analysing PID:" << pid5 << std::endl;
        auto parser5 = MapParser(pid5);
        auto maps5 = parser5.ParseMap();

        struct MAPS_ENTRY heapMetadata5 = NULL_MAPS_ENTRY;
        struct MAPS_ENTRY bss5 = NULL_MAPS_ENTRY;

        bss5= parser5.getStoredBss();
        heapMetadata5= parser5.getStoredHeap();
        REQUIRE(heapMetadata5 != NULL_MAPS_ENTRY);
        REQUIRE(bss5 != NULL_MAPS_ENTRY);
        REQUIRE(bss5.start != nullptr);
        auto b5 = BssSearcher((char*)bss5.start,(char*)bss5.end,pid5,2048);
        auto heapPointers5 = b5.findHeapPointers(heapMetadata5);
        REQUIRE(heapPointers5.size()==5);

        kill(pid5, SIGKILL);
    }
}

TEST_CASE("0 pointers target program"){
    int pid0;
    if ((pid0 = fork()) == 0) {
        const auto targetPath = "./runUntilManipulatedStack";
        execl(targetPath, "");
    } else {
        //Give the kernel a chance to load the process and initialise the /proc/maps6 file
        //A way to avoid sleep but ensuring that the program has been loaded would be ideal but this works

        sleep(1);
        std::cout << "Analysing PID:" << pid0 << std::endl;
        auto parser0 = MapParser(pid0);
        auto maps0 = parser0.ParseMap();

        struct MAPS_ENTRY heapMetadata0 = NULL_MAPS_ENTRY;
        struct MAPS_ENTRY bss0 = NULL_MAPS_ENTRY;

        heapMetadata0= parser0.getStoredHeap();
        bss0= parser0.getStoredBss();

        REQUIRE(bss0!=NULL_MAPS_ENTRY);
        REQUIRE(heapMetadata0!=NULL_MAPS_ENTRY);
        const auto b0 = BssSearcher((char*)bss0.start,(char*)bss0.end, pid0,2048);
        const auto heapPointers0 = b0.findHeapPointers(heapMetadata0);

        kill(pid0, SIGKILL);
        REQUIRE(heapPointers0.empty());
    }
}

