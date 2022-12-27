#include <catch2/catch.hpp>
#include "../lib/filtering/heap-filter.hpp"
#include "../lib/misc/map-parser.hpp"
#include "../lib/heap-traversing/bss-searcher.hpp"
#include "utils.hpp"

TEST_CASE("Exclude none"){
  auto no_exclusions_returns_all = [](pid_t pid,const std::string&) {

ParsedMaps parsed_maps = MapParser::ParseMap(pid);

    auto bss_searcher = BssSearcher(parsed_maps.bss, pid, 2048);
    auto bss_pointers = bss_searcher.FindHeapPointers(parsed_maps.heap);

    auto traverser = HeapTraverser(pid, parsed_maps.heap, 2048);
    const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(bss_pointers);

    const auto AlwaysTrue = [](const RemoteHeapPointer&) { return true; };
    auto matches = HeapFilter::FlattenAndFilter(traversed, AlwaysTrue);
    REQUIRE(matches.size() == PointerCount(traversed));
  };
  ForeachTargetProgram(no_exclusions_returns_all);
}

TEST_CASE("Has child pointers"){
  pid_t pid = LaunchProgram("./multiLayeredBssHeapPointers");

  ParsedMaps parsed_maps = MapParser::ParseMap(pid);


  auto bss_searcher = BssSearcher(parsed_maps.bss,pid,2048);
  auto bss_pointers = bss_searcher.FindHeapPointers(parsed_maps.heap);

  auto traverser = HeapTraverser(pid,parsed_maps.heap,2048);
  const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(bss_pointers);

  const auto HasChildPointers = [] (const RemoteHeapPointer& ptr){
    return ptr.total_sub_pointers!=0;
  };
  auto matches = HeapFilter::FlattenAndFilter(traversed,HasChildPointers);
  REQUIRE(matches.size() ==3);
}

TEST_CASE("Single and multi thread same result") {
  auto single_and_multi = [](const pid_t pid, const std::string&) {
    ParsedMaps parsed_maps = MapParser::ParseMap(pid);


    auto bss_searcher = BssSearcher(parsed_maps.bss, pid, 2048);
    auto bss_pointers = bss_searcher.FindHeapPointers(parsed_maps.heap);

    auto traverser = HeapTraverser(pid, parsed_maps.heap, 2048);
    const std::vector<RemoteHeapPointer> traversed = traverser.TraversePointers(bss_pointers);

    auto MatchAll = [](const RemoteHeapPointer&) -> bool { return true; };
    const auto multi_thread_match_all = HeapFilter::FlattenAndFilter(traversed, MatchAll);
    const auto single_thread_match_all =
		    HeapFilter::FlattenAndFilterSingleThreaded(traversed, MatchAll);
    REQUIRE(multi_thread_match_all.size() == single_thread_match_all.size());

    auto HasChildPointers = [](const RemoteHeapPointer& ptr) -> bool {
      return ptr.total_sub_pointers != 0;
    };
    const auto multi_thread_has_children =
		    HeapFilter::FlattenAndFilter(traversed, HasChildPointers);
    const auto single_thread_has_children =
		    HeapFilter::FlattenAndFilterSingleThreaded(traversed, HasChildPointers);
    REQUIRE(single_thread_has_children.size() == multi_thread_has_children.size());
  };
  ForeachTargetProgram(single_and_multi);
}
