#ifndef DAILY_MAP_PARSER_HPP
#define DAILY_MAP_PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>

struct MAPS_ENTRY {
    void* start;
    void* end;
    std::string permissions;
    std::string unknown;
    std::string fd;
    std::string fd_val;
    std::string unknown_2;
    std::string file_path;
};

struct MAPS_ENTRY ParseMap(const uint64_t &PID);
struct MAPS_ENTRY ParseLine(const std::string &line);
#endif //DAILY_MAP_PARSER_HPP
