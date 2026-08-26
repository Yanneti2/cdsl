# TODO:  <09-05-26, KGM> # Think a better way to handle the test flag on gpt-originals
# TODO:  <09-05-26, KGM> # Think a better way to handle the masks


add_library(libgpt
        bitsequence.cpp
        bitvectorRRR.cpp
        endian.c
        uvector.cpp
        ${CMAKE_SOURCE_DIR}/src/masks/mask01.cpp
        ${CMAKE_SOURCE_DIR}/src/masks/mask10.cpp
        ${CMAKE_SOURCE_DIR}/src/masks/masks0.cpp
        ${CMAKE_SOURCE_DIR}/src/masks/masks1.cpp
)

target_include_directories(libgpt PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_definitions(libgpt PRIVATE DEBUG)
