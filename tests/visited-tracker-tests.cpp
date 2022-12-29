#include <catch2/catch.hpp>
#include <cstring>
#include <unistd.h>
#include "../lib/misc/remote-memory.hpp"
#include "../lib/heap-traversing/visited-tracker.hpp"

TEST_CASE("Visited Tracker") {

  const size_t START = 1000000;
  const size_t END =   1001024;
  const size_t size = END-START;

  const MapsEntry fake_heap = MapsEntry{.start=(void*)START,.end=(void*)END,.permissions="",.offset="",.device="",.inode="",.file_path="",.size=size};

  auto vt = VisitedTracker(fake_heap);

  for (size_t i=0;i<size;i++){
    void* addr = (void*) (START+i);
    REQUIRE_FALSE(vt.IsAlreadyVisited(addr));
    vt.SetAlreadyVisited(addr);
    REQUIRE(vt.IsAlreadyVisited(addr));
  }

}
