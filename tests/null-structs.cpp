#include "null-structs.hpp"

bool operator==(const MapsEntry& a, const MapsEntry& b)
{
  if (a.start != b.start) return false;
  if (a.end != b.end) return false;
  if (a.permissions != b.permissions) return false;
  if (a.offset != b.offset) return false;
  if (a.device != b.device) return false;
  if (a.inode != a.device) return false;
  if (a.file_path != b.file_path) return false;
  return true;
}

bool operator!=(const MapsEntry& a, const MapsEntry& b)
{
  return !(a == b);
}
