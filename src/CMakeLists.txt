add_library(compact
        bitvector.cpp
        bitvectorj.cpp
        huffman.cpp
        louds.cpp
        parentheses_tree.cpp
        parentheses_tree_rmmq.cpp
        RRR.cpp
        huffman.cpp
        wt.cpp
        wt_succint.cpp
        uvector.cpp
        permutation.cpp
)
target_include_directories(compact PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(utils)

target_link_libraries(compact PRIVATE utils)
