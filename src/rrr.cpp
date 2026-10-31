#include "rrr.hpp"

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
ULL binary_search(T *V, T target, ULL beginning, ULL end) {
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        ULL middle = (beginning + end) / 2;
        if (V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}

template <typename T>
ULL binary_search(T *V, T target, ULL beginning, ULL end, ULL factor) {
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        ULL middle = (beginning + end) / 2;
        if (factor * middle - V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}
// iterates at most block_size bits, if set a ceil => const O(1)
uint64_t read_bits(const BitVector& s, size_t start, unsigned width) {
    if (width == 0) return 0;
    size_t w = start / 16;                    
    unsigned off = start % 16;                
    uint32_t two = ((uint32_t) s.accessWord(w, 16) << 16) | (uint32_t) s.accessWord(w + 1, 16);
    return (two >> (32 - off - width)) & ((1u << width) - 1);
}

uint32_t rrr::decode_block(ULL j) const {
    ULL sb = j / blocks_per_superblock;
    uint32_t k = block_class[j];
    ULL start = superblock_ptr[sb] + offsets_size_ptr[j];
    uint32_t off = read_bits(offsets, start, offset_width[k]);
    return lookuptable[k][off];
}

rrr::rrr(BitVector *B, bool fixSizeToWordSize)
{
    n = B->size();
    unsigned logn = ceil(log2(max<ULL>(n,2)));

    // min(15u, logn / 2) for > 2**30
    block_size = max(1u, logn / 2); // b
    superblock_size = max(1u, (unsigned)(logn / 2 * floor(log2(n))));
    
    blocks_per_superblock = superblock_size / block_size; // L
    total_blocks = (n + block_size - 1) / block_size;
    total_superblocks = (n + superblock_size - 1) / superblock_size;

    partial_rankc = (ULL *) malloc(total_superblocks * sizeof(ULL));
    superblock_ptr = (ULL *) malloc(total_superblocks * sizeof(ULL));

    vector<TYPE> next(block_size + 1, 0);
    vector<TYPE> pattern_to_class(1 << (block_size), 0);

    lookuptable.assign(block_size + 1, vector<TYPE>());
    for (TYPE i = 0; i < (1 << block_size); i++)
    {
        ULL popcount = __popcount(i);
        lookuptable[popcount].push_back(i);
        pattern_to_class[i] = next[__popcount(i)];
        next[__popcount(i)] += 1;
    }
    offset_width.assign(block_size + 1, 0);
    for (unsigned k = 0; k <= block_size; k++)
        offset_width[k] = ceil(log2(lookuptable[k].size()));

    unsigned class_bits = ceil(log2(block_size + 1));
    unsigned rel_bits = ceil(log2(blocks_per_superblock * block_size + 1));
    block_class = UVector(class_bits, total_blocks);
    block_ranks = UVector(rel_bits, total_blocks);
    offsets_size_ptr = UVector(rel_bits, total_blocks);

    ULL block_counter = 0;
    for (size_t i = 0; i < total_blocks; i++)
    {
        size_t cur_sb = i / blocks_per_superblock;

        uint32_t chunk = B->accessWord(i, block_size);
        if (i == total_blocks - 1)
        {
            unsigned pad = total_blocks * block_size - n;
            chunk &= ~((1u << pad) - 1);
        }
        uint16_t K_counter = __popcount(chunk);
        block_class.set(i,K_counter);

        if (i % blocks_per_superblock == 0){
            partial_rankc[cur_sb] = block_counter;
            superblock_ptr[cur_sb] = offsets.size();
        }
        block_ranks.set(i, block_counter - partial_rankc[cur_sb]);
        offsets_size_ptr.set(i, offsets.size() - superblock_ptr[cur_sb]); 
        block_counter += K_counter;

        if (offset_width[K_counter] > 0)
            offsets.append(pattern_to_class[chunk], offset_width[K_counter]);
    }
    offsets.append(0, 16);
    offsets.append(0, 16);
    total_ones = block_counter;
}

rrr::~rrr() {
    free(superblock_ptr);
    free(partial_rankc); 
}

ULL rrr::rank1(ULL i) {
    if (i >= this->n) return total_ones;
    ULL block = i / block_size;
    unsigned pos_in_block = i % block_size;
    ULL cur_sb = block/blocks_per_superblock;
    uint32_t pattern = decode_block(block);
    return partial_rankc[cur_sb] + block_ranks[block] + __popcount(pattern >> (block_size - pos_in_block));
}

ULL rrr::rank0(ULL i) {
    if (i > this->n) i = this->n;
    return i - rank1(i);
}
ULL rrr::select1(BitVector *B, ULL i)
{
    if (i > total_ones) return -1;
    if (i == 0) return 0;

    const ULL pos = binary_search(partial_rankc, i, 0, total_superblocks);
    ULL select = pos * superblock_size;
    long long target = i - partial_rankc[pos];
    ULL j;
    for (j = blocks_per_superblock * pos; j < blocks_per_superblock * (pos + 1); j++) {
        if (target - block_class[j] <= 0) break;
        select += block_size;
        target -= block_class[j];
    }
    // uint32_t pattern = decode_block(j);
    // for (unsigned p = 0; target != 0; p++) {
    //     target -= (pattern >> (block_size - 1 - p)) & 1;
    //     select++;
    // }
    while (target != 0)
        target -= (*B)[select++];
    return select;
}

ULL rrr::select0(BitVector *B, ULL i) {
    if (i > this->n - total_ones) return -1;
    if (i == 0) return 0;

    const ULL pos = binary_search(partial_rankc, i, 0, total_superblocks, superblock_size);
    ULL select = pos * superblock_size;
    long long target = i - select + partial_rankc[pos];
    ULL j;
    for (j = blocks_per_superblock * pos; j < blocks_per_superblock * (pos + 1); j++) {
        if (target - block_size + block_class[j] <= 0) break;
        select += block_size;
        target += block_class[j] - block_size ;
    }
    // uint32_t pattern = decode_block(j);
    // for (unsigned p = 0; target != 0; p++) {
    //     target -= 1 ^ ((pattern >> (block_size - 1 - p)) & 1);
    //     select++;
    // }
    while (target != 0)
        target -= 1 ^ (*B)[select++];
    return select;
}

void rrr::print() {
    cout << "BSIZE: " << superblock_size << "\nTOTAL BLOCKS: " << total_superblocks << "\nK SIZE: " << block_size << endl;
    cout << "block_class: ";
    for (ULL i = 0; i < total_blocks; i++) {
        cout << block_class[i] << " ";
    }
    std::cout << endl;
    cout << "PARTIAL RANKS: ";
    for (ULL i = 0; i < total_superblocks; i++) {
        cout << partial_rankc[i] << " ";
    }
    cout << endl;
    cout << "LOOKUPTABLE: ";
    for (int i = 0; i < lookuptable.size(); i++) {
        cout << endl << i << ": ";
        for (TYPE j : lookuptable[i])
            cout << j << " ";
    }
    cout << endl;
    ULL total_superblocks;
    ULL *partial_rankc;
    ULL total_blocks;
    uint16_t block_size;
    uint16_t *block_class;
    uint16_t blocks_per_superblock;
}

#undef bitMask