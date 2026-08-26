#include "endian.h"
#include <cmath>

#ifndef JACOBSONRANK
#define JACOBSONRANK

class bitVector;

class JacobsonRank {
private:
    unsigned long long layer1_size;
    unsigned long long layer2_size;
    unsigned chunk1_size;
    unsigned chunk2_size;
    unsigned chunk2_per_chunk1;
    unsigned long long *layer1;
    short *layer2;

    unsigned long long *select_vector0;
    unsigned long long *select_vector1;
    unsigned long long select_j;

public:
    JacobsonRank();
    JacobsonRank(bitVector *B);
    ~JacobsonRank();
    unsigned long long rank0(bitVector *B, unsigned long long i);
    unsigned long long rank1(bitVector *B, unsigned long long i);
    void print();

    void build_select0(bitVector *B);
    void build_select1(bitVector *B);
    unsigned long long select0(bitVector *B, unsigned long long i);
    unsigned long long select1(bitVector *B, unsigned long long i);
};

#endif
