#include "lambda-creator.hpp"

//   Many lambdas which handle different queries.
//   Below is a list of the different lambdas that may be returned and what scenarios they cover
//   1 Only size of objects specified i.e. >100 && < 2000
//   2 Minimum and maximum size specified and all unique must_contain values
//   3 Minimum and maximum size specified and must_contain with duplicates i.e. >32 && < 64 &&
//   must_contain 100,100

std::function<bool(const RemoteHeapPointer&)> LambdaCreator::GetFilterLambda(
		const pid_t pid, const Query& query) {
  if (query.must_contain.empty()) {
    // Scenario 1
    const auto only_check_size = [&, query](const RemoteHeapPointer& ptr) -> bool {
      return ptr.size_pointed_to >= query.min_size && ptr.size_pointed_to <= query.max_size;
    };
    return only_check_size;
  } else {
    // Scenario 2 and 3
    const auto check_size_and_contains = [&, query, pid](const RemoteHeapPointer& ptr) -> bool {
      const bool correct_size = ptr.size_pointed_to >= query.min_size &&
				ptr.size_pointed_to <= query.max_size;
      if (!correct_size)
	return false;

      const bool contains_all = RemoteMemory::Contains(pid, ptr, query.must_contain);
      return contains_all;
    };

    return check_size_and_contains;
  }
}
