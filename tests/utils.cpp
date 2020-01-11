#include "utils.hpp"

pid_t launchProgram( const char* command, bool wait /*=true*/, int wait_time /*=1*/) {
  pid_t pid;
  if ((pid = fork()) == 0) {
    // We are in the child process
    printf("Launching target\n");
    pid_t lpid = execl(command, command, (char*)0);
    if (lpid == -1) {
      perror("execl failed. Exiting...");
      exit(1);
    }

    // We should NEVER reach here as execl has replaced this thread with the
    // target binary
    exit(1);
  } else if (pid > 0) {
    // Return the pid of the sub process back to the caller
    if (wait) {
      // Give the kernel a chance to load the process and initialise the
      // /proc/maps file A way to avoid sleep but ensuring that the program has
      // been loaded would be ideal but this works If the caller doesn't want
      // this sleep they can set wait to false when the call this function
      // However as most tests launch a program then analyse it's maps file this
      // is normally needed
      sleep(wait_time);
    }
    return pid;
  } else {
    perror("Forked failed!\n");
    exit(1);
  }
}
