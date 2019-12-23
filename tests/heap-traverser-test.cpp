#include <catch2/catch.hpp>
#include "../src/heap-traverser.hpp"
#include "../src/bss-searcher.hpp"
#include "null-structs.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("Multi-layered bss pointers") {
    int pid;
    if ((pid = fork()) == 0) {
        const auto targetPath = "./multiLayeredBssHeapPointers";
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
        const size_t bssSize = (char *) bss.end - (char *) bss.start;

        //If bssSize is close to a size_t max it overflowed as a bssSize of approaching 2**64 will never happen
        REQUIRE(bssSize < (UINTMAX_MAX-128));
        const char *bssCopy = deepCopy(pid, bss.start, bssSize);
        REQUIRE(bssCopy != nullptr);
        auto b = BssSearcher(bssCopy, bssSize, (char *) bss.start, pid);
        auto heapPointers = b.findHeapPointers(heapMetadata);
        REQUIRE(heapPointers.size() == 3);
        delete[] bssCopy;
        auto deepPointers = HeapTraverser::traverseHeapPointers(heapMetadata, heapPointers, pid);
        REQUIRE(HeapTraverser::heapPointersKnown(deepPointers)==9);

        kill(pid, SIGKILL);
    }

}

