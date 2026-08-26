#include "../include/jacobsonrank.h" 
#include "../include/bitvector.h"
#include <iostream>
#include <chrono>
#include <math.h>
#include <string>
#include <time.h>
#include <cstdlib>

//std::popcount
//usar de ref len NBIS

int main(int argc, char *argv[]) {
    bool verbose = false;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            verbose = true;
    }

    if (verbose) {
        string ss = " NAO ";
        std::cout << "\n";
        std::cout
            << "==============================================================="
            << std::endl;
        std::cout << "OS TESTES A SEGUIR USARAM CHUNK SIZE: ";

        #ifdef _nbits
        std::cout << "NBITS e NBITS*NBITS" << std::endl;
        #endif

        #ifdef _nbits512
        std::cout << "NBITS e 512*NBITS" << std::endl;
        #endif

        #ifdef _log
        std::cout << "logN e logN*logN" << std::endl;
        #endif

        #ifdef selectstructure
        ss = " ";
        #endif

        std::cout << "\n\n";
        std::cout << "ALEM DISSO, ELES" << ss << "USARAM A ESTRUTURA AUXILIAR DE SELECT" << std::endl;
        std::cout << "===============================================================" << "\n\n\n\n";

    } else {
        std::cout << "\"Size\";\"Time\"\n";
    }

    for (long long unsigned size = 1000; size < 10000000; size *= 10) {
        for(long long unsigned j = 0; j < 100; j++){
            auto B1 = bitVector(64, 2.0);
            int ordem = log10(size);

            srand(time(0));
            for (TYPE i = 0; i < size; i++) {
                if (rand() % 2 == 0) {
                    B1.append1();
                } else {
                    B1.append0();
                }
            }
            B1.JacobsonRank_build();
            auto start = std::chrono::high_resolution_clock::now();
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::nano> ns_double = end - start;
            
            #ifdef selectstructure
            start = std::chrono::high_resolution_clock::now();
            B1.build_select0();
            // R1.build_select1(&B1);
            end = std::chrono::high_resolution_clock::now();
            ns_double = end - start;
            if (verbose)
                std::cout << "Tempo de construção da estrutura de select1 de ordem " << ordem << ": " << ns_double.count() << " ns\n";
            #endif

            srand(time(0));
            unsigned long long index = rand() % (size);
            // unsigned long long index = rand() % (size / 2);
            auto start1 = std::chrono::high_resolution_clock::now();
            B1.rank1(index);
            auto end1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::nano> ns_double1 = end1 - start1;
            if (verbose) {
                std::cout << "Tempo de 1 operação select1: " << ns_double1.count() << " ns\n\n";
            } else {
                std::cout << size << ";" << ns_double1.count() << "\n";
            }
        }
    }
}
