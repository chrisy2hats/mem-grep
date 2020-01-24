#include "argument-parsing.hpp"

using std::cerr;
using std::cout;

/*
 *  Aside from parsing the arguments this function should also validate the user arguments
 *
 *  Checks to do:
 *  ERROR: No arguments. Checked via argc
 *  ERROR: Any argument defined twice
 *  ERROR: Any arguments that populate the same field in CLIArgs defined twice i.e. --pid and
 * --pidof both initialise pid ERROR: --stack_frames_to_search provided but then next argument isn't an
 * integer WARN: TraverseBss given but not search_bss WARN: TraverseStack given but not search_stack
 *  WARN: If the user specifies to search the stack but species no amount of stack frames warn them
 * the entire stack will be searched WARN: Bounds checking on stack_frames_to_search not negative and
 * should be less than 2**32. This is an arbitrary limit not a technical one. WARN: If the given
 * --stack_frames_to_search as 0 warn that 0 leads to entire stack being searched. Inform them if they
 * don't want to search the entire stack give the --stack_frames_to_search argument
 *
 *  Once all arguments are iterated over and no errors thrown check:
 *  ERROR: PID not set
 *  ERROR: Neither search_stack or search_bss are defined
 *  WARN: If stack_frames_to_search is set but --search_stack isn't given
 *
 *  TODO implement all validations listed above
 *  TODO maybe this argument parsing be utilising GNU getopts?
 */
struct CLIArgs ArgumentParser::parseArguments(int argc, char **argv) {
  // Initialised with default args defined in the header
  struct CLIArgs args;

  if (argc == 1) {
    cerr << "No arguments provided. Exiting...\n";
    exit(1);
  }

  // Allows to differentiate between users that don't specify --stack_frames_to_search and
  // users that do specify it but want to search the entire stack
  bool SearchEntireStack = false;

  for (auto i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--pid", 6) == 0) {
      args.pid = pid(argv[i + 1]);
      i++;
      continue;
    }
    if (strncmp(argv[i], "--pidof", 8) == 0) {
      args.pid = pidof(argv[i + 1]);
      i++;
      continue;
    }
    if (strncmp(argv[i], "--search_bss", 12) == 0) {
      args.search_bss = true;
      continue;
    }
    if (strncmp(argv[i], "--search_stack", 14) == 0) {
      args.search_stack = true;
      continue;
    }
    if (strncmp(argv[i], "--TraverseBss", 14) == 0) {
      args.search_bss = true;
      args.traverse_bss_pointers = true;
      continue;
    }
    if (strncmp(argv[i], "--TraverseStack", 16) == 0) {
      args.search_stack = true;
      args.traverse_stack_pointers = true;
      continue;
    }
    if (strncmp(argv[i], "--stack_frames_to_search", 22) == 0) {
      size_t frames = std::stoul(argv[i + 1]);
      if (frames == 0) {
	SearchEntireStack = true;
      }
      args.stack_frames_to_search = frames;
      i++;  // Assume the number of frames to search in the next argument so don't look at it
      continue;
    }
    if (strncmp(argv[i], "--MaxHeapObjSize", 17) == 0) {
      args.max_heap_obj_size = std::stoul(argv[i + 1]);
    }
    cerr << "Unknown parameter provided: \"" << argv[i] << "\" Exiting...\n";
    exit(1);
  }

  if (args.max_heap_obj_size == 0) {
    args.max_heap_obj_size = DEFAULT_MAX_HEAP_OBJ_SIZE_;
    cout << "No max heap object size provided. Defaulting to " << args.max_heap_obj_size << '\n';
  }

  // Post parsing validation
  if (args.pid == 0) {
    cerr << "No PID defined via --pid or --pidof\n";
    exit(1);
  }
  if (!args.search_bss && !args.search_stack) {
    cerr << "Program not asked to find pointers on the stack or .bss. Not sure what to do. "
	    "Exiting\n";
    cerr << "Please refer to documentation for examples\n";
    exit(1);
  }
  return args;
}

pid_t ArgumentParser::pidof(const std::string &program) {
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

pid_t ArgumentParser::pid(const std::string &pidStr) {
  const pid_t pid = std::stoul(pidStr);
  cout << "PID parsed as:" << pid << '\n';
  return pid;
}
