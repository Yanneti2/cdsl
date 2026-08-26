add_library(utils
    stack.cpp
    queue.cpp
    ll2.cpp
    dll.cpp
	general_tree.cpp
	binary_tree.cpp
)

target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
