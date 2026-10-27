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
        wt_implicit.cpp
        uvector.cpp
        permutation.cpp
)
target_include_directories(compact PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(utils)

target_link_libraries(compact PRIVATE utils)
if (NBITS AND NBITS512)
    message(CMAKE_ERROR "Cannot use two different block sizes")
endif()

if (NBITS)
    message("Using NBITS as jacobson block size")
    target_compile_definitions(compact PRIVATE _nbits)
else()
    if (NBITS512)
    message("Using NBITS512 as jacobson block size")
    target_compile_definitions(compact PRIVATE _nbits512)

    else()
        message("Using log as jacobson block size")
    endif()
endif()
