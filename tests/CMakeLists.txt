include_directories(${CMAKE_SOURCE_DIR}/include)



# Desativados pois dependem do libgpt, que atualmente nao compila

# add_executable(bits-tester
#         bits-tester.cpp
# )
# 
# target_link_libraries(bits-tester PRIVATE libgpt)
#
# add_executable(endian-tester
#         endian-tester.c
# )
# target_link_libraries(endian-tester PRIVATE libgpt)
# add_executable(uarray-tester
#         uarray-tester.c
# )
# target_link_libraries(uarray-tester PRIVATE libgpt)


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
