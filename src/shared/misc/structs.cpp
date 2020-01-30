#include "structs.hpp"
#include <functional>

std::ostream& operator<<(std::ostream& o, const RemoteHeapPointer& p) {
  o << "actual_address : " << p.actual_address << "\npoints_to : " << p.points_to
    << "\nsize_pointed_to : " << p.size_pointed_to
    << "\nchild_pointer_count : " << p.contains_pointers_to.size()
    << "\ndescendant_pointer_count : " << p.total_sub_pointers << "\n";

  return o;
}
