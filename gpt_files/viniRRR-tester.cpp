#include <iostream>
#include <stdlib.h>
#include <vector>
#include <random>
#include "chrono"
#include "bitvectorRRR.h"
#include "math.h"

using namespace std;

int main(){
    const int opTimes = 100000;
    srand(time(0));
    cout << "\"Size\";\"Time\"\n";
    std::chrono::time_point<std::chrono::system_clock> start, end;
    for(unsigned long long size = 1 << 4; size < 1 << 20 + 1; size += 16){
        // cout << "RRR has order " << log10(size) << "\n";
        auto start = std::chrono::steady_clock::now();
        bitvectorRRR* B = new bitvectorRRR(size);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, nano> elapsed_time = {end - start};
        // cout << "constructor " << elapsed_time.count() << "ns\n";
        
        //randomizing vector
        for(auto i = 0; i < size; i++){
            if(rand() % 2 ==0){
                B->set1(i);
            }
            else{
                B->set0(i);
            }
        }

        std::vector<uint64_t> pos(opTimes);
        for(auto &i : pos) i = rand() % size;
        B->preprocess();
        
        //warmup
        uint64_t sink = 0;
        for (size_t i = 0; i < pos.size()/10; i++) sink += B->rank1(pos[i]);
        
        std::vector<double> results(opTimes, 0);

        for(auto i = 0; i < opTimes; i++){
            auto start = std::chrono::system_clock::now();
            B->rank1(pos[i]);
            auto end = std::chrono::system_clock::now();
            elapsed_time = {end-start};
            results[i] = elapsed_time.count();
        }
        for(auto i: results)
        cout << size << ";" << i << "\n";
    }
}