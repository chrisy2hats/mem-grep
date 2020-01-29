cmake_minimum_required(VERSION 3.00)
project(mem-grep)

#pthreads on Unix-like systems
FIND_PACKAGE (Threads REQUIRED)

option(BUILD_UNIT_TESTS "Build the unit tests and associated programs" OFF)
if (BUILD_UNIT_TESTS)
	add_subdirectory(tests)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wreorder -Weffc++ -Wpedantic -Wimplicit-fallthrough")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")
set(EXECUTABLE_OUTPUT_PATH "out")

add_executable(mem-grep
        src/main.cpp
        src/misc/map-parser.cpp src/misc/map-parser.hpp
        src/heap-traversing/bss-searcher.cpp src/heap-traversing/bss-searcher.hpp
        src/misc/malloc-metadata.cpp src/misc/malloc-metadata.hpp
        src/misc/prerun-checks.cpp src/misc/prerun-checks.hpp
        src/heap-traversing/stack-searcher.cpp src/heap-traversing/stack-searcher.hpp
        src/heap-traversing/heap-traverser.cpp src/heap-traversing/heap-traverser.hpp
        src/argument-parsing/argument-parsing.cpp src/argument-parsing/argument-parsing.hpp
		src/misc/structs.cpp src/misc/structs.hpp
        src/misc/remote-memory.cpp src/misc/remote-memory.hpp
        src/filtering/heap-filter.cpp src/filtering/heap-filter.hpp
		src/filtering/lambda-creator.cpp src/filtering/lambda-creator.hpp
		src/misc/utils.hpp)
target_link_libraries(mem-grep Threads::Threads)

add_custom_target(cppcheck
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND rm -rf report 2>/dev/null
        COMMAND mkdir report
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem --xml --xml-version=2 ./src 2>report/report-src.xml
        COMMAND cppcheck-htmlreport --source-dir=. --title=mem-grep --file=report/report-src.xml --report-dir=report
        )
