#include <catch2/catch.hpp>
#include "../lib/heap-traversing/heap-traverser.hpp"
#include "../lib/heap-traversing/bss-searcher.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("Multi-layered bss pointers") {
  const auto targetPath = "./multiLayeredBssHeapPointers";
  pid_t pid = LaunchProgram(targetPath);
  std::cout << "Analysing PID:" << pid << std::endl;
  ParsedMaps parsed_maps = MapParser::ParseMap(pid);


  auto bss = parsed_maps.bss;
  const size_t bssSize = (char *)bss.end - (char *)bss.start;

  // Addresses can't be more than 2**48 on modern x86_64 CPUs.
  // Anything higher is an underflow or garbage data
  REQUIRE(bssSize < pow(2,48));

  const char *bssCopy = RemoteMemory::Copy(pid, bss.start, bssSize);
  REQUIRE(bssCopy != nullptr);
  auto b = BssSearcher(bss, pid, 2048);
  auto heapPointers = b.FindHeapPointers(parsed_maps.heap);
  REQUIRE(heapPointers.size() == 3);
  delete[] bssCopy;
  auto traverser = HeapTraverser(pid,parsed_maps.heap,2048);
  auto deepPointers = traverser.TraversePointers(heapPointers);
  REQUIRE(PointerCount(deepPointers) == 9);

  kill(pid, SIGKILL);
}
