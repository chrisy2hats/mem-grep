// From a tree of RemoteHeapPointer objects attempt to detect linked lists
// We must handle

struct RemoteLL {
  size_t length;
  size_t node_size;
  std::vector<RemoteHeapPointer> nodes;
};



// walk tree with mt walker
class LL_Detector {
  private:
  // if we have gotten to the end of the list return
	  RemoteLL WalkLL(const RemoteHeapPointer& head, const size_t object_size,
		  const size_t minimum_length, std::vector<RemoteLL>& lls, const size_t depth = 0) {
    bool end_of_list = true;
    if (head.contains_pointers_to.empty()){
      return RemoteLL{0,0,{}};
    }
    for (const auto& sub_pointer : head.contains_pointers_to) {
      if (sub_pointer.size_pointed_to == object_size) {
	end_of_list = false;
	auto x = WalkLL(sub_pointer, object_size, minimum_length, lls, depth + 1);
	if (x.length != 0){
	  x.nodes[depth] = sub_pointer;
	}
      }
    }

    if (end_of_list && depth >= minimum_length) {
      return RemoteLL{depth,object_size, decltype(RemoteLL::nodes)(depth)};
    }
    return RemoteLL{0,0,{}};
  }
  public:
  std::vector<RemoteLL> Search(const std::vector<RemoteHeapPointer>& traversed_heap,
		  const size_t minimum_length) {
    auto lls = std::vector<RemoteLL>();
    for (const auto& top_level_pointer : traversed_heap) {
      WalkLL(top_level_pointer, top_level_pointer.size_pointed_to,  minimum_length,lls);
    }
    return lls;
  }

  private:
};
