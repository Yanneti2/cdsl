#include "malloc_count.h"
#include "rrr.hpp"

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <chrono>

using namespace std;

const size_t WSIZE = 1000;
const int REPETITIONS = 10;
const size_t DSIZE = 100000;
const size_t MAX_SIZE = 10000;

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

int main(int argc, char *argv[]) {
    bool rank0 = false;
    bool rank1 = false;

    unsigned dens = 5000;
    
    srand(1791409268);

    cout << "\"Size\";\"Time\";\"RS_Time\";\"RS_Memory\"\n";

    for (int i = 1; i < argc; i++)
    {
        string a = argv[i];
        bool more = i + 1 < argc;

        if ((a == "-d" || a == "--density") && more) dens = atol(argv[++i]);
        else if (a == "--rank0" || a == "-r0") rank0 = true;
        else if (a == "--rank1" || a == "-r1") rank1 = true; 
    }
    if (!rank0 && !rank1) rank1 = true;

    for (int rep = 0; rep < REPETITIONS; rep++)
    {
        for (long long unsigned size = 1; size <= MAX_SIZE; size++)
        {
            BitVector B(size, 0);
            for (size_t i = 0; i < B.size() * (long double) dens / 10000.0; i++) {
                B.set1(i);
            }
            randomize_bitvector(B);

            size_t mem_bf = malloc_count_current();
            auto start = std::chrono::high_resolution_clock::now();
            rrr R(&B);
            auto end = std::chrono::high_resolution_clock::now();
            size_t mem_af = malloc_count_current();

            std::chrono::duration<double, std::nano> build_time = (end - start);
            size_t build_memory = mem_af - mem_bf;

            vector<size_t> warmup(WSIZE);
            vector<size_t> indexes(DSIZE);

            for (size_t j = 0; j < WSIZE; j++)
                warmup[j] = rand() % size; 

            for (unsigned long long i = 0; i < size; i++)
                indexes[i] = rand() % size;

            auto dont_kill_my_methods = 0;

             if (rank0)
             {
                for (size_t i : warmup)
                    dont_kill_my_methods += R.rank0(i);

                start = std::chrono::high_resolution_clock::now();
                for (size_t i : indexes)
                    dont_kill_my_methods += R.rank0(i);
                end = std::chrono::high_resolution_clock::now();
            }
            else if (rank1)
            {
                for (size_t i : warmup)
                    dont_kill_my_methods += R.rank1(i);

                start = std::chrono::high_resolution_clock::now();
                for (size_t i : indexes)
                    dont_kill_my_methods += R.rank1(i);
                end = std::chrono::high_resolution_clock::now();
            }
            cerr << dont_kill_my_methods << endl;

            std::chrono::duration<double, std::nano> ns_time = (end - start);

            cout << size << ";" << ns_time.count() / (long double) size << ";" << build_time.count() << ";" << build_memory << "\n";
        }
    }
}
