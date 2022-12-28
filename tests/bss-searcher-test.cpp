#include <catch2/catch.hpp>
#include "../lib/heap-traversing/region-scanner.hpp"
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
  auto heapPointers = RegionScanner::FindHeapPointers(pid, parsed_maps.heap, parsed_maps.bss, 1024*1024);
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
  auto heapPointers = RegionScanner::FindHeapPointers(pid, parsed_maps.heap, parsed_maps.bss, 1024*1024);

  REQUIRE(heapPointers.empty());
  std::cout << "Killing child\n";
  kill(pid, SIGKILL);
}
