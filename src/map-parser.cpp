#include "map-parser.hpp"

using std::cout;
using std::endl;
using std::string;

void f(){
        
}

struct MAPS_ENTRY ParseLine(const std::string &line) {
        struct MAPS_ENTRY mapEntry = {};

        const auto first_dash = line.find("-");
        const string start_str = line.substr(0, first_dash);
        const int64_t start = std::stoul(start_str, 0, 16);

        //UGLY
        memcpy(&mapEntry.start,&start,sizeof(void*));

        const auto first_space = line.find_first_of(" ");
        const string end_str = line.substr(first_dash + 1, (first_space - first_dash));
        const uint64_t end = std::stoul(end_str, 0, 16);

        //UGLY
        memcpy(&mapEntry.end,&end,sizeof(void*));

        const auto second_space = line.substr(first_space+1).find_first_of(" ");
        const auto permissions = line.substr(first_space+1,second_space);
        mapEntry.permissions = permissions;

        const auto last_space =  line.rfind(" ");
        string file_path = line.substr(last_space);
        file_path.erase(std::remove(file_path.begin(), file_path.end(), ' '), file_path.end());

        mapEntry.file_path = file_path;
        cout << "After last space = \"" << file_path  << "\""<< endl;

        //TODO other parts of line
        return mapEntry;
}

struct MAPS_ENTRY ParseMap(const uint64_t &PID) {
        string maps_path = "/proc/" + std::to_string(PID) + "/maps";

        std::ifstream in(maps_path);

        if (!in) {
                std::cerr << "Failed to open file: " << maps_path << endl;
                abort();
        }

        cout << "Parsing file:" << maps_path << endl;

        string line;
        while (std::getline(in, line)){
                auto mapEntry = ParseLine(line);
                if (mapEntry.file_path == "[stack]"){
                        cout << "Stack found. Returning now..." << endl;
                        in.close();
                        return mapEntry;
                }
        }
        in.close();
        return {};
}
