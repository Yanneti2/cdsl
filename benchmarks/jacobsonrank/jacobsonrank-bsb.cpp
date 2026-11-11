#include "malloc_count.h"
#include "bitvectorj.hpp"

#include <algorithm>
#include <stdlib.h>
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

const size_t WSIZE = 10000;
const int REPETITIONS = 10;
const size_t DSIZE = 100000;

const unsigned long long SEED = 1791409268;

void randomize_bitvector(BitVector &B) {
    srand(2425710);   
    for (size_t i = B.size(); i > 0; i--) {
        size_t j = rand() % i;
        int aux = B[i - 1];
        if (B[j]) {
            B.set1(i - 1);
        } else {
            B.set0(i - 1);
        }

        if (aux) {
            B.set1(j);
        } else {
            B.set0(j);
        }
    }
    if(B.size() == 128) {
        B.print(0);
    }
}


int main(int argc, char *argv[])
{
    bool verbose = false;
    bool rank0 = false;
    bool rank1 = false;
    bool select0 = false;
    bool select1 = false;

    string operation;

    unsigned dens = 5000;

    srand(SEED);

    for (int i = 0; i < argc; i++) {
        if (std::strcmp(argv[i], "-v") == 0 || std::strcmp(argv[i], "--verbose") == 0)
            verbose = true;
        if (std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--density") == 0)
            dens = std::atol(argv[i + 1]);

        if (std::strcmp(argv[i], "--rank0") == 0 || std::strcmp(argv[i], "-r0") == 0) {
            rank0 = true;
            operation = "RANK 0";
        } else if (std::strcmp(argv[i], "--rank1") == 0 || std::strcmp(argv[i], "-r1") == 0) {
            rank1 = true;
            operation = "RANK 1";
        } else if (std::strcmp(argv[i], "--select0") == 0 || std::strcmp(argv[i], "-s0") == 0) {
            select0 = true;
            operation = "SELECT 0";
        } else if (std::strcmp(argv[i], "--select1") == 0 || std::strcmp(argv[i], "-s1") == 0) {
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
        cout << header;
    } else {
        cout << "\"Size\";\"Time\";\"BV_Memory\";\"RS_Time\";\"RS_Memory\";\"SS_Time\";\"SS_Memory\"\n";
    }
    for (size_t size = 16; size < (1 << 16) + 1; size += 16)
    {
        long double avg_opt = 0;
        size_t avg_bvmem = 0;
        long double avg_rst = 0;
        size_t avg_rsmem = 0;
        long double avg_sst = 0;
        size_t avg_ssmem = 0;
        for (int i = 0; i < REPETITIONS; i++)
        {
            size_t mem_rank_structure_0 = malloc_count_current();
            BitVectorJ B(size, 0);
            for (size_t i = 0; i < B.size() * (long double) dens / (double) WSIZE; i++) {
                B.set1(i);
            }
            randomize_bitvector(B);
            size_t mem_rank_structure_1 = malloc_count_current();

            std::chrono::high_resolution_clock::time_point init_start;
            std::chrono::high_resolution_clock::time_point init_end;
            init_start = std::chrono::high_resolution_clock::now();
            B.init();
            init_end = std::chrono::high_resolution_clock::now();
            size_t mem_rank_structure_2 = malloc_count_current();

            std::chrono::high_resolution_clock::time_point ss_start;
            std::chrono::high_resolution_clock::time_point ss_end;
            ss_start = std::chrono::high_resolution_clock::now();
            #ifdef selectstructure
            if (select0) {
                B.build_select0();
            } else if (select1) {
                B.build_select1();
            }
            #endif
            ss_end = std::chrono::high_resolution_clock::now();
            size_t mem_rank_structure_3 = malloc_count_current();

            unsigned long long upper_bound;
            if (select1) {
                upper_bound = B.rank1(size);
            } else if (select0) {
                upper_bound = B.rank0(size);
            } else {
                upper_bound = size;
            }
            upper_bound++;

            vector<size_t> warmup(WSIZE);
            vector<size_t> idx(DSIZE);

            for (size_t j = 0; j < WSIZE; j++)
                warmup[j] = rand() % upper_bound;

            for (size_t j = 0; j < DSIZE; j++)
                idx[j] = rand() % upper_bound;

            std::chrono::high_resolution_clock::time_point start;
            std::chrono::high_resolution_clock::time_point end;
            size_t dont_kill_my_methods;
            
            if (rank0) {
                for (size_t i : warmup) {
                    dont_kill_my_methods += B.rank0(i);
                }
                start = std::chrono::high_resolution_clock::now();
                for (size_t i : idx) {
                    dont_kill_my_methods += B.rank0(i);
                }
                end = std::chrono::high_resolution_clock::now();
            } else if (rank1) {
                for (size_t i : warmup) {
                    dont_kill_my_methods += B.rank1(i);
                }
                start = std::chrono::high_resolution_clock::now();
                for (size_t i : idx) {
                    dont_kill_my_methods += B.rank1(i);
                }
                end = std::chrono::high_resolution_clock::now();
            } else if (select0) {
                for (size_t i : warmup) {
                    dont_kill_my_methods += B.select0(i);
                }
                start = std::chrono::high_resolution_clock::now();
                for (size_t i : idx) {
                    dont_kill_my_methods += B.select0(i);
                }
                end = std::chrono::high_resolution_clock::now();
            } else if (select1) {
                for (size_t i : warmup) {
                    dont_kill_my_methods += B.select1(i);
                }
                start = std::chrono::high_resolution_clock::now();
                for (size_t i : idx) {
                    dont_kill_my_methods += B.select1(i);
                }
                end = std::chrono::high_resolution_clock::now();
            }

            std::chrono::duration<double, nano> init_elapsed_time{init_end - init_start};
            std::chrono::duration<double, nano> ss_elapsed_time{ss_end - ss_start};
            std::chrono::duration<double, nano> elapsed_time{end - start};

            cerr << dont_kill_my_methods << "\n";

            avg_opt += elapsed_time.count();
            avg_bvmem += mem_rank_structure_1 - mem_rank_structure_0;
            avg_rst += init_elapsed_time.count();
            avg_rsmem += mem_rank_structure_2 - mem_rank_structure_1;
            avg_sst += ss_elapsed_time.count();
            avg_ssmem += mem_rank_structure_3 - mem_rank_structure_2;
        }
        if (verbose) { 
            cout << "Size: " << size << "\nTime per " << operation << ": "
            << avg_opt / (long double)(REPETITIONS*DSIZE) << " ns" << endl
            << "Memory usage: " << (avg_bvmem + avg_rsmem + avg_ssmem) / REPETITIONS
            << endl << endl;
        } else {
            cout << size << ";" << avg_opt / (long double)(REPETITIONS*DSIZE)
            << ";" << avg_bvmem / REPETITIONS << ";" << avg_rst / REPETITIONS
            << ";" << avg_rsmem / REPETITIONS << ";" << avg_sst / REPETITIONS
            << ";" << avg_ssmem / REPETITIONS << endl;
        }
    }
}
