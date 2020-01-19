#include <catch2/catch.hpp>
#include "../src/filtering/heap-filter.hpp"
#include "../src/misc/map-parser.hpp"
#include "../src/heap-traversing/bss-searcher.hpp"
#include "utils.hpp"

TEST_CASE("Exclude none"){
  pid_t pid = launchProgram("./multiLayeredBssHeapPointers");
  auto m = MapParser(pid);
  auto entries = m.ParseMap();
  auto bss_metadata = m.getStoredBss();
  auto heap_metadata = m.getStoredHeap();

  auto bss_searcher = BssSearcher((char*)bss_metadata.start,(char*)bss_metadata.end,pid,2048);
  auto bss_pointers = bss_searcher.findHeapPointers(heap_metadata);

  auto traverser = HeapTraverser(pid,heap_metadata,2048);
  const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(bss_pointers);

  const auto AlwaysTrue = [](const RemoteHeapPointer&){ return true;};
  auto matches = HeapFilter::FlattenAndFilter(traversed,AlwaysTrue);
  REQUIRE(matches.size() == HeapTraverser::CountPointers(traversed));
}

TEST_CASE("Has child pointers"){
  pid_t pid = launchProgram("./multiLayeredBssHeapPointers");
  auto m = MapParser(pid);
  auto entries = m.ParseMap();
  auto bss_metadata = m.getStoredBss();
  auto heap_metadata = m.getStoredHeap();

  auto bss_searcher = BssSearcher((char*)bss_metadata.start,(char*)bss_metadata.end,pid,2048);
  auto bss_pointers = bss_searcher.findHeapPointers(heap_metadata);

  auto traverser = HeapTraverser(pid,heap_metadata,2048);
  const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(bss_pointers);

  const auto HasChildPointers = [] (const RemoteHeapPointer& ptr){
    return ptr.total_sub_pointers!=0;
  };
  auto matches = HeapFilter::FlattenAndFilter(traversed,HasChildPointers);
  REQUIRE(matches.size() ==3);
}
