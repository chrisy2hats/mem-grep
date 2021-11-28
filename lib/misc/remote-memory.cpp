#include "remote-memory.hpp"

using std::cerr;
using std::cout;

RemoteObject RemoteMemory::RemoteCopy(const pid_t pid, const void* start, const size_t size){
  return RemoteObject(RemoteMemory::Copy(pid,start,size), size);
}

char* RemoteMemory::Copy(const pid_t pid, const void* start, const size_t size) {
  if (start == nullptr) {
    cerr << __FUNCTION__ << " requested to copy from nullptr\n";
    return nullptr;
  }

  char* mem_area = nullptr;
  try {
    mem_area = new char[size];
  } catch (std::bad_alloc& ba) {
    cerr << "WARNING: " << __FUNCTION__
	 << "was requested to copy a large memory section of size: " << size
	 << " memory could not be allocated to store this section as it is too "
	    "large\n"
	 << "Raw error message from new is:" << ba.what() << "\n";
    return mem_area;
  }
  assert(mem_area != nullptr);

  struct iovec local[1];
  struct iovec remote[1];
  local[0].iov_base = mem_area;
  local[0].iov_len = size;
  remote[0].iov_base = const_cast<void*>(start);
  remote[0].iov_len = size;

  // process_vm_readv returns the number of bytes it managed to read.
  // This may be less than the number requested.
  // On error(i.e. invalid start address) the function returns -1 bytes read
  const ssize_t nread = process_vm_readv(pid, local, 1, remote, 1, 0);
  if (nread != (ssize_t)size) {
    cerr << "Failed to copy memory from remote process at line:" << __LINE__
	 << " in file:" << __FILE__ << "\n"
	 << "Read:" << nread << " bytes when: " << size
	 << " bytes should have been read from address: " << start << "\n"
	 << "Note this is NOT due to failing to allocate memory to store "
	    "the result of the copy\n"
	 << "Error string: " << strerror(errno) << '\n';
    delete[] mem_area;
    return nullptr;
  }
  return mem_area;
}

template <typename T>
ssize_t RemoteMemory::Write(pid_t pid, void* start, T new_value) {

  size_t size = 0;
  if (typeid(T) == typeid(ValidTypes)){
    size = std::visit(ValidTypesVisitor{}, new_value);
  }else{
    size = sizeof(T);
  }
  cout << "Writting " << size << " bytes in Write!\n";

  struct iovec local[1];
  struct iovec remote[1];
  local[0].iov_base = &new_value;
  local[0].iov_len = size;
  remote[0].iov_base = start;
  remote[0].iov_len = size;

  std::cout << "Trying to write " << remote[0].iov_len
	    << " bytes to address: " << local[0].iov_base << '\n';
  ssize_t nwrite = process_vm_writev(pid, local, 1, remote, 1, 0);
  if (nwrite != (ssize_t)local[0].iov_len) {
    std::cout << "writev failed: " << errno << "\n";
    std::cout << "Error string:" << strerror(errno) << "\n";
    abort();
  }
  std::cout << "nwrite successful\n";
  return nwrite;
}

bool RemoteMemory::Contains(const pid_t pid, const RemoteHeapPointer& ptr,
			    const std::vector<ValidTypes>& contains) {

  char* const ptr_copy = RemoteMemory::Copy(pid, ptr.points_to, ptr.size_pointed_to);
  for (const auto& i : contains) {
    ssize_t offset = FindFirstJumpTable(ptr,ptr_copy,i);

    // We failed to find a required element
    if (offset == kNotFoundOffset) {
      delete[] ptr_copy;
      return false;
    }

    // Set the currently found match to 0 so if the user asks to find 2 of the same value the same
    // memory location won't match for both
    ptr_copy[offset] = 0;
  }

  delete[] ptr_copy;
  return true;
}

ssize_t RemoteMemory::Substitute(const pid_t pid,const RemoteHeapPointer& ptr,const Substitutions& subsitutions){
  const char* const ptr_copy = RemoteMemory::Copy(pid,ptr.points_to,ptr.size_pointed_to);

  size_t total_bytes_written = 0;
  for (const auto& substitution : subsitutions){
    const ssize_t offset = FindFirstJumpTable(ptr,ptr_copy,substitution.from);
    if (offset==kNotFoundOffset){
      cerr << "Failed to find value when substituting\n";
      continue;
    }
    const ssize_t bytes_written = RemoteMemory::Write(pid,AddToVoid(ptr.points_to,offset),substitution.to);
    total_bytes_written+=bytes_written;
  }

  delete[] ptr_copy;
  return total_bytes_written;
}

ssize_t RemoteMemory::FindFirst(const char* start, const size_t size, const std::string& to_find) {
  // Careful not to run strcmp as memory that isn't actually a string is likely not null terminated.
  // We must use strncmp.
  // Careful not to run strncmp near the end of the buffer as it could read
  // past the end of the buffer pointed to by start
  for (size_t i = 0; i < (size - to_find.size()); i++) {
    if (std::strncmp(start + i, to_find.c_str(), to_find.size()) == 0) {
      return i;
    }
  }

  return kNotFoundOffset;
}

template <typename T>
ssize_t RemoteMemory::FindFirst(const char* start, const size_t size, ValidTypes to_find) {
  const size_t sizeof_current = std::visit(ValidTypesVisitor{}, to_find);

  for (size_t i = 0; i < size; i += sizeof_current) {
    if (std::get<T>(to_find) == *((T*)(start + i))) {
      return i;
    }
  }
  return kNotFoundOffset;
}

ssize_t RemoteMemory::FindFirstJumpTable(
		const RemoteHeapPointer& ptr, const char* const ptr_copy, ValidTypes to_find) {
  // TODO find a way to not hard code this for the number of elements in ValidTypes
  // The best alternative found is to fail to compile if the number of ValidTypes changes
  // but will compile if the elements in ValidTypes are changed or reordered
  static_assert(std::variant_size_v<ValidTypes> == 5,
		"The number of elements in ValidTypes has changed."
		"You need to update RemoteMemory::FindFirstJumpTable");

  ssize_t offset = 0;
  switch (to_find.index()) {
  case 0:
    offset = FindFirst<std::variant_alternative_t<0, ValidTypes>>(
		    ptr_copy, ptr.size_pointed_to, to_find);
    break;
  case 1:
    offset = FindFirst<std::variant_alternative_t<1, ValidTypes>>(
		    ptr_copy, ptr.size_pointed_to, to_find);
    break;
  case 2:
    offset = FindFirst<std::variant_alternative_t<2, ValidTypes>>(
		    ptr_copy, ptr.size_pointed_to, to_find);
    break;
  case 3:
    offset = FindFirst<std::variant_alternative_t<3, ValidTypes>>(
		    ptr_copy, ptr.size_pointed_to, to_find);
    break;
  case 4:
    static_assert(std::is_same_v<std::string, std::variant_alternative_t<4, ValidTypes>>,
		  "The index of std::string in ValidTypes has changed. You need to update "
		  "RemoteMemory::FindFirstJumpTable");
    offset = FindFirst(ptr_copy, ptr.size_pointed_to, std::get<std::string>(to_find));
    break;
  }
  return offset;
}

