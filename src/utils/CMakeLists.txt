add_library(utils
	general_tree.cpp
	binary_tree.cpp
)

target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
