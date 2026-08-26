add_library(compact
        bitvector.cpp
        parentheses-tree.cpp
        huffman.cpp
        jacobsonrank.cpp
        louds.cpp
)

target_include_directories(compact PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(GPT-originals)
add_subdirectory(utils)

target_link_libraries(compact PRIVATE utils)
