include_directories(${CMAKE_SOURCE_DIR}/benchmarks)

add_subdirectory(jacobsonrank)
add_subdirectory(RRR)
add_subdirectory(wavelet-tree)
add_subdirectory(parentheses-tree)

add_library(interquartil-stats
    interquartil-stats.cpp
)

target_include_directories(interquartil-stats PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(compact PRIVATE utils)
