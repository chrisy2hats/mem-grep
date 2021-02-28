#include "structs.hpp"
#include <functional>

std::ostream& operator<<(std::ostream& o, const RemoteHeapPointer& p) {
  o << "actual_address : " << p.actual_address << "\npoints_to : " << p.points_to
    << "\nsize_pointed_to : " << p.size_pointed_to
    << "\nchild_pointer_count : " << p.contains_pointers_to.size()
    << "\ndescendant_pointer_count : " << p.total_sub_pointers << "\n";

  return o;
}

std::ostream& operator << (std::ostream& o,const std::vector<RemoteHeapPointer>& base_pointers){
  for (const auto& p : base_pointers) {
    o << "BASE:" << p.points_to << " : " << p.actual_address << " : " << p.size_pointed_to
      << " : " << p.total_sub_pointers << "\n";
    return RecursePrintPointer(o,p);
  }
  return o;
}

std::ostream& RecursePrintPointer(std::ostream& o,const RemoteHeapPointer& p, int indent_level /*=0*/){
  for (const auto& i : p.contains_pointers_to) {
    for (int j = 0; j < indent_level; j++) {
      o << "\t";
    }
    o << "\t" << i.points_to << " : " << i.actual_address << " : " << i.size_pointed_to << ":"
      << i.total_sub_pointers << "\n";
    return RecursePrintPointer(o,i, indent_level + 1);
  }
  return o;
}

size_t PointerCount(const std::vector<RemoteHeapPointer>& base_pointers){
  size_t total = 0;
  for (const auto& i : base_pointers) {
    total += i.total_sub_pointers;
  }
  total += base_pointers.size();
  return total;
}

