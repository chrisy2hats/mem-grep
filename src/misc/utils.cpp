#include "utils.hpp"

using std::cout;
using std::cerr;

pid_t pidof(const std::string &program) {
  // Calling the system command pidof here is dirty but gets the job done.
  // The proper way to do this would be to iterate over every directory
  // In /proc/ but that is very long winded
  const std::string command = "pidof " + program;
  char result[15];
  FILE *cmd = popen(command.c_str(), "r");
  fgets(result, 15, cmd);
  const pid_t pid = strtoul(result, NULL, 10);
  pclose(cmd);
  return pid;
}
