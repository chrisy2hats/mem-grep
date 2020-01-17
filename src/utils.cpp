#include "utils.hpp"

std::ostream& operator << (std::ostream &o, const RemoteHeapPointer& p){
  std::string has_copy_str;
  if (p.copy==nullptr){
	has_copy_str = "\nno deep copy exists";
  }else{
    has_copy_str = "\ndeep copy exists";
  }

  o << "actual_address : " << p.actual_address << "\npoints_to : " << p.points_to
    << "\nsize_pointed_to : " << p.size_pointed_to
    << "\nchild_pointer_count : " << p.contains_pointers_to.size()
   << "\ndescendant_pointer_count : " << p.total_sub_pointers << has_copy_str << "\n";

  return o;
}
