#include "malloc_count.h"
#include "RRR.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <random>
#include <string>
#include <vector>

using namespace std;

using Clock = chrono::high_resolution_clock;

const size_t WSIZE = 1000;
const size_t DSIZE = 100000;
const int REPETITIONS = 10;

const unsigned long long SEED = 1791409268;

void randomize_bitvector(BitVector &B) {
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
}


int main(int argc, char *argv[])
{
    bool rank0 = false;
    bool rank1 = false;
    bool select0 = false;
    bool select1 = false;

    unsigned dens = 5000;
    bool verbose = false;

    string operation;

    srand(SEED);

    for (int i = 1; i < argc; i++)
    {
        string a = argv[i];
        bool more = i + 1 < argc;

        if (a == "-v" || a == "--verbose") verbose = true;
        else if ((a == "-d" || a == "--density") && more) dens = atol(argv[++i]);
        else if (a == "--rank0" || a == "-r0") { rank0 = true; operation = "RANK 0"; }
        else if (a == "--rank1" || a == "-r1") { rank1 = true; operation = "RANK 1"; }
        else if (a == "--select0" || a == "-s0") { select0 = true; operation = "SELECT 0"; }
        else if (a == "--select1" || a == "-s1") { select1 = true; operation = "SELECT 1"; }
    }
    if (!rank0 && !rank1 && !select0 && !select1) { rank1 = true; operation = "RANK 1"; }

    if (verbose) {
        cout << "=======================================================================\n"
             << "OPERATION: " << operation << "   DENSITY: " << dens / 100.0 << "%\n"
             << "THE SELECT STRUCTURE IS "
            #ifndef selectstructure
             << "NOT "
            #endif
             << "IMPLEMENTED/USED\n"
             << "=======================================================================\n\n";
    } else {
        cout << "\"Size\";\"Time\";\"RS_Time\";\"RS_Memory\";\"SS_Memory\";\"SS_Time\"\n";
    }

    volatile unsigned long long sink = 0;
    int mismatches = 0;

    for (size_t size = 16; size <= (1ULL << 18); size += 16)
    {
        long double avgtime = 0;
        size_t avg_rs_mem = 0;
        size_t avg_ss_mem = 0;
        long double avg_rs_time = 0;
        long double avg_ss_time = 0;

        for (int rep = 0; rep < REPETITIONS; rep++)
        {
            BitVector B(size, 0);
            for (size_t i = 0; i < B.size() * (long double) dens / 10000.0; i++) {
                B.set1(i);
            }
            randomize_bitvector(B);

            size_t mem0 = malloc_count_current();
            auto rs_start = Clock::now();
            RRR R(&B);
            auto rs_end = Clock::now();
            size_t mem1 = malloc_count_current();

            avg_rs_time += chrono::duration<double, nano>(
                rs_end - rs_start
            ).count();

            avg_rs_mem += mem1 - mem0;

            long long ss_time = -1, ss_mem = -1;
            #ifdef selectstructure
            {
                auto ss_start = Clock::now();
                R.build_select(&B);
                auto ss_end = Clock::now();
                size_t mem2 = malloc_count_current();
                ss_time = (long long)chrono::duration<double, nano>(ss_end - ss_start).count();
                ss_mem = (long long)(mem2 - mem1);
            }
            #endif

            const unsigned long long ones = B.naive_rank1(size);
            const unsigned long long zeros = size - ones;
            unsigned long long upper_bound;

            if (select1)
                upper_bound = ones;
            else if (select0)
                upper_bound = zeros;
            else
                upper_bound = size;
            upper_bound++;

            unsigned long long lower_bound = (select0 || select1) ? 1 : 0;

            vector<size_t> warmup(WSIZE);
            vector<size_t> idx(DSIZE);

            for (size_t j = 0; j < WSIZE; j++)
                warmup[j] = lower_bound + rand() % upper_bound;

            for (size_t j = 0; j < DSIZE; j++)
                idx[j] = lower_bound + rand() % upper_bound;

            unsigned long long dont_kill_my_methods = 0;

            if (rank0) {
                for (size_t i : warmup)
                    dont_kill_my_methods += R.rank0(i);

                auto start = Clock::now();
                for (size_t i : idx)
                    dont_kill_my_methods += R.rank0(i);
                auto end = Clock::now();

                avgtime += chrono::duration<double, nano>(end - start).count();
            }
            else if (rank1) {
                for (size_t i : warmup)
                    dont_kill_my_methods += R.rank1(i);

                auto start = Clock::now();
                for (size_t i : idx)
                    dont_kill_my_methods += R.rank1(i);
                auto end = Clock::now();

                avgtime += chrono::duration<double, nano>(end - start).count();
            }
            else if (select0) {
                for (size_t i : warmup)
                    dont_kill_my_methods += R.select0(&B, i);

                auto start = Clock::now();
                for (size_t i : idx)
                    dont_kill_my_methods += R.select0(&B, i);
                auto end = Clock::now();

                avgtime += chrono::duration<double, nano>(end - start).count();
            }
            else if (select1) {
                for (size_t i : warmup)
                    dont_kill_my_methods += R.select1(&B, i);

                auto start = Clock::now();
                for (size_t i : idx)
                    dont_kill_my_methods += R.select1(&B, i);
                auto end = Clock::now();

                avgtime += chrono::duration<double, nano>(end - start).count();
            }
            cerr << dont_kill_my_methods;
        }
        if (verbose) {
            cout << "Size: " << size << "\nTime per " << operation << ": " << avgtime << " ns\n"
                 << "Rank structure: " << avg_rs_time << " ns, " << avg_rs_mem << " bytes\n"
                 << "Select structure: " << avg_ss_time << " ns, " << avg_ss_mem << " bytes\n\n";
        } else {
            cout << size << ";" << avgtime << ";" << avg_rs_time << ";" << avg_rs_mem
                 << ";" << avg_ss_mem << ";" << avg_ss_time << "\n";
        }
    }
}