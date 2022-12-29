#include "map-parser.hpp"

using std::cerr;
using std::cout;
using std::string;


std::ostream &operator<<(std::ostream &o, const MapsEntry &m) {
  o << "start : " << m.start << "\nend : " << m.end << "\npermissions : " << m.permissions
    << "\noffset : " << m.offset << "\ndevice : " << m.device << "\ninode : " << m.inode
    << "\nfile_path : " << m.file_path << "\n";
  return o;
}

std::ostream &operator<<(std::ostream &o, const ParsedMaps &pm) {
  o << "Stack: " << pm.stack << "\nHeap: " << pm.heap << "\nBss: " << pm.bss << "\nData: " << pm.data << "\nText: "
		  << pm.text << "\nExe path: " << pm.executable_path << "\n";
  return o;
}


std::string MapParser::GetExecutablePath(pid_t pid) {
  const string exe_containing_file = "/proc/" + std::to_string(pid) + "/exe";
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
  cerr << "Failed to obtain executable location for PID:" << pid << '\n';
  cerr << "Error string:" << strerror(errno) << '\n';
  exit(1);
}

[[nodiscard]] ParsedMaps MapParser::ParseMap(pid_t pid) {
  const string maps_path = "/proc/" + std::to_string(pid) + "/maps";

  std::ifstream maps_file(maps_path);
  if (!maps_file) {
    cerr << "Failed to open file: " << maps_path << '\n';
    abort();
  }
  cout << "Parsing file:" << maps_path << '\n';


  ParsedMaps parsed = {.stack = kEmptyMapsEntry,
		  .heap = kEmptyMapsEntry,
		  .bss = kEmptyMapsEntry,
		  .data = kEmptyMapsEntry,
		  .text = kEmptyMapsEntry,
		  .mmap_sections = std::vector<MapsEntry>(),
		  .all_entries = std::vector<MapsEntry>(),

		  .executable_path = MapParser::GetExecutablePath(pid)};

  string line;
  while (std::getline(maps_file, line)) {
    const MapsEntry entry = MapParser::ParseLine(line);
    parsed.all_entries.push_back(entry);

    if (MapParser::IsBssEntry(entry, parsed.executable_path)) {
      parsed.bss = entry;
    } else if (IsHeapEntry(entry)) {
      parsed.heap=entry;
    }
    else if (MapParser::IsStackEntry(entry)) {
      parsed.stack = entry;
    }
    else if (MapParser::IsDataEntry(entry)) {
      parsed.data = entry;
    }
    else if (MapParser::IsTextEntry(entry, parsed.executable_path)) {
      parsed.text = entry;
    }
    else if (MapParser::IsMmapEntry(entry)) {
      parsed.mmap_sections.push_back(entry);
    }
  }
  maps_file.close();
  return parsed;
}

// Parse a single line of a /dev/PID/maps.
// The lines have the following format
//    address           perms offset  dev   inode       pathname
//    00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
struct MapsEntry MapParser::ParseLine(const std::string &line){

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


bool MapParser::IsExecutable(const MapsEntry &entry) {
  return entry.permissions.find('x') != std::string::npos;
}

bool MapParser::IsReadable(const MapsEntry &entry) {
  return entry.permissions.find('r') != std::string::npos;
}
bool MapParser::IsWriteable(const MapsEntry &entry) {
  return entry.permissions.find('w') != std::string::npos;
}
bool MapParser::IsPrivate(const MapsEntry &entry) {
  return entry.permissions.find('p') != std::string::npos;
}

// Text entries MUST be executable,readable and have the file_path of the binary on the filesystem
// For instance this is a valid .text entry:
// 55d3cab00000-55d3cabac000 r-xp 00000000 fd:01 3670594                    /usr/bin/kate
// But this isn't (notice not executable)
// 55d3cadab000-55d3cadb3000 r--p 000ab000 fd:01 3670594                    /usr/bin/kate
bool MapParser::IsTextEntry(const MapsEntry &entry, std::string_view exe_path) {
  return IsReadable(entry) && IsExecutable(entry) && !IsWriteable(entry) &&
	 entry.file_path == exe_path;
}

bool MapParser::IsHeapEntry(const MapsEntry &entry) {
  return entry.file_path == "[heap]";
}

bool MapParser::IsStackEntry(const MapsEntry &entry) {
  return entry.file_path == "[stack]";
}

// Mmaped regions don't have a file path associated
bool MapParser::IsMmapEntry(const MapsEntry &entry) {
  return entry.file_path.empty();
}

bool MapParser::IsBssEntry(const MapsEntry &entry, std::string_view exe_path) {
  return IsReadable(entry) && IsWriteable(entry) && entry.file_path == exe_path;
}

bool MapParser::IsDataEntry(const MapsEntry &) {
  // TODO need to work out the characteristics of .data sections
  return false;
}
