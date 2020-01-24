#include <catch2/catch.hpp>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include "../src/misc/remote-memory.hpp"
#include "../src/misc/map-parser.hpp"
#include "null-structs.hpp"
#include "utils.hpp"

TEST_CASE("Search for NULL"){
    const pid_t self = getpid();
    const auto kilobyte = 1000;
    char* memArea = new char[kilobyte];
    bzero(memArea,kilobyte);
    auto results = RemoteMemory::Search(self,nullptr,nullptr,NULL);
    REQUIRE(results.empty());
    delete[] memArea;
}

TEST_CASE("Search for non existent value") {
  const pid_t self = getpid();
  const auto kilobyte = 1000;
  char *memArea = new char[kilobyte];
  bzero(memArea, kilobyte);
  uint32_t toFind = 1;
  auto results = RemoteMemory::Search(self, memArea, memArea + kilobyte, toFind);
  REQUIRE(results.empty());
  delete[] memArea;
}

TEST_CASE("Search for float") {
  const pid_t self = getpid();
  const auto kilobyte = 1000;
  char *memArea = new char[kilobyte];
  bzero(memArea, kilobyte);
  float toFind = 3.14;
  memcpy(memArea + 200, &toFind, sizeof(float));
  auto results = RemoteMemory::Search(self,memArea, memArea + kilobyte, toFind);
  REQUIRE(results.size() == 1);
  delete[] memArea;
}

TEST_CASE("Find Value on heap") {
  const auto targetPath = "./runUntilManipulatedHeap";
  pid_t pid = LaunchProgram(targetPath);
  std::cout << "Analysing PID:" << pid << std::endl;

  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();

  struct MapsEntry heap = parser.getStoredHeap();

  REQUIRE(heap.start != NULL_MAPS_ENTRY.start);
  REQUIRE(heap.end != NULL_MAPS_ENTRY.end);
  const uint32_t to_find = 127127;
  void *start = (void *)heap.start;
  void *end = (void *)heap.end;
  auto results = RemoteMemory::Search(pid,start, end, to_find);

  REQUIRE(results.size() == 1);
  kill(pid, SIGKILL);
}

TEST_CASE("Find Value on stack") {
  const auto targetPath = "./runUntilManipulatedStack";
  pid_t pid = LaunchProgram(targetPath);

  std::cout << "Analysing PID:" << pid << std::endl;
  auto parser = MapParser(pid);
  auto maps = parser.ParseMap();
  struct MapsEntry stack = NULL_MAPS_ENTRY;
  stack = parser.getStoredStack();

  REQUIRE(stack.start != NULL_MAPS_ENTRY.start);
  REQUIRE(stack.end != NULL_MAPS_ENTRY.end);
  const uint32_t to_find = 127127;
  void *start = (void *)stack.start;
  void *end = (void *)stack.end;
  auto results = RemoteMemory::Search(pid,start, end, to_find);

  REQUIRE(results.size() == 1);
  kill(pid, SIGKILL);
}
