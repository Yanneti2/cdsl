add_executable(permutation-benchmark
    main.cpp
)
target_link_libraries(permutation-benchmark compact
    interquartil-stats
)
