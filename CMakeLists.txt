cmake_minimum_required(VERSION 3.00)
project(mem-grep)
set(MEM-GREP-ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(MEM-GREP-OUT "${MEM-GREP-ROOT}/out")

#pthreads on Unix-like systems
FIND_PACKAGE (Threads REQUIRED)

option(BUILD_UNIT_TESTS "Build the unit tests and associated programs" OFF)
option(BUILD_EXAMPLES "Build the unit tests and associated programs" OFF)
option(BUILD_WITH_SANITISER "Build with address and UB sanitisation" OFF)

if (BUILD_UNIT_TESTS)
	add_subdirectory(tests)
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Werror -Wall -Wextra -Wreorder -Weffc++ -Wpedantic -Wimplicit-fallthrough")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    #Force clang to build std lib function in debug. Otherwise GDB can't see stdlib objects properly
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif ()
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")
set(EXECUTABLE_OUTPUT_PATH "${MEM-GREP-OUT}")
if (BUILD_WITH_SANITISER)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -fno-omit-frame-pointer")
endif()


add_subdirectory(lib)
add_subdirectory(src)
if (BUILD_EXAMPLES)
#	set(EXECUTABLE_OUTPUT_PATH "examples/linked_list")
	unset(EXECUTABLE_OUTPUT_PATH)
	add_subdirectory(examples/linked_list)
endif()

add_custom_target(cppcheck
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND rm -rf report 2>/dev/null
        COMMAND mkdir report
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem --xml --xml-version=2 ./src 2>report/report-src.xml
        COMMAND cppcheck-htmlreport --source-dir=. --title=mem-grep --file=report/report-src.xml --report-dir=report
        )
