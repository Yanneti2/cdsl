#include "../include/RRR.h" 
#include <iostream>
#include <iomanip>
#include <chrono>
#include <math.h>
#include <cstdlib>
#include <assert.h>

int main(void) {
    srand(time(0));
    cout << "\"Size\";\"Time\"\n";
    long double ns_time = 0;
    for (unsigned long long size = 1000; size < 10000000000; size *= 10) {
        bitVector B = bitVector();
        int order = log10(size);
        // cout << "Order: " << order << endl;

        for (TYPE i = 0; i < size; i++) {
            if (rand() % 2 == 0) {
                B.append1();
            } else {
                B.append0();
            }
        }
        RRR R(&B);
        
        for(auto j = 0; j < 1000000; j++){
            unsigned long long index = rand() % B.size();
            auto start = std::chrono::high_resolution_clock::now();
            R.rank1(index);
            auto end = std::chrono::high_resolution_clock::now();
            ns_time += (end - start).count();
        }
        std::cout << size << ";" << (ns_time/(long double)1000000) << "\n";
    }
}

