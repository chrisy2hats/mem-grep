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


option(BUILD_WITH_SANITISER "Build with address and UB sanitisation" OFF)

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

add_library(mem-grep-core lib/misc/map-parser.cpp lib/misc/map-parser.hpp
		lib/heap-traversing/bss-searcher.cpp lib/heap-traversing/bss-searcher.hpp
		lib/misc/malloc-metadata.cpp lib/misc/malloc-metadata.hpp
		lib/misc/prerun-checks.cpp lib/misc/prerun-checks.hpp
		lib/heap-traversing/stack-searcher.cpp lib/heap-traversing/stack-searcher.hpp
		lib/heap-traversing/heap-traverser.cpp lib/heap-traversing/heap-traverser.hpp
		lib/misc/structs.cpp lib/misc/structs.hpp
		lib/misc/remote-memory.cpp lib/misc/remote-memory.hpp
		lib/filtering/heap-filter.cpp lib/filtering/heap-filter.hpp
		lib/filtering/lambda-creator.cpp lib/filtering/lambda-creator.hpp
		lib/misc/utils.hpp lib/misc/utils.cpp
		lib/datastructures/bool-vec.hpp lib/datastructures/bool-vec.cpp
		)

add_executable(cli
        src/CLI/main.cpp
		src/CLI/argument-parsing/argument-parsing.cpp src/CLI/argument-parsing/argument-parsing.hpp
		src/analyse-program.cpp src/analyse-program.hpp)
		
target_link_libraries(cli LINK_PRIVATE mem-grep-core)



target_link_libraries(mem-grep-core Threads::Threads)

add_custom_target(cppcheck
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND rm -rf report 2>/dev/null
        COMMAND mkdir report
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem --xml --xml-version=2 ./src 2>report/report-src.xml
        COMMAND cppcheck-htmlreport --source-dir=. --title=mem-grep --file=report/report-src.xml --report-dir=report
        )
