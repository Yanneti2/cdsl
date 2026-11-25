#include "rrr.hpp"
#include "RRR.hpp" 

#include <assert.h>
#include <iostream>
#include <cstdlib>

using namespace std;

int main(void) {
    for (long long unsigned size = 1; size < 10000; size++) {
        
        BitVector B(size, 0);

        for (TYPE i = 0; i < size; i++) {
            if (rand() % 2 == 0) {
                B.append1();
            } else {
                B.append0();
            }
        }
        RRR R(&B);
        rrr r(&B);

        unsigned long long indexes[size] = {0};
        for (unsigned long long i = 0; i < size; i++)
            indexes[i] = rand() % size;

        for (unsigned long long idx : indexes){
            unsigned long long R1 = R.rank1(idx);
            unsigned long long R0 = R.rank0(idx);
            unsigned long long r1 = r.rank1(idx);
            unsigned long long r0 = r.rank0(idx);
            unsigned long long nr1 = B.naive_rank1(idx);
            unsigned long long nr0 = B.naive_rank0(idx);

            assert(R1 == r1);
            assert(R1 == nr1);
            assert(R0 == r0);
            assert(R0 == nr0);
        }
    }
}