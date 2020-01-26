#include "../misc/remote-memory.hpp"
#include "../misc/structs.hpp"

// Class to provide a lambda to compare a given RemoteHeapPointer vs the parameters specified in the
// query struct
// The lambda is run against all nodes in the heap in the HeapFilter class
class LambdaCreator {
  public:
  // One of several lambdas will be returned all with the same signature but handle
  // different queries optimally. If the user doesn't specific one of the fields of a query such as
  // the minimum size then checking the minimum size for every RemoteHeapPointer is wasteful and
  // inefficient as this lambda will be called thousands of times
	  static std::function<bool(const RemoteHeapPointer&)> GetFilterLambda(
			  const pid_t pid, const Query& query);
};
