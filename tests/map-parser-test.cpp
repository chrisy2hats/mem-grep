#include <catch2/catch.hpp>

#define UNIT_TEST
#include "../src/map-parser.hpp"
#undef UNIT_TEST
#include "null-structs.hpp"

#include <string>
#include <csignal>
#include <unistd.h>


//ParseMap isn't called on this MapParser to the PID can be anything
const MapParser mp = MapParser(0);

TEST_CASE("Stack map entry parsing") {
    auto line = "7ffd6ff40000-7ffd6ff61000 rw-p 00000000 00:00 0                          [stack]";
    auto lineRes = mp.ParseLine(line);
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
    auto lineRes = mp.ParseLine(line);
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
    auto lineRes = mp.ParseLine(line);
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
    auto lineRes = mp.ParseLine(line);
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
    auto lineRes = mp.ParseLine(line);
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
    auto lineRes = mp.ParseLine(line);
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

    const auto asmProgName = "asmTarget";
    const auto asmProgPath = "./asmTarget";
    std::vector<MAPS_ENTRY> results;
    int pid;
    if ((pid=fork())==0){
        execl(asmProgPath,NULL);
    }else {
        //Give the kernel a chance to load the process and initialise the /proc/maps file
        sleep(1);
        auto parser = MapParser(pid);
        results = parser.ParseMap();

        kill(pid, SIGKILL);
    }

    struct MAPS_ENTRY stack = NULL_MAPS_ENTRY;
    struct MAPS_ENTRY heap = NULL_MAPS_ENTRY;
    for (const MAPS_ENTRY& i : results){
        if (i.file_path=="[stack]"){
            stack=i;
        }
        if (i.file_path=="[heap]"){
            heap=i;
        }
    }

    REQUIRE(results[0].file_path.find(asmProgName) != std::string::npos);
    REQUIRE(stack.start != NULL_MAPS_ENTRY.start);

    //This asm program makes 0 heap allocations
    REQUIRE(heap.start == NULL_MAPS_ENTRY.start);
}
