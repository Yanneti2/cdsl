include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(binary_tree-tester
        binary_tree-tester.cpp
)

target_link_libraries(binary_tree-tester compact)

add_executable(bitvectorRRR-tester
        bitvectorRRR-tester.cpp
)

target_link_libraries(bitvectorRRR-tester libgpt)
target_compile_definitions(bitvectorRRR-tester PRIVATE DEBUG)



add_executable(huffman-tester
        huffman-tester.cpp
)

target_link_libraries(huffman-tester compact)


add_executable(bitvector-tester
        bitvector-tester.cpp
)

target_link_libraries(bitvector-tester compact)

add_executable(dll-tester
        dll-tester.cpp
)
target_link_libraries(dll-tester utils)


add_executable(jacobsonrank-tester
        jacobsonrank-tester.cpp
)
target_link_libraries(jacobsonrank-tester compact)


add_executable(ll2-tester
        ll2-tester.cpp
)
target_link_libraries(ll2-tester utils)

# TODO: Fix this
# add_executable(utils-tester
#         utils-tester.cpp
# )
# 
# target_link_libraries(utils-tester libgpt compact)


add_executable(balanced_parenthesis-tester
    balanced_parenthesis-tester.cpp
)

target_link_libraries(balanced_parenthesis-tester compact)

# add_test(NAME TestBalancedParenthesis COMMAND balanced_parenthesis-tester)
