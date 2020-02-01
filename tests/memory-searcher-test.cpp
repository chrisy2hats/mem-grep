#include <catch2/catch.hpp>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include "../src/shared/misc/remote-memory.hpp"
#include "../src/shared/misc/map-parser.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

const pid_t self = getpid();
const auto kilobyte = 1000;

TEST_CASE("Search for NULL"){
    char* memArea = new char[kilobyte];
    bzero(memArea,kilobyte);
    auto results = RemoteMemory::Search(self,nullptr,kilobyte,NULL);
    REQUIRE(results.empty());
    delete[] memArea;
}

TEST_CASE("Search for non existent value") {
  char *memArea = new char[kilobyte];
  bzero(memArea, kilobyte);
  uint32_t toFind = 1;
  auto results = RemoteMemory::Search(self, memArea, kilobyte, toFind);
  REQUIRE(results.empty());
  delete[] memArea;
}

TEST_CASE("Search for float") {
  char *memArea = new char[kilobyte];
  bzero(memArea, kilobyte);
  float toFind = 3.14;
  memcpy(memArea + 200, &toFind, sizeof(float));
  auto results = RemoteMemory::Search(self,memArea, kilobyte, toFind);
  REQUIRE(results.size() == 1);
  delete[] memArea;
}

TEST_CASE("Don't match one address twice") {
  char *mem_area = new char[kilobyte];
  bzero(mem_area, kilobyte);
  RemoteHeapPointer ptr = {
		  &mem_area,
		  mem_area,
		  kilobyte,
		  0,
		  {}
  };

  // Searching for 2 100's when only one 100 is in the area shouldn't match
  const int hundred = 100;
  mem_area[20] = hundred;
  const std::vector<ValidTypes> must_contain = {hundred, hundred};
  bool matches = RemoteMemory::Contains(self, ptr, must_contain);
  REQUIRE(!matches);
}

TEST_CASE("Do match 2 different address"){
  char *mem_area = new char[kilobyte];
  bzero(mem_area, kilobyte);

  RemoteHeapPointer ptr = {
		  &mem_area,
		  mem_area,
		  kilobyte,
		  0,
		  {}
  };
  const int hundred = 100;
  const std::vector<ValidTypes> must_contain = {hundred, hundred};
  mem_area[20] = hundred;
  mem_area[24] = hundred;
  bool matches = RemoteMemory::Contains(self, ptr, must_contain);
  REQUIRE(matches);
}

TEST_CASE("Find Value on heap") {
  pid_t pid = LaunchProgram("runUntilManipulatedHeap");
  std::cout << "Analysing PID:" << pid << std::endl;

  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();

  struct MapsEntry heap = parser.getStoredHeap();

  REQUIRE(heap.start != NULL_MAPS_ENTRY.start);
  REQUIRE(heap.end != NULL_MAPS_ENTRY.end);
  const uint32_t to_find = 127127;
  const size_t size = (size_t)SubFromVoid(heap.end,heap.start);
  auto results = RemoteMemory::Search(pid,heap.start, size, to_find);

  REQUIRE(results.size() == 1);
  kill(pid, SIGKILL);
}

TEST_CASE("Find Value on stack") {
  pid_t pid = LaunchProgram("runUntilManipulatedStack");

  std::cout << "Analysing PID:" << pid << std::endl;
  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();
  struct MapsEntry stack = NULL_MAPS_ENTRY;
  stack = parser.getStoredStack();

  REQUIRE(stack.start != NULL_MAPS_ENTRY.start);
  REQUIRE(stack.end != NULL_MAPS_ENTRY.end);
  const uint32_t to_find = 127127;
  const auto size = (size_t)SubFromVoid(stack.end,stack.start);
  auto results = RemoteMemory::Search(pid,stack.start, size, to_find);

  REQUIRE(results.size() == 1);
  kill(pid, SIGKILL);
}
