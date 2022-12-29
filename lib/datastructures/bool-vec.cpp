#include "bool-vec.hpp"

BoolVec::BoolVec(const size_t size) {
  storage_ = std::make_unique<uint8_t[]>(size/8);
}

void BoolVec::set_bit(const size_t index) {
  const size_t byte_in_block = index/8;
  const uint8_t bit_in_byte = (index%8);

  storage_[byte_in_block] |= BIT_VALUES[bit_in_byte];
}


[[nodiscard]] bool BoolVec::is_set(const size_t index) const{
  const size_t byte_in_block = index/8;
  const uint8_t bit_in_byte = (index%8);

  return (storage_[byte_in_block] >> bit_in_byte) & 0b1;
}
