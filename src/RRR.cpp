#include "../include/RRR.h"
#include <iostream>

#ifndef bitMask
#ifdef IS32BIT
#define bitMask(i) _bitMask32[(i)]
#else
#define bitMask(i) _bitMask64[(i)]
#endif
#endif

uint32_t _bitMask32[] = {
    0xFFFFFFFF,0x7FFFFFFF,0x3FFFFFFF,0x1FFFFFFF,
    0x0FFFFFFF,0x07FFFFFF,0x03FFFFFF,0x01FFFFFF,
    0x00FFFFFF,0x007FFFFF,0x003FFFFF,0x001FFFFF,
    0x000FFFFF,0x0007FFFF,0x0003FFFF,0x0001FFFF,
    0x0000FFFF,0x00007FFF,0x00003FFF,0x00001FFF,
    0x00000FFF,0x000007FF,0x000003FF,0x000001FF,
    0x000000FF,0x0000007F,0x0000003F,0x0000001F,
    0x0000000F,0x00000007,0x00000003,0x00000001,
    0x00000000
};

uint64_t _bitMask64[] = {
    0xFFFFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFFF,0x1FFFFFFFFFFFFFFF,
    0x0FFFFFFFFFFFFFFF,0x07FFFFFFFFFFFFFF,0x03FFFFFFFFFFFFFF,0x01FFFFFFFFFFFFFF,
    0x00FFFFFFFFFFFFFF,0x007FFFFFFFFFFFFF,0x003FFFFFFFFFFFFF,0x001FFFFFFFFFFFFF,
    0x000FFFFFFFFFFFFF,0x0007FFFFFFFFFFFF,0x0003FFFFFFFFFFFF,0x0001FFFFFFFFFFFF,
    0x0000FFFFFFFFFFFF,0x00007FFFFFFFFFFF,0x00003FFFFFFFFFFF,0x00001FFFFFFFFFFF,
    0x00000FFFFFFFFFFF,0x000007FFFFFFFFFF,0x000003FFFFFFFFFF,0x000001FFFFFFFFFF,
    0x000000FFFFFFFFFF,0x0000007FFFFFFFFF,0x0000003FFFFFFFFF,0x0000001FFFFFFFFF,
    0x0000000FFFFFFFFF,0x00000007FFFFFFFF,0x00000003FFFFFFFF,0x00000001FFFFFFFF,
    0x00000000FFFFFFFF,0x000000007FFFFFFF,0x000000003FFFFFFF,0x000000001FFFFFFF,
    0x000000000FFFFFFF,0x0000000007FFFFFF,0x0000000003FFFFFF,0x0000000001FFFFFF,
    0x0000000000FFFFFF,0x00000000007FFFFF,0x00000000003FFFFF,0x00000000001FFFFF,
    0x00000000000FFFFF,0x000000000007FFFF,0x000000000003FFFF,0x000000000001FFFF,
    0x000000000000FFFF,0x0000000000007FFF,0x0000000000003FFF,0x0000000000001FFF,
    0x0000000000000FFF,0x00000000000007FF,0x00000000000003FF,0x00000000000001FF,
    0x00000000000000FF,0x000000000000007F,0x000000000000003F,0x000000000000001F,
    0x000000000000000F,0x0000000000000007,0x0000000000000003,0x0000000000000001,
    0x0000000000000000
};

using namespace std;
template <typename T>
unsigned long long binary_search(T *V, T target, unsigned long long beginning, unsigned long long end) {
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        unsigned long long middle = (beginning + end) / 2;
        if (V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}

template <typename T>
unsigned long long binary_search(T *V, T target, unsigned long long beginning, unsigned long long end, unsigned long long factor) {
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        unsigned long long middle = (beginning + end) / 2;
        if (factor * middle - V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}
RRR::RRR(BitVector *B, bool fixSizeToWordSize) {
    block_size = ceil(log2(B->size())) / 2 * floor(log2(B->size()));
    total_blocks = (B->size() + block_size - 1) / block_size;
    partial_ranks = (unsigned long long *) malloc(total_blocks * sizeof(unsigned long long));

    K_size = ceil(log2(B->size())) / 2;
    K_per_block = block_size / K_size;
    total_K = (B->size() + K_size - 1) / K_size;
    this->K = (uint16_t *) calloc(total_K, sizeof(uint16_t));
    offsets = (uint16_t *) malloc(total_K * sizeof(uint16_t));

    vector<TYPE> next(K_size + 1, 0);
    vector<TYPE> pattern_to_class(1 << (K_size), 0);
    for (TYPE i = 0; i < (1 << K_size); i++) {
        unsigned long long popcount = __popcount(i);
        if (popcount <= lookuptable.size()) {
            lookuptable.push_back(vector<TYPE>());
        }

        lookuptable[popcount].push_back(i);
        pattern_to_class[i] = next[__popcount(i)];
        next[__popcount(i)] += 1;
    }

    unsigned long long block_counter = 0;
    for (size_t i = 0; i < total_K; i++) {
        uint16_t K_counter = __popcount(B->accessWord(i, K_size));
        this->K[i] = K_counter;
        if (i % K_per_block == 0) {
            this->partial_ranks[i / K_per_block] = block_counter;
        }
        block_counter += K_counter;
        offsets[i] = pattern_to_class[B->accessWord(i, K_size) >> (NBITS - K_size)];
    }

}

RRR::~RRR() {
    free(offsets);
    free(K);
    free(partial_ranks);
}

unsigned long long RRR::rank1(unsigned long long i) {
    unsigned long long K_pos = i / K_size;
    unsigned long long block = i / block_size;
    unsigned long long first_K = block * K_per_block;
    unsigned long long rank = partial_ranks[block];
    for (unsigned long long i = first_K; i < K_pos; i++) rank += K[i];
    rank += __popcount(lookuptable[K[K_pos]][offsets[K_pos]] >> (K_size - i % K_size));

    return rank;
}

unsigned long long RRR::rank0(unsigned long long i) {
    return i - rank1(i);
}
unsigned long long RRR::select1(BitVector *B, unsigned long long i) {
    if (i > rank1(B->size())) return -1;
    if (i == 0) return 0;
    const unsigned long long pos = binary_search(partial_ranks, i, 0, (B->size() + block_size - 1) / block_size);
    unsigned long long select = pos * block_size;
    long long target = i - partial_ranks[pos];
    unsigned long long j;
    for (j = K_per_block * pos; j < K_per_block * (pos + 1); j++) {
        if (target - K[j] <= 0) break;
        select += K_size;
        target -= K[j];
    }
    while (target != 0)
        target -= (*B)[select++];
    return select;
}
unsigned long long RRR::select0(BitVector *B, unsigned long long i) {
    if (i > rank0(B->size())) return -1;
    if (i == 0) return 0;
    const unsigned long long pos = binary_search(partial_ranks, i, 0, (B->size() + block_size - 1) / block_size, block_size);
    unsigned long long select = pos * block_size;
    long long target = i - select + partial_ranks[pos];
    unsigned long long j;
    for (j = K_per_block * pos; j < K_per_block * (pos + 1); j++) {
        if (target - K_size + K[j] <= 0) break;
        select += K_size;
        target += K[j] - K_size ;
    }
    while (target != 0)
        target -= 1 ^ (*B)[select++];
    return select;
}

void RRR::print() {
    cout << "BSIZE: " << block_size << "\nTOTAL BLOCKS: " << total_blocks << "\nK SIZE: " << K_size << endl;
    cout << "K: ";
    for (unsigned long long i = 0; i < total_K; i++) {
        cout << K[i] << " ";
    }
    std::cout << endl;
    cout << "PARTIAL RANKS: ";
    for (unsigned long long i = 0; i < total_blocks; i++) {
        cout << partial_ranks[i] << " ";
    }
    cout << endl;
    cout << "LOOKUPTABLE: ";
    for (int i = 0; i < lookuptable.size(); i++) {
        cout << endl << i << ": ";
        for (TYPE j : lookuptable[i])
            cout << j << " ";
    }
    cout << endl;
    unsigned long long total_blocks;
    unsigned long long *partial_ranks;
    unsigned long long total_K;
    uint16_t K_size;
    uint16_t *K;
    uint16_t K_per_block;
}

#undef bitMask
