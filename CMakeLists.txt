cmake_minimum_required(VERSION 3.00)
project(mem-grep)

FIND_PACKAGE (Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -fpermissive -g")
set(EXECUTABLE_OUTPUT_PATH "out")

add_executable(mem-grep
        src/main.cpp
        src/map-parser.cpp src/map-parser.hpp
        src/bss-searcher.cpp src/bss-searcher.hpp
        src/malloc-metadata.cpp src/malloc-metadata.hpp
        src/prerun-checks.cpp src/prerun-checks.hpp
        src/stack-searcher.cpp src/stack-searcher.hpp
        src/heap-traverser.cpp src/heap-traverser.hpp
        src/argument-parsing.cpp src/argument-parsing.hpp
        src/utils.cpp src/utils.hpp
        src/remote-memory.cpp src/remote-memory.hpp
	)
target_link_libraries(mem-grep Threads::Threads)


#Programs for manual testing
project(onstackint C)
add_executable(onstackint
        tests/manual-target-programs/onstackint/main.c
        )

project(onheapint C)
add_executable(onheapint
        tests/manual-target-programs/onheapint/main.c
        )

project(unit-tests)
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/tests/catch2")
    message(FATAL_ERROR "Catch2 submodule not initialised.")
endif ()
include_directories(tests/catch2/single_include)
add_executable(unit-tests
        tests/test_main.cpp
        tests/null-structs.hpp

        tests/map-parser-test.cpp
        tests/memory-searcher-test.cpp
        tests/prerun-checks-test.cpp
        tests/bss-searcher-test.cpp
        tests/heap-traverser-test.cpp

        src/map-parser.cpp src/map-parser.hpp
        src/remote-memory.cpp src/remote-memory.hpp
        src/prerun-checks.cpp src/prerun-checks.hpp
        src/bss-searcher.cpp src/bss-searcher.hpp
        src/malloc-metadata.cpp src/malloc-metadata.hpp
        src/heap-traverser.cpp src/heap-traverser.hpp
        tests/null-structs.cpp
        tests/utils.cpp tests/utils.hpp
        )
target_link_libraries(unit-tests Threads::Threads)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -pedantic")

#Build sample target-programs to be analysed
project(unit-tests-target-programs C)
add_executable(runUntilManipulatedHeap
        tests/target-programs/runUntilManipulatedHeap.c
        )

add_executable(bssHeapPointers
        tests/target-programs/bssHeapPointers.c
        )

add_executable(runUntilManipulatedStack
        tests/target-programs/runUntilManipulatedStack.c
        )

add_executable(multiLayeredBssHeapPointers
        tests/target-programs/multiLayerBssHeapPointers.c
        )

#Build basic assembly program for use in testing
set(CMAKE_NASM_LINK_EXECUTABLE "ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
project("asmTarget" ASM_NASM)
enable_language(ASM_NASM)
add_executable("asmTarget" tests/target-programs/asmTarget.asm)
set_target_properties("asmTarget" PROPERTIES LINKER_LANGUAGE NASM)

add_custom_target(cppcheck
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND rm -rf report 2>/dev/null
        COMMAND mkdir report
        COMMAND cppcheck --enable=all --suppress=missingIncludeSystem --xml --xml-version=2 ./src 2>report/report-src.xml
        COMMAND cppcheck-htmlreport --source-dir=. --title=mem-grep --file=report/report-src.xml --report-dir=report
        )
