add_subdirectory(newRRR)

add_executable(RRR-benchmark
    RRR-tester.cpp
)

target_link_libraries(RRR-benchmark compact malloc_count)

target_include_directories(RRR-benchmark PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)
