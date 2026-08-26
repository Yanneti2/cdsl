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
    for (long long unsigned size = 1000; size < 10000; size *= 10) {
        bitVector B(64, 2.0);
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
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::nano> ns_time = (end - start);
        unsigned long long index = rand() % B.size();
        start = std::chrono::high_resolution_clock::now();
        R.rank1(index);
        end = std::chrono::high_resolution_clock::now();
        ns_time = end - start;
        cout << size << ";" << ns_time.count() << "\n";
        
        
    }
}
