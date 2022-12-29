#ifndef MEM_GREP_LIB_DATASTRUCTURES_BOOL_VEC_HPP_
#define MEM_GREP_LIB_DATASTRUCTURES_BOOL_VEC_HPP_

#include <array>
#include <cstdint>
#include <cstddef>
#include <memory>

// Simple container that is like a std::vector<bool> except it is thread safe
// with no syncronisation where a std::vector<bool> may grow when one thread push_backs
class BoolVec {
  public:
  explicit BoolVec(const size_t max_index);
  void set_bit(size_t index);
  [[nodiscard]]  bool is_set(size_t index) const;

  BoolVec operator=(const BoolVec&) = delete;
  BoolVec(const BoolVec&) = delete;
  BoolVec(BoolVec&) = delete;

  private:
  std::unique_ptr<uint8_t[]> storage_;
  
  // TODO make endian aware
  const std::array<uint8_t,8> BIT_VALUES = {1,2,4,8,16,32,64,128};
};

#endif // MEM_GREP_LIB_DATASTRUCTURES_BOOL_VEC_HPP_
