add_executable(RRR-benchmark
    RRR-tester.cpp
)
target_link_libraries(RRR-benchmark compact malloc_count)

target_include_directories(RRR-benchmark PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)

add_executable(RRR-bsb
    RRR-bsb.cpp
)
target_link_libraries(RRR-bsb compact malloc_count)

target_include_directories(RRR-bsb PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)
