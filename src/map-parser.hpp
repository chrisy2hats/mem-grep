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

class MapParser {
private:
    const pid_t pid_;
    struct MAPS_ENTRY stack_;
    struct MAPS_ENTRY heap_;
    struct MAPS_ENTRY bss_;
    struct MAPS_ENTRY data_;
    struct MAPS_ENTRY text_;

#ifdef UNIT_TEST
public:
#endif
    [[nodiscard]] struct MAPS_ENTRY ParseLine(const std::string &line) const;

public:
    explicit MapParser(pid_t pid);

    [[nodiscard]] struct MAPS_ENTRY getStoredStack() const { return stack_; }
    [[nodiscard]] struct MAPS_ENTRY getStoredHeap() const { return heap_; }
    [[nodiscard]] struct MAPS_ENTRY getStoredBss() const { return bss_; }
    [[nodiscard]] struct MAPS_ENTRY getStoredData() const { return data_; }
    [[nodiscard]] struct MAPS_ENTRY getStoredText() const { return text_; }

    [[nodiscard]] std::vector<struct MAPS_ENTRY> ParseMap();
};

#endif
