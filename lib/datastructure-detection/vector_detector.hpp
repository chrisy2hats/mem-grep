// Vectors, atleast in most stdlibs include GNU C++ stdlib contain 3 member variables
// All of which at pointers to type T
// There is a pointer to the start of the data, the end of the data, and where the next push_back/emplace_back should put the object
#include "../misc/structs.hpp"
#include "../misc/utils.hpp"


class VectorDetector {
  private:

  struct RemoteVector {
    void* start_ptr;
    void* finish_ptr;
    void* end_of_storage_ptr;

    // GLIBC allocator rounds up the 24 bytes object to 32 bytes. TODO make allocator aware
    void* PADDING;

    bool operator==(const RemoteVector& b) const{
      return start_ptr == b.start_ptr && finish_ptr == b.finish_ptr && end_of_storage_ptr == b.end_of_storage_ptr;
    }
    size_t capacity(){
      return (char*) end_of_storage_ptr - (char*)start_ptr;
    }
    size_t size(){
      return (char*) finish_ptr - (char*)start_ptr;
    }
  };

//  bool IsValidVec(const RemoteVector& vec){
//    if (obj.size_pointed_to != sizeof(RemoteVector)) {
//      return false;
//    }
//    return v->start_ptr <= v->finish_ptr && v->finish_ptr <= v->end_of_storage_ptr;
//  }

//  bool ObjIsVec(const pid_t pid, const RemoteHeapPointer& obj) {
//    if (obj.size_pointed_to != sizeof(RemoteVector)) {
//      return false;
//    }
//
//    const RemoteObject remote_obj = RemoteMemory::RemoteCopy(pid, obj.actual_address, obj.size_pointed_to);
//    RemoteVector* v = (RemoteVector*) remote_obj.data;
////    RemoteVector v{};
////    memcpy((char*)&v,remote_obj.data,sizeof(RemoteVector));
//
//    return v->start_ptr <= v->finish_ptr && v->finish_ptr <= v->end_of_storage_ptr;
//  }

  public:
  std::vector<RemoteVector> FindVectors(const pid_t pid, const std::vector<RemoteHeapPointer>& base_pointers,
		  const size_t minimum_size) {
    std::vector<RemoteVector> matches;

    for (const auto& obj : base_pointers) {
      if (obj.size_pointed_to != sizeof(RemoteVector)){
	continue;
      }

      const RemoteObject remote_obj = RemoteMemory::RemoteCopy(pid, obj.actual_address, obj.size_pointed_to);
      auto* v = (RemoteVector*) remote_obj.data;
      if (v->start_ptr <= v->finish_ptr && v->finish_ptr <= v->end_of_storage_ptr){
	std::cout << "matches\n";
	matches.push_back(*v);
      }
    }
    return matches;
  }
};