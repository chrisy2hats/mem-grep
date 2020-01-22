#ifndef UNIT_TESTS_UTILS_HPP
#define UNIT_TESTS_UTILS_HPP
#include <stdio.h> //For perror
#include <unistd.h> //For execl,fork 
#include <stdlib.h> //For exit
#include <iostream>
#include <csignal>
#include <functional>

// Launches the provided program in a seperate thread.
// This is equivalent to doing "./foobar &" in bash
// By default the function waits a second after spawning the target program
// This is to allow the kernel to populate the /proc/PID/maps file
// Most tests calling this will want this as they parse the maps file
// If not the wait can be disable via setting the wait parameter to false
pid_t LaunchProgram(const char* command, bool wait=true, int wait_time=1);

void ForeachTargetProgram(const std::function<void(const pid_t,const std::string&)>& functor );
#endif //UNIT_TESTS_UTILS_HPP
