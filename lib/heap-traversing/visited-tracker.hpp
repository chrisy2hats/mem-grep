#ifndef MEM_GREP_LIB_HEAP_TRAVERSING_VISITED_TRACKER_HPP_
#define MEM_GREP_LIB_HEAP_TRAVERSING_VISITED_TRACKER_HPP_
#include "../misc/map-parser.hpp"
#include "../datastructures/bool-vec.hpp"

class VisitedTracker {
  public:
  explicit VisitedTracker(MapsEntry heap);

  void SetAlreadyVisited(const void* address);
  [[nodiscard]] bool IsAlreadyVisited(const void* address) const;

  private:
  [[nodiscard]] inline size_t CalculateBitVecOffset(const void* address) const;
  const MapsEntry heap_;
  BoolVec visited_storage_;
};

#endif	// MEM_GREP_LIB_HEAP_TRAVERSING_VISITED_TRACKER_HPP_
