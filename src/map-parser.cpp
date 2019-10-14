#include "map-parser.hpp"

using std::cout;
using std::endl;
using std::string;

std::vector<struct MAPS_ENTRY> ParseMap(const uint64_t &PID) {
    const string maps_path = "/proc/" + std::to_string(PID) + "/maps";

    std::ifstream maps_file(maps_path);
    if (!maps_file) {
        std::cerr << "Failed to open file: " << maps_path << endl;
        abort();
    }
    cout << "Parsing file:" << maps_path << endl;

    std::vector<MAPS_ENTRY> entries = {};
    string line;
    while (std::getline(maps_file, line)) {
        MAPS_ENTRY mapEntry = ParseLine(line);
        entries.push_back(mapEntry);
    }
    maps_file.close();
    return entries;
}


// Parse a single line of a /dev/PID/maps.
// The lines have the following format
//    address           perms offset  dev   inode       pathname
//    00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
struct MAPS_ENTRY ParseLine(const std::string &line) {
    struct MAPS_ENTRY mapEntry = {};

    if (line.empty())
        return mapEntry;

    std::vector<size_t> spaces = {};
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == ' ') {
            spaces.push_back(i + 1);
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

    string file_path = line.substr(spaces[5]);
    file_path.erase(std::remove(file_path.begin(), file_path.end(), ' '), file_path.end());
    mapEntry.file_path = file_path;

    return mapEntry;
}