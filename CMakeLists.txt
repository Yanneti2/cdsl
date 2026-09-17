cmake_minimum_required(VERSION 3.28)
project(EstruturasDeDadosCompactas C CXX)

# Habilitando otimizações de compilação por padrão
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


option(BUILD_TESTS "Habilita a compilação dos testes" ON)

add_subdirectory(src)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif()
