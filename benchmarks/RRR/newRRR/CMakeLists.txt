add_executable(newRRR-bsb
    newRRR-bsb.cpp
)

target_link_libraries(newRRR-bsb compact malloc_count)

target_include_directories(newRRR-bsb PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/malloc_count)
