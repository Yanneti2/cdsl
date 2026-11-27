#include "interquartil-stats.hpp"
#include "malloc_count.h"
#include "RRR.hpp"

#include <optional>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <random>
#include <string>
#include <vector>
#include <map>

using namespace std;

const size_t WSIZE = 1000;
const int REPETITIONS = 10;
const size_t DSIZE = 100000;
const size_t MAX_SIZE = 10000;

const unsigned long long SEED = 1791409268;

map<uint16_t, RRRtable> tables;

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
    bool c_time = false;
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

        if (a == "-ct" || a == "--ctime") c_time = true;
        else if (a == "-v" || a == "--verbose") verbose = true;
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
        // cout << "\"Size\";\"Time\";\"RS_Time\";\"RS_Memory\";\"SS_Memory\";\"SS_Time\"\n";
        cout << "\"Size\";\"Time\";\"RS_Time\";\"RS_Memory\"\n";
    }
    for (int rep = 0; rep < REPETITIONS; rep++)
    {
        for (size_t size = 64; size <= MAX_SIZE; size*=2)
        {
            BitVector* B = new BitVector(size, 0);
            for (size_t i = 0; i < B->size() * (long double) dens / 10000.0; i++) {
                B->set1(i);
            }
            randomize_bitvector(*B);

            std::chrono::high_resolution_clock::time_point start;
            std::chrono::high_resolution_clock::time_point end;

            struct timespec begin,stop;
            struct timespec total = {0,0};
            long double rs_time;

            uint16_t cur_k = RRR::chunk_size(B);
            auto t = tables.find(cur_k);
            if (t == tables.end())
                t = tables.emplace(cur_k, RRRtable(cur_k)).first;
            const RRRtable& cur_t = t->second;

            // RRRtable* table = new RRRtable(RRR::chunk_size(B));

            // optional<RRR> opt;
            // if (c_time) {
            //     clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
            //     opt.emplace(&B);
            //     clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
            //     add_time(begin,stop,&total);
            //     rs_time = get_time(total);
            // } else {
            // opt.emplace(&B);

            size_t mem0 = malloc_count_current();
            start = std::chrono::high_resolution_clock::now();
            RRR* R = new RRR(B, &cur_t);
            end = std::chrono::high_resolution_clock::now();
            size_t mem1 = malloc_count_current();

            // }
            // RRR& R = *opt;
            // if (!c_time) 

            rs_time = chrono::duration<double, nano>(end - start).count();

            size_t rs_mem = mem1 - mem0;

            vector<size_t> warmup(WSIZE);
            vector<size_t> idx(DSIZE);

            for (size_t j = 0; j < WSIZE; j++)
                warmup[j] = rand() % size; // lower bound + rand % upper_bound

            for (size_t j = 0; j < DSIZE; j++)
                idx[j] =  rand() % size;

            unsigned long long dont_kill_my_methods = 0;
            double time = 0;

            if (rank0) {
                for (size_t i : warmup)
                    dont_kill_my_methods += R->rank0(i);
                if (c_time){
                    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
                    for (size_t i : idx)
                        dont_kill_my_methods += R->rank0(i);
                    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
                    add_time(begin,stop,&total);
                    time += get_time(total);
                } else {
                    start = std::chrono::high_resolution_clock::now();
                    for (size_t i : idx)
                        dont_kill_my_methods += R->rank0(i);
                    end = std::chrono::high_resolution_clock::now();
                    time += chrono::duration<double, nano>(end - start).count();
                }
            }
            else if (rank1) {
                for (size_t i : warmup)
                    dont_kill_my_methods += R->rank1(i);
                if (c_time){
                    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &begin);
                    for (size_t i : idx)
                        dont_kill_my_methods += R->rank1(i);
                    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
                    add_time(begin,stop,&total);
                    time += get_time(total);
                } else {
                    start = std::chrono::high_resolution_clock::now();
                    for (size_t i : idx)
                        dont_kill_my_methods += R->rank1(i);
                    end = std::chrono::high_resolution_clock::now();
                    time += chrono::duration<double, nano>(end - start).count();
                }
            }
            cerr << dont_kill_my_methods << endl;

            if (verbose) {
            cout << "Size: " << size << "\nTime per " << operation << ": " << time  / DSIZE<< " ns\n"
                 << "Rank structure: " << rs_time << " ns, " << rs_mem << " bytes\n";
            } else {
                cout << size << ";" << time / DSIZE<< ";" << rs_time << ";" << rs_mem << "\n";
            }
            delete R;
            delete B;
        }
    }
}