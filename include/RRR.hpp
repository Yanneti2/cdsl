#include "bitvector.hpp"

#include <vector>
#include <cmath>

#ifndef RRRANK
#define RRRANK

struct RRRtable {
    std::vector<std::vector<TYPE>> lookuptable;
    std::vector<TYPE> pattern_to_class;

    explicit RRRtable(uint16_t K_size){
        // lookuptable[c][o] = o-th pattern of K_size bits with popcount c (RRR class/offset decoding).
        pattern_to_class.assign(1ULL << K_size, 0);
        lookuptable.assign(K_size + 1, std::vector<TYPE>());
        for (TYPE p = 0; p < (1ULL << K_size); p++) {
            unsigned c = std::__popcount(p);
            pattern_to_class[p] = lookuptable[c].size();
            lookuptable[c].push_back(p);
        }
    }
};

class RRR {
private:
    unsigned long long block_size;
    unsigned long long total_blocks;
    unsigned long long *partial_ranks;  // absolute rank at the start of each superblock
    uint16_t *block_ranks;              // rank at the start of each chunk, relative to its superblock
    unsigned long long total_K;
    uint16_t K_size;
    uint16_t *K;
    uint16_t K_per_block;

    uint16_t *offsets;
    const RRRtable* table;

public:
    static uint16_t chunk_size(BitVector* B) {
        unsigned long long n = B->size();    
        unsigned long long lg = n > 1 ? (unsigned long long) ceil(log2(n)) : 1;
        return lg / 2 > 0 ? lg / 2 : 1;
    }

    RRR(BitVector *B, const RRRtable *table);
    ~RRR();
    unsigned long long rank0(unsigned long long i);
    unsigned long long rank1(unsigned long long i);
    void print();

    void build_select(BitVector *B);
    unsigned long long select0(BitVector *B, unsigned long long i);
    unsigned long long select1(BitVector *B, unsigned long long i);
};

#endif
