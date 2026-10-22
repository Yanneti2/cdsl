add_executable(bitvector-bsb
    bitvector-bsb.cpp
)
target_link_libraries(bitvector-bsb compact malloc_count)
target_include_directories(bitvector-bsb PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)
