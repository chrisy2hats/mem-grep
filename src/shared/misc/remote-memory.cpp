#include "remote-memory.hpp"

using std::cerr;
using std::cout;

char* RemoteMemory::Copy(const pid_t& pid, const void* start, const size_t& size) {
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
  ssize_t nread = 0;
  nread = process_vm_readv(pid, local, 1, remote, 1, 0);
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

bool RemoteMemory::Contains(const pid_t pid, const RemoteHeapPointer& ptr,
		const std::vector<ValidTypes>& contains) {
  // TODO find a way to not hard code this for the number of elements in ValidTypes
  // The best alternative found is to fail to compile if the number of ValidTypes changes
  // but will compile if the elements in ValidTypes are changed or reordered
  static_assert(std::variant_size_v<ValidTypes> == 4,
		  "The number of elements in ValidTypes has changed."
		  "You need to update RemoteMemory::Contains");

  char* ptr_copy = RemoteMemory::Copy(pid, ptr.points_to, ptr.size_pointed_to);
  for (const auto& i : contains) {
    ssize_t offset = 0;
    switch (i.index()) {
    case 0:
      offset = FindFirst<std::variant_alternative_t<0, ValidTypes>>(
		      ptr_copy, ptr.size_pointed_to, i);
      break;
    case 1:
      offset = FindFirst<std::variant_alternative_t<1, ValidTypes>>(
		      ptr_copy, ptr.size_pointed_to, i);
      break;
    case 2:
      offset = FindFirst<std::variant_alternative_t<2, ValidTypes>>(
		      ptr_copy, ptr.size_pointed_to, i);
      break;
    case 3:
      offset = FindFirst<std::variant_alternative_t<3, ValidTypes>>(
		      ptr_copy, ptr.size_pointed_to, i);
      break;
    }

    // We failed to find a required element
    if (offset == kNotFoundOffset)
      return false;

    // Set the currently found match to 0 so if the user asks to find 2 of the same value the same
    // memory location won't match for both
    ptr_copy[offset] = 0;
  }

  delete[] ptr_copy;
  return true;
}
