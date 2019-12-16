#include "map-parser.hpp"
#include "bss-searcher.hpp"
#include "deep-copy.hpp"
#include "prerun-checks.hpp"

int main(int argc, char **argv) {
    auto pid = 0;
    if (argc == 3) {
        if (strcmp("--pid", argv[1]) == 0) {
            pid = std::stoul(argv[2]);
            std::cout << "PID parsed as:" << pid << std::endl;
        } else {
            std::cout << "ERR: 1st arg isn't --pid it is" << argv[1] << std::endl;
            exit(1);
        }
    } else {
        std::cout << "PID not provided.\n";
        exit(1);
    }
    if (!PreRunCheck()){
        std::cout << "Prerun checks failed. mem-grep will now exit. Look at above output for info\n";
        exit(1);
    }

    const auto entries = ParseMap(pid);

    MAPS_ENTRY stack = {};
    MAPS_ENTRY heapMetadata = {};
    MAPS_ENTRY bss = {};
    MAPS_ENTRY data = {};
    for (const auto& entry : entries) {
        if (entry.file_path == "[stack]") {
            stack = entry;
        }
        if (entry.file_path == "[heap]") {
            heapMetadata = entry;
        }
        if (entry.file_path == ".bss") {
            bss = entry;
        }
        if (entry.file_path == ".data") {
            data = entry;
        }
    }

    const auto bssSize = (char *) bss.end - (char *) bss.start;

    const char *bssCopy = deepCopy(pid, bss.start, bssSize);
    auto bssAnalysis = BssSearcher(bssCopy, bssSize, (char *) bss.start,pid);
    const std::vector<RemoteHeapPointer> heapPointers = bssAnalysis.findHeapPointers(heapMetadata);
    const std::vector<RemoteHeapPointer> deepLayerPointers = bssAnalysis.traverseHeapPointers(heapMetadata, heapPointers);
    delete[] bssCopy;

    for (auto i : deepLayerPointers) {
        std::cout << i.pointsTo << " : " << i.actualAddress << " : " << i.sizePointedTo << "\n";
    }

    std::cout << "mem-grep finished without an error.\n";
    return 0;
}
