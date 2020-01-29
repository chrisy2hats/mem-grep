#ifndef MEM_GREP_MISC_UTILS_HPP_
#define MEM_GREP_MISC_UTILS_HPP_

#include <cstddef>

inline void* AddToVoid(const void* base, const size_t to_add) {
  return (char*)base + to_add;
}

inline void* AddToVoid(const void* base, const void* to_add) {
  return (char*)base + (size_t)to_add;
}

inline void* SubFromVoid(const void* base, const size_t to_sub) {
  return (char*)base - to_sub;
}

inline void* SubFromVoid(const void* base, const void* to_add) {
  return (char*)base - (size_t)to_add;
}
#endif	// MEM_GREP_MISC_UTILS_HPP_
