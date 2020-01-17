#include <catch2/catch.hpp>
#include "../src/heap-traverser.hpp"
#include "../src/bss-searcher.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("Multi-layered bss pointers") {
  const auto targetPath = "./multiLayeredBssHeapPointers";
  pid_t pid = launchProgram(targetPath);
  std::cout << "Analysing PID:" << pid << std::endl;

  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();

  struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
  struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;
  heapMetadata = parser.getStoredHeap();
  bss = parser.getStoredBss();
  const size_t bssSize = (char *)bss.end - (char *)bss.start;

  // Addresses can't be more than 2**48 on modern x86_64 CPUs.
  // Anything higher is an underflow or garbage data
  REQUIRE(bssSize < pow(2,48));

  const char *bssCopy = DeepCopy(pid, bss.start, bssSize);
  REQUIRE(bssCopy != nullptr);
  auto b = BssSearcher((char *)bss.start, (char *)bss.end, pid, 2048);
  auto heapPointers = b.findHeapPointers(heapMetadata);
  REQUIRE(heapPointers.size() == 3);
  delete[] bssCopy;
  auto traverser = HeapTraverser(pid,heapMetadata,2048);
  auto deepPointers = traverser.TraversePointers(heapPointers);
  REQUIRE(HeapTraverser::CountPointers(deepPointers) == 9);

  kill(pid, SIGKILL);
}
