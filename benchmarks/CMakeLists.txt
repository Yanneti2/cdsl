include_directories(${CMAKE_SOURCE_DIR}/benchmarks)

add_subdirectory(jacobsonrank)
add_subdirectory(RRR)
add_subdirectory(wavelet-tree)
add_subdirectory(louds)

target_link_libraries(compact PRIVATE utils)
