add_executable(jacobsonrank-benchmark
    jacobsonrank-tester.cpp
)
target_link_libraries(jacobsonrank-benchmark compact malloc_count)


target_include_directories(jacobsonrank-benchmark PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)


add_executable(jacobsonrank-bsb
    jacobsonrank-bsb.cpp
)
target_link_libraries(jacobsonrank-bsb compact malloc_count)
target_include_directories(jacobsonrank-bsb PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)
