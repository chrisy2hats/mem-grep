cmake_minimum_required(VERSION 3.00)
project(linked-list-example)

add_executable(linked-list-analyser
        analyser.cpp
        )

target_link_libraries(linked-list-analyser LINK_PRIVATE mem-grep-core)


add_executable(linked-list-target
        target.c
        )