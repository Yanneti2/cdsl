#include "bitvectorBS.h"
#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include "malloc_count.h"

using namespace std;

void randomize_bitvector(bitvectorBS &B) {
    for (size_t i = B.size(); i > 0; i--) {
        size_t j = rand() % i;
        int aux = B[i - 1];
        if (B[j]) {
            B.set(i - 1);
        } else {
            B.reset(i - 1);
        }

        if (aux) {
            B.set(j);
        } else {
            B.reset(j);
        }
    }
}


int main(int argc, char *argv[]) {
    bool verbose = false;
    bool rank0 = false;
    bool rank1 = false;
    bool select0 = false;
    bool select1 = false;
    string operation;
    unsigned dens = 5000;

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
            // "THE FOLLOWING TESTS UTILIZE CHUNK SIZE: " _CHUNK_SIZE "\n"
            "IN ADDITION, THEY "
            #ifndef selectstructure
            "DO NOT "
            #endif
            "UTILIZE AUXILIARY SELECT STRUCTURE\n"
            "=======================================================================\n\n"
            ;

        cout << header;
    } else {
        cout << "\"Size\";\"Time\";\"BS_Memory\";\"BS_Time\"\n";
    }
    for (int i = 0; i < 10; i++)
    {
        for (size_t size = 16; size < (1 << 13) + 1; size += 16)
        {
            srand(time(0));

            std::chrono::high_resolution_clock::time_point start;
            std::chrono::high_resolution_clock::time_point end;

            size_t mem_rank_structure_0 = malloc_count_current();

            start = std::chrono::high_resolution_clock::now();
            bitvectorBS B(size);
            end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, nano> elapsed_time{end - start};
            double build_time = elapsed_time.count();

            B.reset_all();
            for (size_t i = 0; i < B.size() * (long double) dens / 10000.0; i++) {
                B.set(i);
            }
            randomize_bitvector(B);
            B.preprocess();
            size_t mem_rank_structure_1 = malloc_count_current();

            // std::chrono::high_resolution_clock::time_point init_start;
            // std::chrono::high_resolution_clock::time_point init_end;
            // init_start = std::chrono::high_resolution_clock::now();
            // init_end = std::chrono::high_resolution_clock::now();
            // size_t mem_rank_structure_2 = malloc_count_current();

            // std::chrono::high_resolution_clock::time_point ss_start;
            // std::chrono::high_resolution_clock::time_point ss_end;
            // size_t mem_rank_structure_3 = malloc_count_current();

            unsigned long long upper_bound;
            if (select1) {
                upper_bound = B.rank1(size);
            } else if (select0) {
                upper_bound = B.rank0(size);
            } else {
                upper_bound = size;
            }
            upper_bound++;

            vector<size_t> warmup(100000);
            vector<size_t> idx(100000);

            for (size_t j = 0; j < 10000; j++)
                warmup[j] = rand() % upper_bound;

            for (size_t j = 0; j < 100000; j++)
                idx[j] = rand() % upper_bound;

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
            elapsed_time = end - start;

            cerr << dont_kill_my_methods << "\n";
            if (verbose) { 
                cout << "Size: " << size << "\nTime per " << operation << ": " << elapsed_time.count() / (long double) 100000 << " ns" << endl;
                cout << "Memory usage: " << mem_rank_structure_1 - mem_rank_structure_0 << endl;
                cout << "Construction Time: " << build_time << " ns" << endl;
                cout << endl;
            } else {
                cout << size << ";" << elapsed_time.count() / (long double) 100000 
                            << ";" << mem_rank_structure_1 - mem_rank_structure_0
                            << ";" << build_time
                            << endl;
            }
        }
    }
}