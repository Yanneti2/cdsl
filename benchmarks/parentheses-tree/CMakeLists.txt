add_library(parentheses-tree_benchmark
    parentheses-tree_benchmark.cpp
)
target_link_libraries(parentheses-tree_benchmark compact
    # parentheses_tree.cpp
    # parentheses_tree_rmmq.cpp
)

add_executable(parentheses_tree-benchmark
    main.cpp
)

target_link_libraries(parentheses_tree-benchmark compact
    interquartil-stats
    parentheses-tree_benchmark
)