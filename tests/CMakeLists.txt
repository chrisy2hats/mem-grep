cmake_minimum_required(VERSION 3.00)
project(unit-tests)

FIND_PACKAGE (Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Werror -Wall -Wextra -Wreorder -Weffc++ -Wpedantic -Wimplicit-fallthrough")
set(EXECUTABLE_OUTPUT_PATH "out")


if (NOT EXISTS "${PROJECT_SOURCE_DIR}/catch2")
    message(FATAL_ERROR "Catch2 submodule not initialised. Please refer to https://github.com/ChrisFoster4/mem-grep/blob/master/docs/BuildingUnitTests.md")
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

        ../src/shared/misc/map-parser.cpp ../src/shared/misc/map-parser.hpp
        ../src/shared/misc/remote-memory.cpp ../src/shared/misc/remote-memory.hpp
        ../src/shared/misc/prerun-checks.cpp ../src/shared/misc/prerun-checks.hpp
        ../src/shared/heap-traversing/bss-searcher.cpp ../src/shared/heap-traversing/bss-searcher.hpp
        ../src/shared/misc/malloc-metadata.cpp ../src/shared/misc/malloc-metadata.hpp
        ../src/shared/heap-traversing/heap-traverser.cpp ../src/shared/heap-traversing/heap-traverser.hpp
        ../src/shared/filtering/heap-filter.cpp ../src/shared/filtering/heap-filter.hpp
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

add_executable(oneTwoSevenOnStack
	target-programs/oneTwoSevenOnStack.c
        )

add_executable(multiLayeredBssHeapPointers
        target-programs/multiLayerBssHeapPointers.c
        )

#Programs for manual testing
project(onstackint C)
add_executable(onstackint
        manual-target-programs/onstackint/main.c
        )

project(onheapint C)
add_executable(onheapint
        manual-target-programs/onheapint/main.c
        )


#Build basic assembly program for use in testing
set(CMAKE_NASM_LINK_EXECUTABLE "ld <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CAN_USE_ASSEMBLER TRUE)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
project("asmTarget" ASM_NASM)
enable_language(ASM_NASM)
add_executable("asmTarget" target-programs/asmTarget.asm)
set_target_properties("asmTarget" PROPERTIES LINKER_LANGUAGE NASM)

