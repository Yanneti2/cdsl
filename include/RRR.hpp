#include "bitvector.hpp"

#include <vector>
#include <cmath>

#ifndef RRRANK
#define RRRANK

class RRR {
private:
    unsigned long long block_size;
    unsigned long long total_blocks;
    unsigned long long *partial_ranks;
    unsigned long long total_K;
    uint16_t K_size;
    uint16_t *K;
    uint16_t K_per_block;

    uint16_t *offsets;
    std::vector<std::vector<TYPE>> lookuptable;

public:
    RRR(BitVector *B, bool fixSizeToWordSize = false);
    ~RRR();
    unsigned long long rank0(unsigned long long i);
    unsigned long long rank1(unsigned long long i);
    void print();

    void build_select(BitVector *B);
    unsigned long long select0(BitVector *B, unsigned long long i);
    unsigned long long select1(BitVector *B, unsigned long long i);
};

#endif
