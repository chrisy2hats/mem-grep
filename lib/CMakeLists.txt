add_library(mem-grep-core
        misc/map-parser.cpp misc/map-parser.hpp
        heap-traversing/bss-searcher.cpp heap-traversing/bss-searcher.hpp
        misc/malloc-metadata.cpp misc/malloc-metadata.hpp
        misc/prerun-checks.cpp misc/prerun-checks.hpp
        heap-traversing/stack-searcher.cpp heap-traversing/stack-searcher.hpp
        heap-traversing/heap-traverser.cpp heap-traversing/heap-traverser.hpp
        misc/structs.cpp misc/structs.hpp
        misc/remote-memory.cpp misc/remote-memory.hpp
        filtering/heap-filter.cpp filtering/heap-filter.hpp
        filtering/lambda-creator.cpp filtering/lambda-creator.hpp
        misc/utils.hpp misc/utils.cpp
        datastructures/bool-vec.hpp datastructures/bool-vec.cpp)

target_link_libraries(mem-grep-core Threads::Threads)