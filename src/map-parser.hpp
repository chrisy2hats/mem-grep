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
#include <climits> //For PATH_MAX constant

// Representing a single line of the /proc/PID/maps file in a struct
// There will be as many MAPS_ENTRYs as lines in the maps file
// This is what the maps file looks like
// address           perms offset  dev   inode       pathname
// 00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
// 00651000-00652000 r--p 00051000 08:02 173521      /usr/bin/dbus-daemon
// 00652000-00655000 rw-p 00052000 08:02 173521      /usr/bin/dbus-daemon
// 00e03000-00e24000 rw-p 00000000 00:00 0           [heap]
// 00e24000-011f7000 rw-p 00000000 00:00 0           [heap]

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
 public:
  explicit MapParser(pid_t pid);

  [[nodiscard]] std::vector<struct MAPS_ENTRY> ParseMap();
  [[nodiscard]] struct MAPS_ENTRY getStoredStack() const { return stack_; }

  // TODO a program can have multiple heaps if it is multi-threaded. This function needs to return a std::vector of heaps
  [[nodiscard]] struct MAPS_ENTRY getStoredHeap() const { return heap_; }
  [[nodiscard]] struct MAPS_ENTRY getStoredBss() const { return bss_; }
  [[nodiscard]] struct MAPS_ENTRY getStoredData() const { return data_; }

  [[nodiscard]] std::vector<struct MAPS_ENTRY> getStoredText() const { return text_sections_; }

  [[nodiscard]] std::vector<struct MAPS_ENTRY> getStoredMmap() const { return mmap_sections_; }

 protected:
 private:
  const pid_t pid_;

  struct MAPS_ENTRY stack_;
  struct MAPS_ENTRY heap_;
  struct MAPS_ENTRY bss_;
  struct MAPS_ENTRY data_;
  std::vector<struct MAPS_ENTRY> text_sections_;
  std::vector<struct MAPS_ENTRY> mmap_sections_;
  std::string executable_path_;

  //Rudimentary functions that simply check for a certain character in the MAPS_ENTRY.permissions string
  [[nodiscard]] bool IsExecutable(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsReadable(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsWriteable(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsPrivate(const MAPS_ENTRY &entry) const;

  //Higher level functions returning a combination of the above 4 functions and checking the MAPS_ENTRY.file_path
  [[nodiscard]] bool IsDataEntry(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsTextEntry(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsHeapEntry(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsStackEntry(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsMmapEntry(const MAPS_ENTRY &entry) const;
  [[nodiscard]] bool IsBssEntry(const MAPS_ENTRY &entry) const;

  //Obtain the path of the binary being executed from /proc/PID/exe
  std::string GetExecutablePath();

  // Allows for ParseLine to be unit tested. In normal compiles this is not defined so doesn't break encapsulation
  // If you try to access ParseLine in the source code you will get errors saying attempting to access private method
#ifdef UNIT_TEST
 public:
#endif
  [[nodiscard]] struct MAPS_ENTRY ParseLine(const std::string &line) const;
};
#endif
