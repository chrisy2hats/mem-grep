#include "../src/map-parser.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Stack map entry parsing") {
    auto line = "7ffd6ff40000-7ffd6ff61000 rw-p 00000000 00:00 0                          [stack]";
    auto lineRes = ParseLine(line);
    REQUIRE(lineRes.start == (void *) 0x7ffd6ff40000);
    REQUIRE(lineRes.end == (void *) 0x7ffd6ff61000);
    REQUIRE(lineRes.permissions == "rw-p");
    REQUIRE(lineRes.offset == "00000000");
    REQUIRE(lineRes.device == "00:00");
    REQUIRE(lineRes.inode == "0");
    REQUIRE(lineRes.file_path == "[stack]");
}

TEST_CASE("Heap map entry parsing") {
    auto line = "5567f5648000-5567f5669000 rw-p 00000000 00:00 0                          [heap]";
    auto lineRes = ParseLine(line);
    REQUIRE(lineRes.start == (void *) 0x5567f5648000);
    REQUIRE(lineRes.end == (void *) 0x5567f5669000);
    REQUIRE(lineRes.permissions == "rw-p");
    REQUIRE(lineRes.offset == "00000000");
    REQUIRE(lineRes.device == "00:00");
    REQUIRE(lineRes.inode == "0");
    REQUIRE(lineRes.file_path == "[heap]");
}


TEST_CASE(".so path line parsing"){
    auto line = "7fd7357f7000-7fd7357f8000 rw-p 00032000 fd:01 3675544                    /usr/lib/firefox/libmozavutil.so";
    auto lineRes = ParseLine(line);
    REQUIRE(lineRes.start == (void *)0x7fd7357f7000);
    REQUIRE(lineRes.end == (void *)0x7fd7357f8000);
    REQUIRE(lineRes.permissions == "rw-p");
    REQUIRE(lineRes.offset == "00032000");
    REQUIRE(lineRes.device == "fd:01");
    REQUIRE(lineRes.inode == "3675544");
    REQUIRE(lineRes.file_path == "/usr/lib/firefox/libmozavutil.so");
}

TEST_CASE("No file path line parsing") {
    auto line = "5567f5648000-5567f5669000 rw-p 00000000 00:00 0                    ";
    auto lineRes = ParseLine(line);
    REQUIRE(lineRes.start == (void *) 0x5567f5648000);
    REQUIRE(lineRes.end == (void *) 0x5567f5669000);
    REQUIRE(lineRes.permissions == "rw-p");
    REQUIRE(lineRes.offset == "00000000");
    REQUIRE(lineRes.device == "00:00");
    REQUIRE(lineRes.inode == "0");
    REQUIRE(lineRes.file_path == "");
}

TEST_CASE("Empty line parsing") {
    auto line = "";
    auto lineRes = ParseLine(line);
    REQUIRE(lineRes.start == (void *) nullptr);
    REQUIRE(lineRes.end == (void *) nullptr);
    REQUIRE(lineRes.permissions == "");
    REQUIRE(lineRes.offset == "");
    REQUIRE(lineRes.device == "");
    REQUIRE(lineRes.inode == "");
    REQUIRE(lineRes.file_path == "");
}

TEST_CASE("Garbage input line parsing") {
    auto line = " �=����@��d�G0�9IB����v߳q��!#։%֜.[�0�1땑I� �A? ";
    auto lineRes = ParseLine(line);
    REQUIRE(lineRes.start == (void *) nullptr);
    REQUIRE(lineRes.end == (void *) nullptr);
    REQUIRE(lineRes.permissions == "");
    REQUIRE(lineRes.offset == "");
    REQUIRE(lineRes.device == "");
    REQUIRE(lineRes.inode == "");
    REQUIRE(lineRes.file_path == "");
}
