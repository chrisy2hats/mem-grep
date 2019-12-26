#include "argument-parsing.hpp"

using std::cout;
using std::cerr;

/*
 *  Aside from parsing the arguments this function should also validate the user arguments
 *
 *  Checks to do:
 *  ERROR: No arguments. Checked via argc
 *  ERROR: Any argument defined twice
 *  ERROR: Any arguments that populate the same field in cliArgs defined twice i.e. --pid and --pidof both initialise pid
 *  ERROR: --StackFramesToSearch provided but then next argument isn't an integer
 *  WARN: TraverseBss given but not SearchBss
 *  WARN: TraverseStack given but not SearchStack
 *  WARN: If the user specifies to search the stack but species no amount of stack frames warn them the entire stack will be searched
 *  WARN: Bounds checking on StackFramesToSearch not negative and should be less than 2**32. This is an arbitrary limit not a technical one.
 *  WARN: If the given --StackFramesToSearch as 0 warn that 0 leads to entire stack being searched. Inform them if they don't want to search the entire stack give the --StackFramesToSearch argument
 *
 *  Once all arguments are iterated over and no errors thrown check:
 *  ERROR: PID not set
 *  ERROR: Neither SearchStack or SearchBss are defined
 *  WARN: If StackFramesToSearch is set but --SearchStack isn't given
 *
 *  TODO implement all validations listed above
 *  TODO maybe this argument parsing be utilising GNU getopts?
 */
struct cliArgs ArgumentParser::parseArguments(int argc, char **argv) {

    //Initialised with default args defined in the header
    struct cliArgs args;

    if (argc==1){
        std::cerr << "No arguments provided. Exiting...\n";
        exit(1);
    }

    //Allows to differentiate between users that don't specify --StackFramesToSearch and
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
        if (strncmp(argv[i], "--SearchBss", 12) == 0) {
            args.SearchBss = true;
            continue;
        }
        if (strncmp(argv[i], "--SearchStack", 14) == 0) {
            args.SearchStack = true;
            continue;
        }
        if (strncmp(argv[i], "--TraverseBss", 14) == 0) {
            args.TraverseBssPointers = true;
            continue;
        }
        if (strncmp(argv[i], "--TraverseStack", 16) == 0) {
            args.TraverseStackPointers = true;
            continue;
        }
        if (strncmp(argv[i], "--StackFramesToSearch", 22) == 0) {
            size_t frames = std::stoul(argv[i + 1]);
            if (frames == 0) {
                SearchEntireStack = true;
            }
            args.StackFramesToSearch =frames;
            continue;
        }
        std::cerr << "Unknown parameter provided: \"" << argv[i] << "\" Exiting...\n";
        exit(1);
    }

    //Post parsing validation
    if (args.pid == 0) {
        std::cerr << "No PID defined via --pid or --pidof\n";
        exit(1);
    }
    if (!args.SearchBss && !args.SearchStack) {
        std::cerr << "Program not asked to find pointers on the stack or .bss. Not sure what to do. Exiting\n";
        std::cerr << "Please refer to documentation for examples\n";
        exit(1);
    }
    return args;
}

pid_t ArgumentParser::pidof(const std::string& program) {
    //Calling the system command pidof here is dirty but gets the job done.
    //The proper way to do this would be to iterate over every directory
    //In /proc/ but that is very long winded
    const std::string command = "pidof " + program;
    char result[15];
    FILE *cmd = popen(program.c_str(), "r");
    fgets(result, 15, cmd);
    const pid_t pid = strtoul(result, NULL, 10);
    pclose(cmd);
    return pid;
}

pid_t ArgumentParser::pid(const std::string &pidStr) {
    const pid_t pid = std::stoul(pidStr);
    std::cout << "PID parsed as:" << pid << std::endl;
    return pid;
}
