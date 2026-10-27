#include "RRR.hpp"

#include <exception>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <math.h>
#include <cstdlib>
#include <assert.h>
#include "malloc_count.h"

using namespace std;

int main(void) {
    srand(time(0));
    // CSV no formato:
    // tamanho do bitvector;
    // Tempo medio por operacao;
    // Tempo para construir estrutura de select;
    // Memoria usada pela estrutura de select;
    // Tempo de construcao da estrutura de select
    cout << "\"Size\";\"Time\";\"RS_Time\";\"RS_Memory\";\"SS_Memory\";\"SS_Time\"\n";
    long double ns_time = 0;
    for (unsigned long long size = 1000; size < 10000000000; size *= 10) {
        size_t initial_mem_usage = malloc_count_current();


        BitVector B = BitVector();
        int order = log10(size);
        // cout << "Order: " << order << endl;

        for (TYPE i = 0; i < size; i++) {
            if (rand() % 2 == 0) {
                B.append1();
            } else {
                B.append0();
            }
        }

        auto start = std::chrono::high_resolution_clock::now();
        RRR R(&B);
        auto rank_time = (std::chrono::high_resolution_clock::now() - start).count()/(long double)1000000;
        size_t rank_m_usage = malloc_count_current() - initial_mem_usage;


        // TODO: Implementar estrutura de select no RRR
        // initial_mem_usage = malloc_count_current();
        // start = std::chrono::high_resolution_clock::now();
        // R.build_select(&B);
        // auto select_time = (start - std::chrono::high_resolution_clock::now()).count()/(long double)1000000;
        // auto select_m_usage = malloc_count_current() - initial_mem_usage;
        auto select_time = -1;
        auto select_m_usage = -1;


       start = std::chrono::high_resolution_clock::now();
        for(uint64_t j = 0; j < 1000000000000000; j++){
            unsigned long long index = rand() % B.size();
            auto start = std::chrono::high_resolution_clock::now();
            R.rank1(index);
       }
       auto end = std::chrono::high_resolution_clock::now();
       ns_time = (end - start).count();

        std::cout << size << ";";
        std::cout << (ns_time/(long double)1000000) << ";";
        std::cout << rank_time << ";";
        std::cout << rank_m_usage << ";";
        std::cout << select_time << ";";
        std::cout << select_m_usage << "\n";
    }
}

