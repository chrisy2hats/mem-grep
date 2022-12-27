#ifndef UNIT_TESTS_NULL_STRUCTS_HPP
#define UNIT_TESTS_NULL_STRUCTS_HPP
#include "../lib/misc/map-parser.hpp"

const struct MapsEntry NULL_MAPS_ENTRY =  {nullptr,nullptr,"","","","","",0};

bool operator==(const MapsEntry& a,const MapsEntry& b);
bool operator!=(const MapsEntry& a,const MapsEntry& b);
#endif //UNIT_TESTS_NULL_STRUCTS_HPP
