add_library(wavelet-tree_benchmark
    wavelet-tree_benchmark.cpp
)
target_link_libraries(wavelet-tree_benchmark compact)

add_executable(wavelet-benchmark
    main.cpp
)
target_link_libraries(wavelet-benchmark compact
    interquartil-stats
    wavelet-tree_benchmark
)