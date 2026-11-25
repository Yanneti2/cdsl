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
target_link_libraries(RRR-tester compact malloc_count)

target_include_directories(RRR-tester PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)

add_executable(rrr-tester
	rrr-tester.cpp
)
target_link_libraries(rrr-tester compact malloc_count)

target_include_directories(rrr-tester PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)

add_executable(comp-rrr
        comp-rrr.cpp
)
target_link_libraries(comp-rrr compact)

add_executable(wt-tester
        wt-tester.cpp
)
target_link_libraries(wt-tester compact)

add_executable(uvector_tester
        uvector_tester.cpp
)
target_link_libraries(uvector_tester compact)

add_executable(rmmq_tester
        rmmq_tester.cpp
)
target_link_libraries(rmmq_tester compact)


add_executable(permutation_tester 
    permutation_tester.cpp
)
target_link_libraries(permutation_tester compact)

add_executable(granularity-test 
    granularity-test.cpp
)
target_link_libraries(granularity-test compact)

add_test(NAME TestPermutations COMMAND permutation_tester)
