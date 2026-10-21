#include "bitvectorj.hpp"
#include "malloc_count.h"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <cmath>

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

int main(int argc, char *argv[])
{
    if (argc <= 1)
        throw invalid_argument("No benchmark flags were provided, check this folder's README.md for more informations.");

    bool verbose = false;
    bool select0 = false;
    bool select1 = false;
    bool rank0 = false;
    bool rank1 = false;

    string operation;

    long int dummy = 0;

    for (int i = 0; i < argc; i++)
    {
        const char* cur_arg = argv[i];

        if (!strcmp(cur_arg, "-v") || !strcmp(cur_arg, "--verbose"))
            verbose = true;

        if (!strcmp(cur_arg, "-r0") || !strcmp(cur_arg, "--rank0")) {
            rank0 = true;
            operation = "RANK 0";
        } else if (!strcmp(cur_arg, "-r1") || !strcmp(cur_arg, "--rank1")) {
            rank1 = true;
            operation = "RANK 1";
        } else if (!strcmp(cur_arg, "-s0") || !strcmp(cur_arg, "select0")) {
            select0 = true;
            operation = "SELECT 0";
        } else if (!strcmp(cur_arg, "-s1") || !strcmp(cur_arg, "select1")) {
            select1 = true;
            operation = "SELECT 1";
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
        std::cout << "\"Size\";\"Time\";\"Memory\"\n";
    }

    for (unsigned long long order = 3; order < 10; order += 1) {
        srand(time(0));
        unsigned long long size = pow(10, order);

        BitVectorJ B1(size);

        for (int i = 0; i < size; i++)
        {
            if (randomize_bit(i))
                B1.append1();
            else
                B1.append0();
        }

        B1.init();
        
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
                dummy += B1.rank0(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (rank1) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                dummy += B1.rank1(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (select0) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                dummy += B1.select0(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (select1) {
            start = std::chrono::high_resolution_clock::now();
            for (unsigned long long i : idx) {
                dummy += B1.select1(i);
            }
            end = std::chrono::high_resolution_clock::now();
        }
        std::chrono::duration<double, nano> elapsed_time{end - start};
        size_t mem_usage = malloc_count_current();

        if (verbose) { 
            cout << "Dummy: " << dummy << endl;
            cout << "Size: " << size << "\nTime per " << operation << ": " << elapsed_time.count() / (long double) 999999 << " ns" << endl;
            cout << "Memory usage: " << mem_usage << endl;
            cout << endl;
        } else {
            cout << size << ";" << elapsed_time.count() / (long double) 999999 << ";" << mem_usage << endl;
        }
    }
}
