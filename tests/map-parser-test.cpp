#include "../src/map-parser.hpp"
#include <catch2/catch.hpp>

//Call parse line with various lines meant to break it
TEST_CASE("Stack map entry parsing") {
    auto stack_line = "7ffd6ff40000-7ffd6ff61000 rw-p 00000000 00:00 0                          [stack]";
    auto stack_line_result = ParseLine(stack_line);
    REQUIRE(stack_line_result.file_path == "[stack]");
    REQUIRE(stack_line_result.start == (void *) 0x7ffd6ff40000);
    REQUIRE(stack_line_result.end == (void *) 0x7ffd6ff61000);
    REQUIRE(stack_line_result.permissions == "rw-p");
}

TEST_CASE("Heap map entry parsing") {
    auto heap_line = "5567f5648000-5567f5669000 rw-p 00000000 00:00 0                          [heap]";
    auto heap_line_result = ParseLine(heap_line);
    REQUIRE(heap_line_result.file_path == "[heap]");
    REQUIRE(heap_line_result.start == (void *) 0x5567f5648000);
    REQUIRE(heap_line_result.end == (void *) 0x5567f5669000);
    REQUIRE(heap_line_result.permissions == "rw-p");
}
