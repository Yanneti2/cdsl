add_library(wavelet-tree_benchmark
    wavelet-tree_benchmark.cpp
)
target_link_libraries(wavelet-tree_benchmark compact)

add_executable(wavelet-benchmark
    wavelet-tree_benchmark.cpp
    main.cpp
)
target_link_libraries(wavelet-benchmark compact
    malloc_count
    interquartil-stats
    wavelet-tree_benchmark
)
target_include_directories(wavelet-benchmark PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)