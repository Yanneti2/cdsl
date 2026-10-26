cmake_minimum_required(VERSION 3.28)
project(EstruturasDeDadosCompactas C CXX)

# Habilitando otimizações de compilação por padrão
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


# Por padrao utiliza LOG
option(NBITS512 "Define tamanho do bloco do jacobson" OFF)
option(NBITS "Define tamanho do bloco do jacobson" OFF)

add_subdirectory(dependencies/malloc_count)
add_subdirectory(src)

option(BUILD_TESTS "Habilita a compilação dos testes" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif()
