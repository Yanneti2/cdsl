#include "bitvector.hpp"
#include "uvector.hpp"

#include <vector>
#include <cmath>

using namespace std;

#define ULL unsigned long long

#ifndef RAMANRAO
#define RAMANRAO

class rrr {
private:
    ULL superblock_size;    // ((log n)**2)
    ULL *partial_rankc;     // absolute cumulative rank at the start of each superblock
    ULL total_superblocks; 
    ULL *superblock_ptr;

    uint16_t blocks_per_superblock;

    UVector block_ranks;    // rank at the start of each chunk, relative to its superblock
    UVector block_class;    // block class aka num of 1's until i'th index

    BitVector offsets;         // exact unique bit pattern (permutation) of a block among all possible combinations in its class
    vector<ULL> offset_width; 
    UVector offsets_size_ptr;  // make it possible to know where the i'th block offset starts given offsets variable width

    vector<vector<TYPE>> lookuptable;

public:
    ULL n;                   // Bitvector->size()
    ULL total_ones;
    ULL total_blocks;       // blocks_per_superblock * total_superblocks  
    uint16_t block_size;    // (1/2 log n)

    rrr(BitVector *B, bool fixSizeToWordSize = false);

    ULL rank0(ULL i);
    ULL rank1(ULL i);

    void build_select(BitVector *B);
    ULL select0(BitVector *B, ULL i);
    ULL select1(BitVector *B, ULL i);

    uint32_t decode_block(ULL j) const;

    void print();

    ~rrr();
};

#endif
