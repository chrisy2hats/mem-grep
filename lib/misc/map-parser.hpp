#ifndef MEMGREP_MAP_PARSER_HPP
#define MEMGREP_MAP_PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <unistd.h>
#include <climits>  //For PATH_MAX constant

#include "utils.hpp"

// A struct representing a single line of the /proc/PID/maps file in a struct
// There will be as many MAPS_ENTRY as lines in the maps file
// This is what the maps file looks like
// address           perms offset  dev   inode       pathname
// 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
// 00651000-00652000 r--p 00051000 08:02 173521      /usr/bin/dbus-daemon
// 00652000-00655000 rw-p 00052000 08:02 173521      /usr/bin/dbus-daemon
// 00e03000-00e24000 rw-p 00000000 00:00 0           [heap]
// 00e24000-011f7000 rw-p 00000000 00:00 0           [heap]
struct MapsEntry {
  void *start;
  void *end;
  std::string permissions;
  std::string offset;
  std::string device;
  std::string inode;
  std::string file_path;
  size_t size;

  [[nodiscard]] inline bool contains_addr(const void *address) const {
    return address >= start && address <= end;
  }
};
const MapsEntry kEmptyMapsEntry = {nullptr, nullptr, "", "", "", "", "", 0};

struct ParsedMaps {
  MapsEntry stack;
  MapsEntry heap;
  MapsEntry bss;
  MapsEntry data;
  MapsEntry text;
  std::vector<MapsEntry> mmap_sections;
  std::vector<MapsEntry> all_entries;
  std::string executable_path;
};

std::ostream &operator<<(std::ostream &o, const MapsEntry &m);
std::ostream &operator<<(std::ostream &o, const ParsedMaps &pm);

class MapParser {
  public:
  [[nodiscard]] static ParsedMaps ParseMap(pid_t pid);



  private:
  enum ALLOCATOR {
    GLIBC,     // Linux default
    JEMALLOC,  // FreeBSD default
    PTMALLOC,
    TCMALLOC  // From Google. Used in games i.e. TF2
  };

  // Rudimentary functions that check for a certain character in the MapsEntry.permissions string
  [[nodiscard]] static bool IsExecutable(const MapsEntry &entry);
  [[nodiscard]] static bool IsReadable(const MapsEntry &entry);
  [[nodiscard]] static bool IsWriteable(const MapsEntry &entry);
  [[nodiscard]] static bool IsPrivate(const MapsEntry &entry);

  // Higher level functions returning a combination of the above 4 functions and checking the
  // MapsEntry.file_path
  [[nodiscard]] static bool IsDataEntry(const MapsEntry &entry);
  [[nodiscard]] static bool IsTextEntry(const MapsEntry &entry, std::string_view exe_path);
  [[nodiscard]] static bool IsHeapEntry(const MapsEntry &entry);
  [[nodiscard]] static bool IsStackEntry(const MapsEntry &entry);
  [[nodiscard]] static bool IsMmapEntry(const MapsEntry &entry);
  [[nodiscard]] static bool IsBssEntry(const MapsEntry &entry, std::string_view exe_path);

  // Obtain the path of the binary being executed from /proc/PID/exe
  static std::string GetExecutablePath(const pid_t pid);

  // Allows for ParseLine to be unit tested
#ifdef UNIT_TEST
  public:
#endif
  [[nodiscard]] static MapsEntry ParseLine(const std::string &line);
};
#endif
