#include <catch2/catch.hpp>
#include "../src/shared/heap-traversing/heap-traverser.hpp"
#include "../src/shared/heap-traversing/bss-searcher.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("Multi-layered bss pointers") {
  const auto targetPath = "./multiLayeredBssHeapPointers";
  pid_t pid = LaunchProgram(targetPath);
  std::cout << "Analysing PID:" << pid << std::endl;

  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();

  struct MapsEntry heapMetadata = NULL_MAPS_ENTRY;
  struct MapsEntry bss = NULL_MAPS_ENTRY;
  heapMetadata = parser.getStoredHeap();
  bss = parser.getStoredBss();
  const size_t bssSize = (char *)bss.end - (char *)bss.start;

  // Addresses can't be more than 2**48 on modern x86_64 CPUs.
  // Anything higher is an underflow or garbage data
  REQUIRE(bssSize < pow(2,48));

  const char *bssCopy = RemoteMemory::Copy(pid, bss.start, bssSize);
  REQUIRE(bssCopy != nullptr);
  auto b = BssSearcher(bss, pid, 2048);
  auto heapPointers = b.FindHeapPointers(heapMetadata);
  REQUIRE(heapPointers.size() == 3);
  delete[] bssCopy;
  auto traverser = HeapTraverser(pid,heapMetadata,2048);
  auto deepPointers = traverser.TraversePointers(heapPointers);
  REQUIRE(PointerCount(deepPointers) == 9);

  kill(pid, SIGKILL);
}
