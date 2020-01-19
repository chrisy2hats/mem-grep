#ifndef UNIT_TESTS_NULL_STRUCTS_HPP
#define UNIT_TESTS_NULL_STRUCTS_HPP
#include "../src/misc/map-parser.hpp"

const struct MAPS_ENTRY NULL_MAPS_ENTRY =  {nullptr,nullptr,"","","","","",0};

bool operator==(const MAPS_ENTRY& a,const MAPS_ENTRY& b);
bool operator!=(const MAPS_ENTRY& a,const MAPS_ENTRY& b);
#endif //UNIT_TESTS_NULL_STRUCTS_HPP
