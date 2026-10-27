add_executable(bitvectorBS-bsb
    bitvectorBS-bsb.cpp
)
target_link_libraries(bitvectorBS-bsb compact malloc_count)
target_include_directories(bitvectorBS-bsb PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)
