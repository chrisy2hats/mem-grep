#ifndef MEM_GREP_UTILS_HPP
#define MEM_GREP_UTILS_HPP
#include <iostream>
#include <vector>

// A pointer to heap memory in the remote process
// The pointer may be from the stack, from the .bss or found from traversing the heap
// The same structure is used to represent pointers for all these scenarios
struct RemoteHeapPointer {
  // The address in the remote pointer where this pointer is stored. This is the 8 byte address NOT what it pointers to
  // So if the remote program has the source code "int* i = new int;" then this is &i
  void * actual_address =nullptr;

  // What the actual_address pointers to. So the 8 bytes inside actual_address
  // So if the remote program has the source code "int* i = new int;" then this is i
  void * points_to =nullptr;  // The heap location pointed to

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
  std::vector<RemoteHeapPointer> contains_pointers_to ={};

  // For performance reasons the section of memory from the remote process MAY be stored
  // rather than making a fresh copy every time it needs to be accessed
  // This copy is used when finding changes in heap memory since last scans
  // All code utilising this variable MUST be able to handle this variable being nullptr
  void* copy=nullptr;
};

// Rudimentary overload of << so that we can "std::cout <<" a RemoteHeapPointer
std::ostream& operator << (std::ostream &o, const RemoteHeapPointer& p);
#endif	// MEM_GREP_UTILS_HPP
