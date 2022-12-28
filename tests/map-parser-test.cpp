#include <catch2/catch.hpp>

#define UNIT_TEST
#include "../lib/misc/map-parser.hpp"
#undef UNIT_TEST
#include "null-structs.hpp"
#include "utils.hpp"

#include <string>
#include <csignal>
#include <unistd.h>



TEST_CASE("Stack map entry parsing") {
    auto line = "7ffd6ff40000-7ffd6ff61000 rw-p 00000000 00:00 0                          [stack]";
    auto lineRes = MapParser::ParseLine(line);
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
    auto lineRes = MapParser::ParseLine(line);
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
    auto lineRes = MapParser::ParseLine(line);
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
    auto lineRes = MapParser::ParseLine(line);
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
    auto lineRes = MapParser::ParseLine(line);
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
    auto lineRes = MapParser::ParseLine(line);
    REQUIRE(lineRes.start == (void *) nullptr);
    REQUIRE(lineRes.end == (void *) nullptr);
    REQUIRE(lineRes.permissions == "");
    REQUIRE(lineRes.offset == "");
    REQUIRE(lineRes.device == "");
    REQUIRE(lineRes.inode == "");
    REQUIRE(lineRes.file_path == "");
}
TEST_CASE("Small x64_64 asm program"){
    /*Example maps file for small asm program. Note no heap
    00400000-00401000 r-xp 00000000 fd:01 9452734                            /home/foobar/mem-analyse/target-programs/redzone-user/write
    7ffc74a2a000-7ffc74a4b000 rwxp 00000000 00:00 0                          [stack]
    7ffc74bf3000-7ffc74bf6000 r--p 00000000 00:00 0                          [vvar]
    7ffc74bf6000-7ffc74bf8000 r-xp 00000000 00:00 0                          [vdso]
    ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]
     */

    const auto asmProgPath = "./asmTarget";
    pid_t pid = LaunchProgram(asmProgPath);

    ParsedMaps parsed_maps = MapParser::ParseMap(pid);



    REQUIRE(parsed_maps.all_entries[0].file_path.find("asmTarget") != std::string::npos);
    //This asm program makes 0 heap allocations
    REQUIRE(parsed_maps.heap.start == NULL_MAPS_ENTRY.start);
    REQUIRE(parsed_maps.stack.start != NULL_MAPS_ENTRY.start);

    kill(pid, SIGKILL);
}

TEST_CASE("Mandatory sections for every program"){
  auto has_mandatory_sections = [&] (const pid_t pid,const std::string& exe_name)->void{
    ParsedMaps parsed_maps = MapParser::ParseMap(pid);

    REQUIRE(parsed_maps.stack!=NULL_MAPS_ENTRY);
    REQUIRE(!parsed_maps.all_entries.empty());
    if (exe_name!="asmTarget" && exe_name != "runUntilManipulatedStack"){
      REQUIRE(parsed_maps.heap!=NULL_MAPS_ENTRY);
    }
  };
  ForeachTargetProgram(has_mandatory_sections);
}
