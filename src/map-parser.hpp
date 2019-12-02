#ifndef MEMGREP_MAP_PARSER_HPP
#define MEMGREP_MAP_PARSER_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

struct MAPS_ENTRY {
    void *start;
    void *end;
    std::string permissions;
    std::string offset;
    std::string device;
    std::string inode;
    std::string file_path;
};

std::vector<struct MAPS_ENTRY> ParseMap(const uint64_t &PID);
struct MAPS_ENTRY ParseLine(const std::string &line);
#endif
