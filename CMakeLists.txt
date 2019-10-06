cmake_minimum_required(VERSION 3.00)
project(mem-analyze)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -fpermissive")
set(EXECUTABLE_OUTPUT_PATH "out")

add_executable(mem-analyze
        src/main.cpp
        src/map-parser.cpp src/map-parser.hpp
        src/memory-searcher.hpp
        src/memory-writer.hpp
        )

#Build sample target-programs to be analysed
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -pedantic")

project(onstackint)
add_executable(onstackint
        target-programs/onstackint/main.c
        )

project(onheapint)
add_executable(onheapint
        target-programs/onheapint/main.c
        )

#Run unit tests
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/tests/catch2")
    message(FATAL_ERROR "Catch2 submodule not initialised.")
endif ()
include_directories(tests/catch2/single_include)
add_executable(unit-tests
        tests/test_main.cpp
        tests/map-parser-test.cpp

        src/map-parser.cpp
        src/map-parser.hpp
        )

#Generate cppcheck report
add_custom_target(cppcheck
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND rm -rf report 2>/dev/null
        COMMAND mkdir report
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem --xml --xml-version=2 ./src 2>report/report-src.xml
        COMMAND cppcheck-htmlreport --source-dir=. --title=mem-analyse --file=report/report-src.xml --report-dir=report
        )