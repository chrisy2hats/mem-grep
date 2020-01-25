#include "lambda-creator.hpp"
#include "../misc/remote-memory.hpp"

//   Many lambdas which handle different queries.
//   Below is a list of the different lambdas that may be returned and what scenarios they cover
//   1 Minimum and maximum size specified and all unique must_contain values
//   2 TODO Minimum and maximum size specified and must_contain with duplicates i.e. >32 && < 64 &&
//   must_contain 100,100
//   3 TODO Only size of objects specified i.e. >100 && < 2000
//   4 TODO no size specified only must_contain members
//   TODO check values of Query object to find optimal lambda

std::function<bool(const RemoteHeapPointer&)> LambdaCreator::GetFilterLambda(
		const pid_t pid, const Query& query) {
  // Scenario 1
  // Size requirements specified and unique must_contain values
  const auto size_with_must_contain = [&, query, pid](const RemoteHeapPointer& ptr) -> bool {
    const bool correct_size =
		    ptr.size_pointed_to >= query.min_size && ptr.size_pointed_to <= query.max_size;
    if (!correct_size)
      return false;

    for (const auto i : query.must_contain) {
      const bool contains = RemoteMemory::Contains(
		      pid, ptr.points_to, (char*)ptr.points_to + ptr.size_pointed_to, i);
      if (!contains)
	return false;
    }

    return true;
  };

  return size_with_must_contain;
}
