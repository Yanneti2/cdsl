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

static mt19937_64 rng(42);

static void fill_random(BitVector &B, unsigned dens)
{
    const size_t n = B.size();
    const size_t ones = (size_t)((long double)n * dens / 10000.0L);

    for (size_t i = 0; i < n; i++)
        if (i < ones) B.set1(i); else B.set0(i);

    for (size_t i = n; i > 1; i--)
    {
        size_t j = rng() % i;
        int a = B[i - 1], b = B[j];
        if (b) B.set1(i - 1); else B.set0(i - 1);
        if (a) B.set1(j); else B.set0(j);
    }
}

int main(int argc, char *argv[])
{
    bool verbose = false, check = false;
    bool rank0 = false, rank1 = false, select0 = false, select1 = false;
    string operation;
    unsigned dens = 5000;
    size_t step = 16;
    int max_exp = 18;
    size_t Q = 100000;

    for (int i = 1; i < argc; i++)
    {
        string a = argv[i];
        bool more = i + 1 < argc;
        if (a == "-v" || a == "--verbose") verbose = true;
        else if (a == "--check") check = true;
        else if ((a == "-d" || a == "--density") && more) dens = atol(argv[++i]);
        else if (a == "--step" && more) step = atoll(argv[++i]);
        else if (a == "--max" && more) max_exp = atoi(argv[++i]);
        else if (a == "--queries" && more) Q = atoll(argv[++i]);
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

    for (size_t size = step; size <= ((size_t)1 << max_exp); size += step) {
        BitVector B(size, 0);
        fill_random(B, dens);

        size_t mem0 = malloc_count_current();
        auto rs_start = Clock::now();
        RRR R(&B);
        auto rs_end = Clock::now();
        size_t mem1 = malloc_count_current();

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
        const double rs_time = chrono::duration<double, nano>(rs_end - rs_start).count();
        const size_t rs_mem = mem1 - mem0;

        const unsigned long long ones = B.naive_rank1(size);
        const unsigned long long zeros = size - ones;
        unsigned long long upper;
        unsigned long long lo;
        if (select1)      { lo = 1; upper = ones; }
        else if (select0) { lo = 1; upper = zeros; }
        else              { lo = 0; upper = size; } 

        double time_ns = -1;
        if (upper > 0) {
            vector<size_t> warmup(Q / 10 + 1), idx(Q);
            for (auto &x : warmup) x = lo + rng() % upper;
            for (auto &x : idx) x = lo + rng() % upper;

            auto run = [&](auto &&op) {
                unsigned long long acc = 0;
                for (size_t i : warmup) acc += op(i);
                auto s = Clock::now();
                for (size_t i : idx) acc += op(i);
                auto e = Clock::now();
                sink += acc;
                time_ns = chrono::duration<double, nano>(e - s).count() / (double)Q;
            };

            if (rank0)        run([&](size_t i) { return R.rank0(i); });
            else if (rank1)   run([&](size_t i) { return R.rank1(i); });
            else if (select0) run([&](size_t i) { return R.select0(&B, i); });
            else              run([&](size_t i) { return R.select1(&B, i); });
        }

        if (check) {
            for (int t = 0; t < 2000; t++) {
                size_t i = rng() % size;
                if (R.rank1(i) != B.naive_rank1(i)) { mismatches++; cerr << "rank1 mismatch n=" << size << " i=" << i << "\n"; }
                if (R.rank0(i) != B.naive_rank0(i)) { mismatches++; cerr << "rank0 mismatch n=" << size << " i=" << i << "\n"; }
                if (ones)  { size_t k = 1 + rng() % ones;  if (R.select1(&B, k) != B.naive_select1(k)) { mismatches++; cerr << "select1 mismatch n=" << size << " k=" << k << "\n"; } }
                if (zeros) { size_t k = 1 + rng() % zeros; if (R.select0(&B, k) != B.naive_select0(k)) { mismatches++; cerr << "select0 mismatch n=" << size << " k=" << k << "\n"; } }
            }
        }

        if (verbose) {
            cout << "Size: " << size << "\nTime per " << operation << ": " << time_ns << " ns\n"
                 << "Rank structure: " << rs_time << " ns, " << rs_mem << " bytes\n"
                 << "Select structure: " << ss_time << " ns, " << ss_mem << " bytes\n\n";
        } else {
            cout << size << ";" << time_ns << ";" << rs_time << ";" << rs_mem
                 << ";" << ss_mem << ";" << ss_time << "\n";
        }
    }

    cerr << "sink=" << sink << "\n";
    if (check) cerr << (mismatches ? "CHECK FAILED: " : "CHECK OK: ") << mismatches << " mismatches\n";
    return mismatches ? 1 : 0;
}