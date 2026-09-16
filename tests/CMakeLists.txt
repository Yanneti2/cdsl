include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(balanced_parenthesis-tester
    balanced_parenthesis-tester.cpp
)
target_link_libraries(balanced_parenthesis-tester compact)

add_test(NAME TestBalancedParenthesis COMMAND balanced_parenthesis-tester)


add_executable(bitvector-tester
        bitvector-tester.cpp
)
target_link_libraries(bitvector-tester compact)


add_executable(huffman-tester
        huffman-tester.cpp
)
target_link_libraries(huffman-tester compact)


add_executable(louds-tester
        louds-tester.cpp
)
target_link_libraries(louds-tester compact)


add_executable(RRR-tester
	RRR-tester.cpp
)
target_link_libraries(RRR-tester compact)


add_executable(wt-tester
        wt-tester.cpp
)
target_link_libraries(wt-tester compact)
