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

  [[nodiscard]] inline bool contains_addr(const void* address) const {
    return address >= start && address <= end;
  }
};



struct ParsedMaps {
  MapsEntry stack;
  MapsEntry heap;
  MapsEntry bss;
  MapsEntry data;
  MapsEntry text;
  std::vector<MapsEntry> mmap_sections;
//  ALLOCATOR allocator;
  std::vector<MapsEntry> all_entries;
  std::string executable_path;
};

std::ostream &operator<<(std::ostream &o, const MapsEntry &m);
std::ostream &operator<<(std::ostream &o, const ParsedMaps &pm);
//class MapParserv2 {
//  public:
//
//};
//
//ParsedMaps MapParserv2::ParseMap(pid_t pid) {
//
//}

class MapParser {
 public:

  explicit MapParser(pid_t pid); //TODO remove use of this
  static ParsedMaps ParseMap(pid_t pid);


 private:
       [[nodiscard]] std::vector<MapsEntry> ParseMap();
       [[nodiscard]] MapsEntry getStoredStack() const { return stack_; }
       [[nodiscard]] MapsEntry getStoredHeap() const { return heap_; }
       [[nodiscard]] MapsEntry getStoredBss() const { return bss_; }
       [[nodiscard]] MapsEntry getStoredData() const { return data_; }
       [[nodiscard]] MapsEntry getStoredText() const { return text_; }
       [[nodiscard]] std::vector<MapsEntry> getStoredMmap() const { return mmap_sections_; }

  const pid_t pid_;
  const MapsEntry kEmptyMapsEntry = {nullptr, nullptr, "", "", "", "", "", 0};
  enum ALLOCATOR{
    GLIBC, //Linux default
    JEMALLOC, //FreeBSD default
    PTMALLOC,
    TCMALLOC //From Google. Used in games i.e. TF2
  };

  MapsEntry merged_heaps_ = kEmptyMapsEntry;
  MapsEntry stack_ = kEmptyMapsEntry;
  MapsEntry heap_ = kEmptyMapsEntry;
  MapsEntry bss_ = kEmptyMapsEntry;
  MapsEntry data_ = kEmptyMapsEntry;
  MapsEntry text_ = kEmptyMapsEntry;
  std::vector<MapsEntry> mmap_sections_={};
  std::vector<MapsEntry> heap_sections_ = {};
  std::string executable_path_="";

  // Rudimentary functions that check for a certain character in the MapsEntry.permissions string
  [[nodiscard]] bool IsExecutable(const MapsEntry &entry) const;
  [[nodiscard]] bool IsReadable(const MapsEntry &entry) const;
  [[nodiscard]] bool IsWriteable(const MapsEntry &entry) const;
  [[nodiscard]] bool IsPrivate(const MapsEntry &entry) const;

  // Higher level functions returning a combination of the above 4 functions and checking the MapsEntry.file_path
  [[nodiscard]] bool IsDataEntry(const MapsEntry &entry) const;
  [[nodiscard]] bool IsTextEntry(const MapsEntry &entry) const;
  [[nodiscard]] bool IsHeapEntry(const MapsEntry &entry) const;
  [[nodiscard]] bool IsStackEntry(const MapsEntry &entry) const;
  [[nodiscard]] bool IsMmapEntry(const MapsEntry &entry) const;
  [[nodiscard]] bool IsBssEntry(const MapsEntry &entry) const;

  // Obtain the path of the binary being executed from /proc/PID/exe
  std::string GetExecutablePath();

  // Allows for ParseLine to be unit tested. In normal compiles this is not defined so doesn't break
  // encapsulation If you try to access ParseLine in the source code you will get errors saying
  // attempting to access private method
#ifdef UNIT_TEST
  public:
#endif
  [[nodiscard]] MapsEntry ParseLine(const std::string &line) const;

  // Some programs have multiple heaps.
  // If they are contiguous in memory we can treat them as one giant heap
  // i.e. These heaps are all contiguous as the next heap starts when the previous one ends
  // 09afa000-0df3a000 rw-p 00000000 00:00 0                                  [heap]
  // 0df3a000-0df46000 rwxp 00000000 00:00 0                                  [heap]
  // 0df46000-13dae000 rw-p 00000000 00:00 0                                  [heap]
  // 13dae000-13dc3000 rwxp 00000000 00:00 0                                  [heap]
  // 13dc3000-13dc4000 rw-p 00000000 00:00 0                                  [heap]
  // 13dc4000-13dd7000 rwxp 00000000 00:00 0                                  [heap]
  // 13dd7000-4cb94000 rw-p 00000000 00:00 0                                  [heap]
  //This also sorts the entries by their .start values
  [[nodiscard]] bool AreContiguous(std::vector<MapsEntry>& entries) const;

  // Combine all heap entries into one.
  // In a non multi heap program it will simply return the heap unchanged
  //This method assumes the entries are sorted.
  MapsEntry MergeContiguousEntries(const std::vector<MapsEntry> &entries) const;
};
#endif
