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

  // If bssSize is close to a size_t max it overflowed as a bssSize of
  // approaching 2**64 will never happen
  REQUIRE(bssSize < (UINTMAX_MAX - 128));
  const char *bssCopy = DeepCopy(pid, bss.start, bssSize);
  REQUIRE(bssCopy != nullptr);
  auto b = BssSearcher((char *)bss.start, (char *)bss.end, pid, 2048);
  auto heapPointers = b.findHeapPointers(heapMetadata);
  REQUIRE(heapPointers.size() == 3);
  delete[] bssCopy;
  auto deepPointers = HeapTraverser::TraverseHeapPointers(heapMetadata, heapPointers, pid, 2048);
  REQUIRE(HeapTraverser::CountHeapPointers(deepPointers) == 9);

  kill(pid, SIGKILL);
}
