#include <catch2/catch.hpp>
#include "../lib/heap-traversing/bss-searcher.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

#include <csignal>
#include <unistd.h>

TEST_CASE("BSS: 5 pointers target program") {
  const auto targetPath = "./bssHeapPointers";
  int pid = LaunchProgram(targetPath);

  std::cout << "Analysing PID:" << pid << std::endl;

  ParsedMaps parsed_maps = MapParser::ParseMap(pid);



  REQUIRE(parsed_maps.heap != NULL_MAPS_ENTRY);
  REQUIRE(parsed_maps.bss != NULL_MAPS_ENTRY);
  REQUIRE(parsed_maps.bss.start != nullptr);
  auto b = BssSearcher(parsed_maps.bss, pid, 2048);
  auto heapPointers = b.FindHeapPointers(parsed_maps.heap);
  REQUIRE(heapPointers.size() == 5);

  std::cout << "Killing child\n";
  kill(pid, SIGKILL);
}

TEST_CASE("BSS: 0 pointers target program") {
  const auto targetPath = "./oneTwoSevenOnStack";
  int pid = LaunchProgram(targetPath);

  std::cout << "Analysing PID:" << pid << std::endl;
  ParsedMaps parsed_maps = MapParser::ParseMap(pid);

  REQUIRE(parsed_maps.bss != NULL_MAPS_ENTRY);
  REQUIRE(parsed_maps.heap != NULL_MAPS_ENTRY);
  const auto b = BssSearcher(parsed_maps.bss, pid, 2048);
  const auto heapPointers = b.FindHeapPointers(parsed_maps.heap);

  REQUIRE(heapPointers.empty());
  std::cout << "Killing child\n";
  kill(pid, SIGKILL);
}
