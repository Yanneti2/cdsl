#include "../include/jacobsonrank.h" 
#include "../include/bitvector.h"
#include <iostream>
#include <chrono>
#include <math.h>
#include <cstring>
#include <time.h>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <assert.h>

#ifdef _nbits
#define _CHUNK_SIZE "NBITS e NBITS * NBITS\n"
#elifdef _nbits512
#define _CHUNK_SIZE "NBITS e 512 * NBITS\n"
#else
#define _CHUNK_SIZE "LOG(N) E LOG(N) * LOG(N)\n"
#endif

bool randomize_bit(unsigned long i) {
    return rand() % 2 == 0;
}

int main(int argc, char *argv[]) {
    bool verbose = false;
    bool rank0 = false;
    bool rank1 = false;
    bool select0 = false;
    bool select1 = false;
    for (int i = 0; i < argc; i++) {
        if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--verbose") == 0)
            verbose = true;

        if (std::strcmp(argv[i], "-op") == 0 || std::strcmp(argv[i], "--operation") == 0) {
            if (std::strcmp(argv[i + 1], "rank0") == 0 || std::strcmp(argv[i + 1], "r0") == 0) {
                rank0 = true;
            } else if (std::strcmp(argv[i + 1], "rank1") == 0 || std::strcmp(argv[i + 1], "r1") == 0) {
                rank1 = true;
            } else if (std::strcmp(argv[i + 1], "select0") == 0 || std::strcmp(argv[i + 1], "s0") == 0) {
                select0 = true;
            } else if (std::strcmp(argv[i + 1], "select1") == 0 || std::strcmp(argv[i + 1], "s1") == 0) {
                select1 = true;
            }
        }


    }

    if (verbose) {
        string header = 
            "===============================================================\n"
            "OS TESTES A SEGUIR USARAM CHUNK SIZE: " _CHUNK_SIZE
            "ALEM DISSO, ELES "
            #ifndef selectstructure
            "NAO "
            #endif
            "USARAM A ESTRUTURA AUXILIAR DE SELECT\n"
            "===============================================================\n\n"
            ;

        std::cout << header;
    } else {
        std::cout << "\"Size\";\"Time\"\n";
    }

    for (long long unsigned size = 1000; size < 10000000000; size *= 10) {
        srand(time(0));
        bitVector B1(size, randomize_bit);
        int order = log10(size);

        B1.JacobsonRank_build();

        vector<unsigned long long> times(0);

        #ifdef selectstructure
        if (select0) {
            B1.build_select0();       
        } else if (select1) {
            B1.build_select1();       
        }
        #endif
        for (unsigned long long j = 0; j < 999999; j++) {
            std::chrono::high_resolution_clock::time_point start;
            std::chrono::high_resolution_clock::time_point end;

            if (rank0) {
                unsigned long long index = rand() % size;
                start = std::chrono::high_resolution_clock::now();
                B1.rank0(index);
                end = std::chrono::high_resolution_clock::now();
            } else if (rank1) {
                unsigned long long index = rand() % size;
                start = std::chrono::high_resolution_clock::now();
                B1.rank1(index);
                end = std::chrono::high_resolution_clock::now();
            } else if (select0) {
                unsigned long long index = rand() % (size / 2);
                start = std::chrono::high_resolution_clock::now();
                B1.select0(index);
                end = std::chrono::high_resolution_clock::now();
            } else if (select1) {
                unsigned long long index = rand() % (size / 2);
                start = std::chrono::high_resolution_clock::now();
                B1.select1(index);
                end = std::chrono::high_resolution_clock::now();
            }
		
            times.push_back((end - start).count());
        }


        unsigned long long sum = 0;
        for (unsigned long long time : times) sum += time;

        long double mean = (long double) sum / (long double) times.size();

        if (verbose) {
            sort(times.begin(), times.end());
            long double mediana1 = times[249999];
            long double mediana2 = times[499999];
            long double mediana3 = times[749999];
            std::cout << "Ordem do vetor de bits: " << order << "\nTempo médio: " << mean << "\nInterquartil: " << (mediana3 - mediana1) << "\n\n";
            
        } else {
            cout << size << ";" << mean << "\n";
        }
    }
}
