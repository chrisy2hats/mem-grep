#include "map-parser.hpp"

using std::cerr;
using std::cout;
using std::string;

MapParser::MapParser(const pid_t pid) : pid_(pid) {
  executable_path_ = GetExecutablePath();
}

std::ostream &operator<<(std::ostream &o, const MapsEntry &m) {
  o << "start : " << m.start << "\nend : " << m.end << "\npermissions : " << m.permissions
    << "\noffset : " << m.offset << "\ndevice : " << m.device << "\ninode : " << m.inode
    << "\nfile_path : " << m.file_path << "\n";
  return o;
}

std::string MapParser::GetExecutablePath() {
  const string exe_containing_file = "/proc/" + std::to_string(pid_) + "/exe";
  cout << "Obtaining executable location from:" << exe_containing_file << "\n";

  char buff[PATH_MAX];
  ssize_t len = ::readlink(exe_containing_file.c_str(), buff, sizeof(buff) - 1);
  if (len != -1) {
    buff[len] = '\0';
    const auto exe = std::string(buff);
    assert(!exe.empty());
    assert(exe.at(0) == '/');
    return exe;
  }
  cerr << "Failed to obtain executable location for PID:" << pid_ << '\n';
  cerr << "Error string:" << strerror(errno) << '\n';
  exit(1);
}

[[nodiscard]] std::vector<struct MapsEntry> MapParser::ParseMap() {
  const string maps_path = "/proc/" + std::to_string(pid_) + "/maps";

  std::ifstream maps_file(maps_path);
  if (!maps_file) {
    cerr << "Failed to open file: " << maps_path << '\n';
    abort();
  }
  cout << "Parsing file:" << maps_path << '\n';

  std::vector<MapsEntry> entries = {};
  string line;
  while (std::getline(maps_file, line)) {
    MapsEntry entry = ParseLine(line);
    if (IsBssEntry(entry)) {
      bss_ = entry;
      entries.push_back(entry);
      continue;
    }
    if (IsHeapEntry(entry)) {
      heap_sections_.push_back(entry);
      entries.push_back(entry);
      continue;
    }
    if (IsStackEntry(entry)) {
      stack_ = entry;
      entries.push_back(entry);
      continue;
    }
    if (IsDataEntry(entry)) {
      data_ = entry;
      entries.push_back(entry);
      continue;
    }
    if (IsTextEntry(entry)) {
      text_ = entry;
      continue;
    }
    if (IsMmapEntry(entry)) {
      mmap_sections_.push_back(entry);
      entries.push_back(entry);
      continue;
    }
    entries.push_back(entry);
  }
  if (heap_sections_.empty()){
    cerr << "Target program appears to have no heap.\n";
  }else{
    if (AreContiguous(heap_sections_)) {
      heap_ = MergeContiguousEntries(heap_sections_);
      cout << "Multiple heaps being treated as one as they are contiguous\n";
      cout << "Merged heap: " << heap_ << "\n";
    } else {
      //TODO support non contig multi-heap programs
      cerr << "Non-contiguous multi heap programs currently not supported\n";
      exit(1);
    }
  }

  maps_file.close();
  return entries;
}

// Parse a single line of a /dev/PID/maps.
// The lines have the following format
//    address           perms offset  dev   inode       pathname
//    00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
struct MapsEntry MapParser::ParseLine(const std::string &line) const {

  if (line.empty())
    return kEmptyMapsEntry;

  std::vector<size_t> spaces = {};
  for (size_t i = 0; i < line.length(); i++) {
    if (std::isspace(line.at(i)))
      spaces.emplace_back(i + 1);
  }
  // A valid maps line will have atleast 5 spaces in it and contain a '-'
  if (spaces.size() < 5) {
    return kEmptyMapsEntry;
  }

  MapsEntry mapEntry = kEmptyMapsEntry;
  const auto first_dash = line.find('-');
  if (first_dash == string::npos)
    return mapEntry;

  const string start_str = line.substr(0, first_dash);
  const uint64_t start = std::stoul(start_str, 0, 16);
  memcpy(&mapEntry.start, &start, sizeof(void *));

  const string end_str = line.substr(first_dash + 1, spaces[0]);
  const uint64_t end = std::stoul(end_str, 0, 16);
  memcpy(&mapEntry.end, &end, sizeof(void *));
  mapEntry.size = (char *)mapEntry.end - (char *)mapEntry.start;

  mapEntry.permissions = line.substr(spaces[0], spaces[1] - spaces[0] - 1);
  mapEntry.offset = line.substr(spaces[1], spaces[2] - spaces[1] - 1);
  mapEntry.device = line.substr(spaces[2], spaces[3] - spaces[2] - 1);
  mapEntry.inode = line.substr(spaces[3], spaces[4] - spaces[3] - 1);

  string file_path = line.substr(spaces[4]);
  file_path.erase(std::remove(file_path.begin(), file_path.end(), ' '), file_path.end());
  mapEntry.file_path = file_path;

  return mapEntry;
}

// Take the .start from the first heap and the .end from the last heap
MapsEntry MapParser::MergeContiguousEntries(const std::vector<MapsEntry> &entries) const {
  if (entries.empty())
    return kEmptyMapsEntry;

  MapsEntry merged_heap = entries[0];
  merged_heap.end = entries[entries.size() - 1].end;
  merged_heap.size = reinterpret_cast<size_t>(SubFromVoid(merged_heap.end, merged_heap.start));
  return merged_heap;
}

bool MapParser::AreContiguous(std::vector<MapsEntry>& entries) const {
  //We consider one entry contiguous
  if (entries.size() <= 1)
    return true;

  const auto kCompareStart = [] (const MapsEntry& a,const MapsEntry& b) -> bool {
    return a.start < b.start;
  };
  std::sort(begin(entries),end(entries),kCompareStart);

  MapsEntry previous = entries[0];
  for (size_t i=1;i<entries.size();i++){
    if (entries[i].start != previous.end)
      return false;

    previous=entries[i];
  }
  return true;
}

bool MapParser::IsExecutable(const MapsEntry &entry) const {
  return entry.permissions.find('x') != std::string::npos;
}

bool MapParser::IsReadable(const MapsEntry &entry) const {
  return entry.permissions.find('r') != std::string::npos;
}
bool MapParser::IsWriteable(const MapsEntry &entry) const {
  return entry.permissions.find('w') != std::string::npos;
}
bool MapParser::IsPrivate(const MapsEntry &entry) const {
  return entry.permissions.find('p') != std::string::npos;
}

// Text entries MUST be executable,readable and have the file_path of the binary on the filesystem
// For instance this is a valid .text entry:
// 55d3cab00000-55d3cabac000 r-xp 00000000 fd:01 3670594                    /usr/bin/kate
// But this isn't (notice not executable)
// 55d3cadab000-55d3cadb3000 r--p 000ab000 fd:01 3670594                    /usr/bin/kate
bool MapParser::IsTextEntry(const MapsEntry &entry) const {
  return IsReadable(entry) && IsExecutable(entry) && !IsWriteable(entry) &&
	 entry.file_path == executable_path_;
}

bool MapParser::IsHeapEntry(const MapsEntry &entry) const {
  return entry.file_path == "[heap]";
}

bool MapParser::IsStackEntry(const MapsEntry &entry) const {
  return entry.file_path == "[stack]";
}

// Mmaped regions don't have a file path associated
bool MapParser::IsMmapEntry(const MapsEntry &entry) const {
  return entry.file_path.empty();
}

bool MapParser::IsBssEntry(const MapsEntry &entry) const {
  return IsReadable(entry) && IsWriteable(entry) && entry.file_path == executable_path_;
}

bool MapParser::IsDataEntry(const MapsEntry& ) const {
  // TODO need to work out the characteristics of .data sections
  return false;
}
