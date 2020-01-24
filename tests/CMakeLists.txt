cmake_minimum_required(VERSION 3.00)

FIND_PACKAGE (Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -fpermissive -g")
set(EXECUTABLE_OUTPUT_PATH "out")

#Programs for manual testing
project(onstackint C)
add_executable(onstackint
        manual-target-programs/onstackint/main.c
        )

project(onheapint C)
add_executable(onheapint
        manual-target-programs/onheapint/main.c
        )

project(unit-tests)
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/catch2")
    message(FATAL_ERROR "Catch2 submodule not initialised.")
endif ()
include_directories(catch2/single_include)
add_executable(unit-tests
        map-parser-test.cpp
        memory-searcher-test.cpp
        prerun-checks-test.cpp
        bss-searcher-test.cpp
        heap-traverser-test.cpp
        null-structs.cpp
        heap-filter-test.cpp

        test_main.cpp
        null-structs.hpp
        utils.cpp
        utils.hpp

        ../src/misc/map-parser.cpp ../src/misc/map-parser.hpp
        ../src/misc/remote-memory.cpp ../src/misc/remote-memory.hpp
        ../src/misc/prerun-checks.cpp ../src/misc/prerun-checks.hpp
        ../src/heap-traversing/bss-searcher.cpp ../src/heap-traversing/bss-searcher.hpp
        ../src/misc/malloc-metadata.cpp ../src/misc/malloc-metadata.hpp
        ../src/heap-traversing/heap-traverser.cpp ../src/heap-traversing/heap-traverser.hpp
        ../src/filtering/heap-filter.cpp ../src/filtering/heap-filter.hpp
        )
target_link_libraries(unit-tests Threads::Threads)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -pedantic")

#Build sample target-programs to be analysed
project(unit-tests-target-programs C)
add_executable(runUntilManipulatedHeap
        target-programs/runUntilManipulatedHeap.c
        )

add_executable(bssHeapPointers
        target-programs/bssHeapPointers.c
        )

add_executable(runUntilManipulatedStack
        target-programs/runUntilManipulatedStack.c
        )

add_executable(multiLayeredBssHeapPointers
        target-programs/multiLayerBssHeapPointers.c
        )

#Build basic assembly program for use in testing
set(CMAKE_NASM_LINK_EXECUTABLE "ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
project("asmTarget" ASM_NASM)
enable_language(ASM_NASM)
add_executable("asmTarget" target-programs/asmTarget.asm)
set_target_properties("asmTarget" PROPERTIES LINKER_LANGUAGE NASM)
