#include <catch2/catch.hpp>
#include "../src/heap-traversing/bss-searcher.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("BSS: 5 pointers target program") {
  const auto targetPath = "./bssHeapPointers";
  int pid = LaunchProgram(targetPath);

  std::cout << "Analysing PID:" << pid << std::endl;
  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();

  struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
  struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;

  bss = parser.getStoredBss();
  heapMetadata = parser.getStoredHeap();
  REQUIRE(heapMetadata != NULL_MAPS_ENTRY);
  REQUIRE(bss != NULL_MAPS_ENTRY);
  REQUIRE(bss.start != nullptr);
  auto b = BssSearcher(bss, pid, 2048);
  auto heapPointers = b.FindHeapPointers(heapMetadata);
  REQUIRE(heapPointers.size() == 5);

  std::cout << "Killing child\n";
  kill(pid, SIGKILL);
}

TEST_CASE("BSS: 0 pointers target program") {
  const auto targetPath = "./runUntilManipulatedStack";
  int pid = LaunchProgram(targetPath);

  std::cout << "Analysing PID:" << pid << std::endl;
  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();

  struct MAPS_ENTRY heapMetadata = NULL_MAPS_ENTRY;
  struct MAPS_ENTRY bss = NULL_MAPS_ENTRY;

  heapMetadata = parser.getStoredHeap();
  bss = parser.getStoredBss();

  REQUIRE(bss != NULL_MAPS_ENTRY);
  REQUIRE(heapMetadata == NULL_MAPS_ENTRY);
  const auto b = BssSearcher(bss, pid, 2048);
  const auto heapPointers = b.FindHeapPointers(heapMetadata);

  REQUIRE(heapPointers.empty());
  std::cout << "Killing child\n";
  kill(pid, SIGKILL);
}
