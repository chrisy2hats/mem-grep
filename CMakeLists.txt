cmake_minimum_required(VERSION 3.00)
project(mem-grep)
set(MEM-GREP-ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(MEM-GREP-OUT "${MEM-GREP-ROOT}/out")

#pthreads on Unix-like systems
FIND_PACKAGE (Threads REQUIRED)

option(BUILD_UNIT_TESTS "Build the unit tests and associated programs" OFF)
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

add_executable(mem-grep
        src/CLI/main.cpp
		src/CLI/argument-parsing/argument-parsing.cpp src/CLI/argument-parsing/argument-parsing.hpp

		src/shared/misc/map-parser.cpp src/shared/misc/map-parser.hpp
		src/shared/heap-traversing/bss-searcher.cpp src/shared/heap-traversing/bss-searcher.hpp
		src/shared/misc/malloc-metadata.cpp src/shared/misc/malloc-metadata.hpp
		src/shared/misc/prerun-checks.cpp src/shared/misc/prerun-checks.hpp
		src/shared/heap-traversing/stack-searcher.cpp src/shared/heap-traversing/stack-searcher.hpp
		src/shared/heap-traversing/heap-traverser.cpp src/shared/heap-traversing/heap-traverser.hpp
		src/shared/misc/structs.cpp src/shared/misc/structs.hpp
		src/shared/misc/remote-memory.cpp src/shared/misc/remote-memory.hpp
		src/shared/filtering/heap-filter.cpp src/shared/filtering/heap-filter.hpp
		src/shared/filtering/lambda-creator.cpp src/shared/filtering/lambda-creator.hpp
		src/shared/misc/utils.hpp src/shared/misc/utils.cpp
		src/shared/analyse-program.cpp src/shared/analyse-program.hpp)
target_link_libraries(mem-grep Threads::Threads)

add_custom_target(cppcheck
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND rm -rf report 2>/dev/null
        COMMAND mkdir report
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem --xml --xml-version=2 ./src 2>report/report-src.xml
        COMMAND cppcheck-htmlreport --source-dir=. --title=mem-grep --file=report/report-src.xml --report-dir=report
        )
