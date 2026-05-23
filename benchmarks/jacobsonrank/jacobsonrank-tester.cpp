#include "bitvector.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>

#ifdef _nbits
#define _CHUNK_SIZE "NBITS & NBITS * NBITS"
#else
#ifdef _nbits512
#define _CHUNK_SIZE "NBITS & 512 * NBITS"
#else
#define _CHUNK_SIZE "LOG(N) & LOG(N) * LOG(N)"
#endif
#endif

using namespace std;

bool randomize_bit(unsigned long i) {
    return rand() % 2 == 0;
}

int main(int argc, char *argv[]) {
    bool verbose = false;
    bool rank0 = false;
    bool rank1 = false;
    bool select0 = false;
    bool select1 = false;
    string operation;

    for (int i = 0; i < argc; i++) {
        if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--verbose") == 0)
            verbose = true;

        if (std::strcmp(argv[i], "-op") == 0 || std::strcmp(argv[i], "--operation") == 0) {
            if (std::strcmp(argv[i + 1], "rank0") == 0 || std::strcmp(argv[i + 1], "r0") == 0) {
                rank0 = true;
                operation = "RANK 0";
            } else if (std::strcmp(argv[i + 1], "rank1") == 0 || std::strcmp(argv[i + 1], "r1") == 0) {
                rank1 = true;
                operation = "RANK 1";
            } else if (std::strcmp(argv[i + 1], "select0") == 0 || std::strcmp(argv[i + 1], "s0") == 0) {
                select0 = true;
                operation = "SELECT 0";
            } else if (std::strcmp(argv[i + 1], "select1") == 0 || std::strcmp(argv[i + 1], "s1") == 0) {
                select1 = true;
                operation = "SELECT 1";
            }
        }


    }

    if (verbose) {
        char header[] = 
            "=======================================================================\n"
            "THE FOLLOWING TESTS UTILIZE CHUNK SIZE: " _CHUNK_SIZE "\n"
            "IN ADDITION, THEY "
            #ifndef selectstructure
            "DO NOT "
            #endif
            "UTILIZE AUXILIARY SELECT STRUCTURE\n"
            "=======================================================================\n\n"
            ;

        std::cout << header;
    } else {
        std::cout << "\"Size\";\"Time\"\n";
    }

    for (unsigned long long order = 3; order < 10; order += 1) {
        srand(time(0));
        unsigned long long size = pow(10, order);
        bitVector B1(size, randomize_bit);

        B1.JacobsonRank_build();

        #ifdef selectstructure
        if (select0) {
            B1.build_select0();
        } else if (select1) {
            B1.build_select1();
        }
        #endif
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        vector<unsigned long long> idx(999999);
        unsigned long long upper_bound = (select0 || select1) ? size / 2 : size;

        for (unsigned long long j = 0; j < 999999; j++) {
            idx[j] = rand() % upper_bound;
        }

        if (rank0) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                B1.rank0(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (rank1) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                B1.rank1(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (select0) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                B1.select0(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (select1) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                B1.select1(i);
            }
            end = std::chrono::high_resolution_clock::now();
        }

        std::chrono::duration<double, nano> elapsed_time{end - start};
        if (verbose) { 
            cout << "Size: " << size << "\nTime per " << operation << ": " << elapsed_time.count() / (long double) 999999 << " ns\n\n";

        } else {
            cout << size << ";" << elapsed_time.count() / (long double) 999999 << "\n";
        }
    }
}
