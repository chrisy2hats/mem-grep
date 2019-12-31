#include "utils.hpp"

std::ostream& operator << (std::ostream &o, const RemoteHeapPointer& p){
  std::string has_copy_str;
  if (p.copy==nullptr){
	has_copy_str = "\nno deep copy exists";
  }else{
    has_copy_str = "\ndeep copy exists";
  }

  o << "actual_address : " << p.actualAddress
   << "\npoints_to : " << p.pointsTo
   << "\nsize_pointed_to : " << p.sizePointedTo
   << "\nchild_pointer_count : " << p.containsPointersTo.size()
   << "\ndescendant_pointer_count : " << p.totalSubPointers
   << has_copy_str << "\n";

  return o;
}
