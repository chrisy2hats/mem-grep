#include <catch2/catch.hpp>
#include <cstring>
#include <unistd.h>
#include "../lib/misc/remote-memory.hpp"

const pid_t self = getpid();
const auto kilobyte = 1000;

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
  delete[] mem_area;
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
  delete[] mem_area;
  REQUIRE(matches);
}


TEST_CASE("Search for string"){
  std::string foobar = "foo bar";
  std::string barfoo = "bar foo";

  const std::vector<ValidTypes> must_contain = {foobar,foobar};
  char *mem_area = new char[kilobyte];
  bzero(mem_area,kilobyte);

  RemoteHeapPointer ptr = {
		  &mem_area,
		  mem_area,
		  kilobyte,
		  0,
		  {}
  };
  const int string_offset = 50;

  //Ensure strings at the end of the block are found
  // +1 for null_terminator
  memcpy(&mem_area[kilobyte-(foobar.size()+1)], foobar.c_str(), foobar.size());

  memcpy(&mem_area[string_offset], foobar.c_str(), foobar.size());

  const bool matches = RemoteMemory::Contains(self,ptr,must_contain);

  const std::vector<ValidTypes> cant_contain = {barfoo};
  const bool not_matches = RemoteMemory::Contains(self,ptr,cant_contain);

  delete[] mem_area;
  REQUIRE(!not_matches);
  REQUIRE(matches);
}
