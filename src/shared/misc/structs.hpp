#ifndef MEM_GREP_UTILS_HPP
#define MEM_GREP_UTILS_HPP
#include <iostream>
#include <vector>
#include <functional>
#include <variant>

// A pointer to heap memory in the remote process
// The pointer may be from the stack, from the .bss or found from traversing the heap
// The same structure is used to represent pointers for all these scenarios
struct RemoteHeapPointer {
  // The address in the remote pointer where this pointer is stored. This is the 8 byte address NOT what it pointers to
  // So if the remote program has the source code "int* i = new int;" then this is &i
  void* actual_address = nullptr;

  // What the actual_address pointers to. So the 8 bytes inside actual_address
  // So if the remote program has the source code "int* i = new int;" then this is i
  void* points_to = nullptr;  // The heap location pointed to

  // The size of the allocation this points to on the heap
  // This SHOULD never be below 32 bytes as glibc never allocates less than 32 bytes
  // It is possible and should be expected that this variable will have garbage values in it
  // As false positive heap values will contain garbage which will be put in this variable
  //NOTE this is the size in bytes NOT in bits
  size_t size_pointed_to =0;

  // All pointers from BOTH directly scanning the block pointed to for heap address
  // AND pointers found by following those pointers in the initial block pointed to
  size_t total_sub_pointers =0;

  // This contains pointers to other parts of the heap that are stored in heap memory pointed to
  // This is different to total_sub_pointers as it does NOT include pointers found by traversing pointers in the initial block pointed to
  std::vector<RemoteHeapPointer> contains_pointers_to;
};

// Rudimentary overload of << so that we can "std::cout <<" a RemoteHeapPointer
std::ostream& operator << (std::ostream &o, const RemoteHeapPointer& p);

//All valid types that the user can ask to search for or substitute
typedef std::variant<int,float,double,size_t> ValidTypes;

//Allows for runtime equivalent of sizeof
struct ValidTypesVisitor
{
  size_t operator()(int) const {
    return sizeof(int);
  }
  size_t operator()(double) const {
    return sizeof(double);
  }
  size_t operator()(float) const {
    return sizeof(float);
  }
  size_t operator()(size_t) const {
    return sizeof(size_t);
  }
};

struct Substitution {
  const ValidTypes from;
  const ValidTypes to;
};
typedef std::vector<Substitution> Substitutions;

// The user can provide criteria to filter output by either from CLI arguments or via the GUI
// Both CLI and GUIs use this struct to represent the users query
struct Query{
  pid_t pid;
  //Pointer discovery parameters
  bool search_bss;
  bool search_stack;
  bool traverse_bss_pointers;
  bool traverse_stack_pointers;
  size_t stack_frames_to_search;
  size_t max_heap_obj_size;

  //Pointer filtering parameters
  size_t min_children;
  size_t max_children;
  size_t min_decendants;
  size_t max_decendants;
  size_t min_size;
  size_t max_size;
  std::vector<ValidTypes> must_contain;

  //Post-filtering actions
  Substitutions subsitutions;
};
#endif	// MEM_GREP_UTILS_HPP
