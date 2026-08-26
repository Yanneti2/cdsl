#include "bitvectorj.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

#ifndef bitMask
#ifdef IS32BIT
#define bitMask(i) bitMask32[(i)]
#else
#define bitMask(i) bitMask64[(i)]
#endif
#endif

#define MIN(A, B) ((A) < (B) ? (A) : (B)) 

extern uint32_t bitMask32[];
extern uint64_t bitMask64[];

template <typename T>
size_t binary_search(T *V, T target, size_t beginning, size_t end) {
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        size_t middle = (beginning + end) / 2;
        if (V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}

template <typename T>
size_t binary_search(T *V, T target, size_t beginning, size_t end, size_t factor) {
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        size_t middle = (beginning + end) / 2;
        if (factor * middle - V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}

template <typename T>
size_t binary_search(T *V, T target, size_t beginning, size_t end, size_t factor1, size_t factor2) {
    factor2 = beginning;
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        size_t middle = (beginning + end) / 2;
        if (factor1 * (middle - factor2) - V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}

// g++ -D _nbits, _log, _nbits512
BitVectorJ::BitVectorJ() {
    const size_t _size = size();

    #ifdef _nbits
    chunk1_size = NBITS * NBITS;
    chunk2_size = NBITS;

    #elifdef _nbits512
    chunk1_size = 512 * NBITS;
    chunk2_size = NBITS;

    #else
    const long double logN = log2((long double) _size);
    chunk1_size = ceil(logN) * floor(logN);
    chunk2_size = chunk1_size / ceil(logN);
    #endif

    layer1_size = (_size + chunk1_size - 1) / chunk1_size + 1;
    chunk2_per_chunk1 = chunk1_size / chunk2_size;
    layer2_size = chunk2_per_chunk1 * (layer1_size - 1);

    layer1 = (size_t *) malloc(layer1_size * sizeof(size_t));
    layer2 = (short *) malloc(layer2_size * sizeof(short));

    size_t layer1_counter = 0;
    size_t layer2_counter = 0;
    for (size_t i = 0; i < layer2_size; i++) {
        if (i % chunk2_per_chunk1 == 0) {
            layer1[i / chunk2_per_chunk1] = layer1_counter;
            layer2_counter = 0;
        }
        layer2[i] = layer2_counter;
        short pop_count = std::__popcount(accessWord(i, chunk2_size));
        layer1_counter += pop_count;
        layer2_counter += pop_count;
    }
    layer1[layer1_size - 1] = layer1_counter;
}

BitVectorJ::~BitVectorJ() {
    if (layer1) free(layer1);
    if (layer2) free(layer2);
    if (select_vector0) free(select_vector0);
    if (select_vector1) free(select_vector1);
}

size_t BitVectorJ::rank0(size_t i) const {
    return i - rank1(i);
}

size_t BitVectorJ::rank1(size_t i) const {
    size_t chunk1 = i / chunk1_size;
    size_t chunk2 = i / chunk2_size;
    unsigned pop_count = std::__popcount(accessWord(chunk2, chunk2_size) & ~bitMask(i % chunk2_size));
    return layer1[chunk1] + layer2[chunk2] + pop_count;
}

// void BitVectorJ::print() {
//     std::cout << "Layer1_Size: " << layer1_size << 
//         "\n" << "Layer2_Size: " << layer2_size << 
//         "\n" << "Chunk1_Size: " << chunk1_size << 
//         "\n" << "Chunk2_Size: " << chunk2_size << 
//         "\n" << "Chunk2_Per_Chunk1: " << chunk2_per_chunk1 << 
//         "\n" << "j: " << select_j << 
//         "\n\n";
//     for (size_t i = 0; i < layer2_size; i++) {
//         if (i % chunk2_per_chunk1 == 0) {
//             std::cout << "\n" << layer1[i / chunk2_per_chunk1] << ":\n";
//         }
//         std::cout << layer2[i] << "   ";
//     }
//
//     // std::cout << "\n";
//     // std::cout << "\n";
//     // for (int i = 0; i < 3; i++) {
//     //     cout << select_vector1[i] << "   ";
//     // }
//     std::cout << "\n";
// }

void BitVectorJ::build_select1() {
    const size_t _size = size();

    select_j = ceil(log2((long double) _size) * log((long double) _size));
    select_j = std::max(select_j, (unsigned) 1);
    select_vector1 = (size_t *) malloc(((rank1(_size) + select_j - 1) / select_j + 1) * sizeof(size_t));

    size_t counter = 0;
    select_vector1[0] = 0;
    for (size_t i = 0; i < _size; i++) {
        if ((*this)[i] == 0) continue;

        counter++;
        if (counter % select_j == 0) {
            select_vector1[counter / select_j] = i + 1;
        }
    }

    if (counter % select_j == 0 && counter != 0) {
        select_vector1[counter / select_j] = size();
    }

    select_vector1[(rank1(size()) + select_j - 1) / select_j] = size();
}

void BitVectorJ::build_select0() {
    const size_t _size = size();

    select_j = ceil(log2((long double) _size) * log((long double) _size));
    select_j = std::max(select_j, (unsigned) 1);
    select_vector0 = (size_t *) malloc(((rank0(_size) + select_j - 1) / select_j + 1) * sizeof(size_t));

    size_t counter = 0;
    select_vector0[0] = 0;
    for (size_t i = 0; i < _size; i++) {
        if ((*this)[i] == 1) continue;

        counter++;
        if (counter % select_j == 0) {
            select_vector0[counter / select_j] = i + 1;
        }
    }

    if (counter % select_j == 0 && counter != 0) {
        select_vector0[counter / select_j] = _size;
    }

    select_vector0[(rank0(_size) + select_j - 1) / select_j] = _size;
}

// g++ -D selectstructure (ss??)
size_t BitVectorJ::select0(size_t i) const {
    const size_t _size = size();

    if (i > rank0(_size)) return -1;

    #ifdef selectstructure
    const size_t lower_bound = select_vector0[i / select_j];
    const size_t upper_bound = select_vector0[i / select_j + 1];
    if (i % select_j == 0) return lower_bound;
    #else
    if (i == 0) return 0;
    const size_t lower_bound = 0; 
    const size_t upper_bound = _size;
    #endif

    size_t layer1_pos = binary_search(layer1, i, lower_bound / chunk1_size, (upper_bound + chunk1_size - 1) / chunk1_size, chunk1_size);
    size_t layer2_pos = binary_search(layer2, (short) (i - layer1_pos * chunk1_size + layer1[layer1_pos]), layer1_pos * chunk2_per_chunk1,
                                   MIN((layer1_pos + 1) * chunk2_per_chunk1, layer2_size - 1), chunk2_size, layer1_pos * chunk1_size - layer1[layer1_pos]);
    size_t counter = 0;
    const size_t target = i - (layer2_pos * chunk2_size - layer1[layer1_pos] - layer2[layer2_pos]);

    // Busca sequencial na palavra, é possível fazer uma busca binária com pop_count mas talvez não seja tão eficiente
    for (unsigned j = 0; j < chunk2_size; j++) {
        counter += !(*this)[j + layer2_pos * chunk2_size];
        if (counter == target) {
            return layer2_pos * chunk2_size + j + 1;
        } 
    }
    return chunk2_size + layer2_pos * chunk2_size;
}

size_t BitVectorJ::select1(size_t i) const {
    const size_t _size = size();

    if (i > rank1(_size)) return -1;

    #ifdef selectstructure
    const size_t lower_bound = select_vector1[i / select_j];
    const size_t upper_bound = select_vector1[i / select_j + 1];
    if (i % select_j == 0) {
        return lower_bound; 
    }
    #else
    if (i == 0) return 0;
    const size_t lower_bound = 0;
    const size_t upper_bound = _size;
    #endif

    const size_t layer1_pos = binary_search(layer1, i, lower_bound / chunk1_size, (upper_bound + chunk1_size - 1) / chunk1_size);
    const size_t layer2_pos = binary_search(layer2, (short) (i - layer1[layer1_pos]), layer1_pos * chunk2_per_chunk1, MIN((layer1_pos + 1) * chunk2_per_chunk1, layer2_size - 1));
    size_t counter = 0;
    const size_t target = i - layer1[layer1_pos] - layer2[layer2_pos];

    for (unsigned j = 0; j < chunk2_size; j++) {
        counter += (*this)[j + layer2_pos * chunk2_size];
        if (counter == target) {
            return layer2_pos * chunk2_size + j + 1;
        }
    }
    return 0;
}
#undef MIN
