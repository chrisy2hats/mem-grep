#include "visited-tracker.hpp"

VisitedTracker::VisitedTracker(MapsEntry heap) :
		heap_(std::move(heap)), visited_storage_(BoolVec(heap_.size)) {}


[[nodiscard]] inline size_t VisitedTracker::CalculateBitVecOffset(const void* address) const {
  const size_t heap_offset = (size_t)address - (size_t)heap_.start;
  const size_t byte_offset = (heap_offset / 8) * 8;
  const uint8_t bit_into_byte = (size_t)address%8;
  return byte_offset + bit_into_byte;
}

void VisitedTracker::SetAlreadyVisited(const void* address) {
  visited_storage_.set_bit(CalculateBitVecOffset(address));
}

[[nodiscard]] bool VisitedTracker::IsAlreadyVisited(const void* address) const {
  return visited_storage_.is_set(CalculateBitVecOffset(address));
}
