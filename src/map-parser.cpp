#include "map-parser.hpp"

using std::cout;
using std::endl;
using std::string;

MapParser::MapParser(const pid_t pid) :
        pid_(pid) {
    executable_path_ = GetExecutablePath();
}

std::ostream& operator << (std::ostream &o, const MAPS_ENTRY& m){
  o << "start : " << m.start
  << "\nend : " << m.end
  << "\npermissions : " << m.permissions
  << "\noffset : " << m.offset
  << "\ndevice : " << m.device
  << "\ninode : " << m.inode
  << "\nfile_path : " << m.file_path << "\n";
  return o;
}

std::string MapParser::GetExecutablePath() {
  const string exe_containing_file = "/proc/" + std::to_string(pid_) + "/exe";
  std::cout << "Obtaining executable location from:" << exe_containing_file << "\n";

  char buff[PATH_MAX];
  ssize_t len = ::readlink(exe_containing_file.c_str(), buff, sizeof(buff) - 1);
  if (len != -1) {
    buff[len] = '\0';
    const auto exe = std::string(buff);
    assert(!exe.empty());
    assert(exe.at(0) == '/');
    return exe;
  }
  std::cerr << "Failed to obtain executable location for PID:" << pid_ << std::endl;
  std::cerr << "Error string:" << strerror(errno) << std::endl;
  exit(1);
}

[[nodiscard]] std::vector<struct MAPS_ENTRY> MapParser::ParseMap() {

  const string maps_path = "/proc/" + std::to_string(pid_) + "/maps";

  std::ifstream maps_file(maps_path);
  if (!maps_file) {
    std::cerr << "Failed to open file: " << maps_path << endl;
    abort();
  }
  cout << "Parsing file:" << maps_path << endl;

  std::vector<MAPS_ENTRY> entries = {};
  string line;
  while (std::getline(maps_file, line)) {
    MAPS_ENTRY entry = ParseLine(line);
    if (IsBssEntry(entry)) bss_=entry;
    if (IsHeapEntry(entry))heap_=entry;
    if (IsStackEntry(entry))stack_=entry;
    if (IsDataEntry(entry))data_=entry;
    if (IsTextEntry(entry))text_sections_.push_back(entry);
    if (IsMmapEntry(entry))mmap_sections_.push_back(entry);

    entries.push_back(entry);
  }
  maps_file.close();
  return entries;
}

// Parse a single line of a /dev/PID/maps.
// The lines have the following format
//    address           perms offset  dev   inode       pathname
//    00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
struct MAPS_ENTRY MapParser::ParseLine(const std::string &line) const {
//	std::cout << "line=" << line<< std::endl;
  struct MAPS_ENTRY mapEntry = {};

  if (line.empty())
    return mapEntry;

  std::vector<size_t> spaces = {};
  for (size_t i = 0; i < line.length(); i++) {
    if (line[i] == ' ') {
      spaces.emplace_back(i + 1);
    }
  }
  //A valid maps line will have atleast 5 spaces in it and contain a '-'
  if (spaces.size() < 5) {
    return mapEntry;
  }

  const auto first_dash = line.find("-");
  if (first_dash == string::npos)
    return mapEntry;

  const string start_str = line.substr(0, first_dash);
  const uint64_t start = std::stoul(start_str, 0, 16);
  memcpy(&mapEntry.start, &start, sizeof(void *)); //UGLY

  const string end_str = line.substr(first_dash + 1, spaces[0]);
  const uint64_t end = std::stoul(end_str, 0, 16);
  memcpy(&mapEntry.end, &end, sizeof(void *)); //UGLY

  mapEntry.permissions = line.substr(spaces[0], spaces[1] - spaces[0] - 1);
  mapEntry.offset = line.substr(spaces[1], spaces[2] - spaces[1] - 1);
  mapEntry.device = line.substr(spaces[2], spaces[3] - spaces[2] - 1);
  mapEntry.inode = line.substr(spaces[3], spaces[4] - spaces[3] - 1);

  string file_path = line.substr(spaces[4]);
  file_path.erase(std::remove(file_path.begin(), file_path.end(), ' '), file_path.end());
  mapEntry.file_path = file_path;

  return mapEntry;
}


bool MapParser::IsExecutable(const MAPS_ENTRY& entry)const {
  return entry.permissions.find('x')!=std::string::npos;
}

bool MapParser::IsReadable(const MAPS_ENTRY& entry)const {
    return entry.permissions.find('r')!=std::string::npos;

}
bool MapParser::IsWriteable(const MAPS_ENTRY& entry)const {
    return entry.permissions.find('w')!=std::string::npos;

}
bool MapParser::IsPrivate(const MAPS_ENTRY& entry) const{
    return entry.permissions.find('p')!=std::string::npos;

}

//Text entries MUST be executable,readable and have the file_path of the binary on the filesystem
//For instance this is a valid .text entry:
//55d3cab00000-55d3cabac000 r-xp 00000000 fd:01 3670594                    /usr/bin/kate
//But this isn't (notice not executable)
//55d3cadab000-55d3cadb3000 r--p 000ab000 fd:01 3670594                    /usr/bin/kate
bool MapParser::IsTextEntry(const MAPS_ENTRY &entry) const {
    return IsReadable(entry) && IsExecutable(entry) && entry.file_path == executable_path_;
}

bool MapParser::IsHeapEntry(const MAPS_ENTRY &entry) const {
    return IsReadable(entry) && IsWriteable(entry) && (!IsExecutable(entry)) && entry.file_path == "[heap]";
}

bool MapParser::IsStackEntry(const MAPS_ENTRY &entry) const {
    return IsReadable(entry) && IsWriteable(entry) && !IsExecutable(entry) && entry.file_path == "[stack]";
}

//Mmaped regions don't have a file path associated
bool MapParser::IsMmapEntry(const MAPS_ENTRY &entry) const {
    return entry.file_path.empty();
}

bool MapParser::IsBssEntry(const MAPS_ENTRY &entry) const {
    return IsReadable(entry) && IsWriteable(entry) && !IsExecutable(entry) && entry.file_path == executable_path_;
}

bool MapParser::IsDataEntry(const MAPS_ENTRY &entry) const {
    //TODO need to work out the characteristics of .data sections
    return false;
}


