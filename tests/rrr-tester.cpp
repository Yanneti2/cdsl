#include "malloc_count.h"
#include "rrr.hpp"

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <chrono>

using namespace std;

int main(void) {
    srand(1791409268);
    cout << "\"Size\";\"Time\";\"RS_Time\";\"RS_Memory\"\n";
    for (long long unsigned size = 1; size < 10000; size++) {
        
        BitVector B(size, 0);

        for (TYPE i = 0; i < size; i++) {
            if (rand() % 2 == 0) {
                B.append1();
            } else {
                B.append0();
            }
        }
        size_t mem_bf = malloc_count_current();
        auto start = std::chrono::high_resolution_clock::now();
        rrr R(&B);
        auto end = std::chrono::high_resolution_clock::now();
        size_t mem_af = malloc_count_current();

        std::chrono::duration<double, std::nano> build_time = (end - start);
        size_t build_memory = mem_af - mem_bf;

        unsigned long long indexes[size] = {0};
        for (unsigned long long i = 0; i < size; i++)
            indexes[i] = rand() % size;

        auto dont_kill_my_methods = 0;

        start = std::chrono::high_resolution_clock::now();
        for (unsigned long long idx : indexes)
            dont_kill_my_methods += R.rank1(idx);
        end = std::chrono::high_resolution_clock::now();
        
        cerr << dont_kill_my_methods << endl;

        std::chrono::duration<double, std::nano> ns_time = (end - start);

        cout << size << ";" << ns_time.count() / (long double) size << ";" << build_time.count() << ";" << build_memory << "\n";
    }
}
