#include "prerun-checks.hpp"

//To be able to read memory from another process either the kernel has to have been told to allow this (via ptrace_scope)
//Or we need to be running as root
//Or both.
[[nodiscard]] bool PreRunCheck(void) {
    //I don't know of any architecture where this will fail
    //But if this isn't the case all the memcpys from pointers will fail
    if (sizeof(void *) != sizeof(size_t)) {
        std::cout << "Program running on unusual architecture where a pointer isn't as large as a size_t.Don't be surprised if this doesn't work...\n";
    }

    //If euid is 0 you are effectively running as root but not as root via setuid or other means
    //This might work but I haven't tested. Warn the user that this isn't tested or supported and move on
    auto euid = geteuid();
    auto uid = getuid();

    const std::ifstream PTRACE_SCOPE_FILE("/proc/sys/kernel/yama/ptrace_scope");
    std::stringstream buffer;
    buffer << PTRACE_SCOPE_FILE.rdbuf();
    const int ptrace_scope = std::stoi(buffer.str());
    if (ptrace_scope == 0) {
        return true;
    }

    if (uid == 0) {
        return true;
    }
    if (euid == 0) {
        std::cout << "WARNING: running with euid as 0 but not uid 0.\n This isn't supported but the program will attempt to continue.\n If you get permission denied type errors this is probably why!\n";
        return true;
    }
    std::cout << "ERROR: Yama ptrace to restricted and program not run as root. mem-grep cannot run as it can't access other processes memory. Exiting now...\n";

    return false;
}