#include "../include/jacobsonrank.h"
#include "../include/bitvector.h"
#include <iostream>

#ifndef bitMask
#ifdef IS32BIT
#define bitMask(i) bitMask32[(i)]
#else
#define bitMask(i) bitMask64[(i)]
#endif
#endif

#define MIN(A, B) ((A) < (B) ? (A) : (B)) 
#define ULL unsigned long long

extern uint32_t bitMask32[];
extern uint64_t bitMask64[];

template <typename T>
unsigned long long binary_search(T *V, T target, unsigned long long beginning, unsigned long long end) {
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

        // if (V[middle] > target) {
        //     end = middle - 1;
        // } else if (V[middle] < target) {
        //     beginning = middle + 1;
        // } else {
        //     return middle - 1;
        // }
        // if (end < beginning) {
        //     return end;
        // }
    }
}

// 1 6 7 9 12 19


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

template <typename T>
unsigned long long binary_search(T *V, T target, unsigned long long beginning, unsigned long long end, unsigned long long factor1, unsigned long long factor2) {
    // 47, 10, 20, 9, 46
    // 0   8   12   17   19   21   25   30   33   40
    factor2 = beginning;
    if (end == beginning) {
        return end;
    }
    while(1) {
        if (end == beginning) {
            return end - 1;
        }
        //135 - 46 - 21 = 68 ==> 10, 15, 12
        //108 - 46 - 12 = 50 ==> 10, 12, 11
        //99  - 46 -  8 = 45 ==> 12, 12, 12
        unsigned long long middle = (beginning + end) / 2;
        if (factor1 * (middle - factor2) - V[middle] < target) {
            beginning = middle + 1;
        } else {
            end = middle;
        }
    }
}

JacobsonRank::JacobsonRank(){   
}

// g++ -D _nbits, _log, _nbits512
JacobsonRank::JacobsonRank(bitVector *B) {
    const ULL B_length = B->size();
    unsigned chunk1_size;
    unsigned chunk2_size;
    ULL layer1_size;
    ULL layer2_size;
    unsigned chunk2_per_chunk1;

    #ifdef _nbits
    chunk1_size = NBITS * NBITS;
    chunk2_size = NBITS;
    #endif
     
    #ifdef _log
    const long double logN = log2((long double)B_length);
    chunk1_size = ceil(logN) * floor(logN);
    chunk2_size = chunk1_size / ceil(logN);
    #endif

    #ifdef _nbits512
    chunk1_size = 512 * NBITS;
    chunk2_size = NBITS;
    #endif

    layer1_size = (B_length + chunk1_size - 1) / chunk1_size + 1;
    chunk2_per_chunk1 = (chunk1_size + chunk2_size - 1) / chunk2_size;
    layer2_size = chunk2_per_chunk1 * layer1_size;

    this->chunk1_size = chunk1_size;
    this->chunk2_size = chunk2_size;
    this->layer1_size = layer1_size;
    this->layer2_size = layer2_size;
    this->chunk2_per_chunk1 = chunk2_per_chunk1;

    this->layer1 = (ULL*) malloc(layer1_size * sizeof(ULL));
    this->layer2 = (short*) malloc(layer2_size * sizeof(short));

    ULL layer1_counter = 0;
    ULL layer2_counter = 0;
    for(ULL i = 0; i < layer2_size; i++) {
        if (i % chunk2_per_chunk1 == 0) {
            this->layer1[i / chunk2_per_chunk1] = layer1_counter;
            layer2_counter = 0;
        }
        this->layer2[i] = layer2_counter;
        short pop_count = std::__popcount(B->accessWord(i, chunk2_size));
        layer1_counter += pop_count;
        layer2_counter += pop_count;
    }
    this->layer1[layer1_size - 1] = layer1_counter;
}

JacobsonRank::~JacobsonRank() {
    if (layer1) free(layer1);
    if (layer2) free(layer2);
    //if (select_vector0) free(select_vector0);
    //if (select_vector1) free(select_vector1);
}

ULL JacobsonRank::rank1(bitVector *B, ULL i) {
    ULL chunk1 = i / chunk1_size;
    ULL chunk2 = i / chunk2_size;
    unsigned pop_count = std::__popcount(B->accessWord(chunk2, chunk2_size) & ~bitMask(i % chunk2_size));
    return layer1[chunk1] + layer2[chunk2] + pop_count;
}

ULL JacobsonRank::rank0(bitVector *B, ULL i) {
    return i - rank1(B, i);
}

void JacobsonRank::print() {
    std::cout << "Layer1_Size: " << layer1_size << 
        "\n" << "Layer2_Size: " << layer2_size << 
        "\n" << "Chunk1_Size: " << chunk1_size << 
        "\n" << "Chunk2_Size: " << chunk2_size << 
        "\n" << "Chunk2_Per_Chunk1: " << chunk2_per_chunk1 << 
        "\n" << "j: " << select_j << 
        "\n\n";
    for (ULL i = 0; i < layer2_size; i++) {
        if (i % chunk2_per_chunk1 == 0) {
            std::cout << "\n" << layer1[i / chunk2_per_chunk1] << ":\n";
        }
        std::cout << layer2[i] << "   ";
    }

    // std::cout << "\n";
    // std::cout << "\n";
    // for (int i = 0; i < 3; i++) {
    //     cout << select_vector1[i] << "   ";
    // }
    std::cout << "\n";
}

void JacobsonRank::build_select1(bitVector *B) {
    const ULL B_length = B->size();
    ULL select_j = ceil(log2((long double)B_length) * log((long double)B_length));
    if (select_j == 0) select_j = 1;
    ULL *select_vector1 = (ULL *) malloc(((rank1(B, B->size()) + select_j - 1) / select_j + 1) * sizeof(ULL));
    ULL counter = 0;
    select_vector1[0] = 0;
    for (ULL i = 0; i < B->size(); i++) {
        if ((*B)[i] == 1) {
            counter++;
            if (counter % select_j == 0) {
                select_vector1[counter / select_j] = i + 1;
            } 
        }
    } 
    if (counter % select_j == 0 && counter != 0) {
        select_vector1[counter / select_j] = B->size();
    } 
    select_vector1[(rank1(B, B->size()) + select_j - 1) / select_j] = B->size();
    this->select_j = select_j;
    this->select_vector1 = select_vector1;
}

void JacobsonRank::build_select0(bitVector *B) {
    const ULL B_length = B->size();
    ULL select_j = ceil(log2((long double)B_length) * log((long double)B_length));
    if (select_j == 0) select_j = 1;
    ULL *select_vector0 = (ULL *) malloc(((rank0(B, B->size()) + select_j - 1) / select_j + 1) * sizeof(unsigned long long));
    ULL counter = 0;
    select_vector0[0] = 0;
    for (ULL i = 0; i < B->size(); i++) {
        if ((*B)[i] == 0) {
            counter++;
            if (counter % select_j == 0) {
                select_vector0[counter / select_j] = i + 1;
            }
        }
    }
    if (counter % select_j == 0 && counter != 0) {
        select_vector0[counter / select_j] = B->size();
    } 
    select_vector0[(rank0(B, B->size()) + select_j - 1) / select_j] = B->size();
    this->select_j = select_j;
    this->select_vector0 = select_vector0;
}

// g++ -D selectstructure (ss??)
ULL JacobsonRank::select0(bitVector *B, ULL i) {
    if (i > rank0(B, B->size())) return -1;
    #ifdef selectstructure
    const ULL lower_bound = select_vector0[i / select_j];
    const ULL upper_bound = select_vector0[i / select_j + 1];
    if (i % select_j == 0) return lower_bound;
    #else
    if (i == 0) return 0;
    const ULL lower_bound = 0; 
    const ULL upper_bound = B->size();
    #endif
    ULL layer1_pos = binary_search(layer1, i, lower_bound / chunk1_size, (upper_bound + chunk1_size - 1) / chunk1_size, chunk1_size);
    ULL layer2_pos = binary_search(layer2, (short) (i - layer1_pos * chunk1_size + layer1[layer1_pos]), layer1_pos * chunk2_per_chunk1,
                                   MIN((layer1_pos + 1) * chunk2_per_chunk1, layer2_size - 1), chunk2_size, layer1_pos * chunk1_size - layer1[layer1_pos]);
    ULL counter = 0;
    const ULL target = i - (layer2_pos * chunk2_size - layer1[layer1_pos] - layer2[layer2_pos]);

    // Busca sequencial na palavra, é possível fazer uma busca binária com pop_count mas talvez não seja tão eficiente
    for (ULL j = 0; j < chunk2_size; j++) {
        counter += 1 ^ ((*B)[j + layer2_pos * chunk2_size]);
        if (counter == target) {
            return layer2_pos * chunk2_size + j + 1;
        } 
    }
    return chunk2_size + layer2_pos * chunk2_size;
}

ULL JacobsonRank::select1(bitVector *B, ULL i) {
    if (i > rank1(B, B->size())) return -1;
    #ifdef selectstructure
    const ULL lower_bound = select_vector1[i / select_j];
    const ULL upper_bound = select_vector1[i / select_j + 1];
    if (i % select_j == 0) {
        return lower_bound; 
    }
    #else
    if (i == 0) return 0;
    const ULL lower_bound = 0;
    const ULL upper_bound = B->size();
    #endif
    const ULL layer1_pos = binary_search(layer1, i, lower_bound / chunk1_size, (upper_bound + chunk1_size - 1) / chunk1_size);
    const ULL layer2_pos = binary_search(layer2, (short) (i - layer1[layer1_pos]), layer1_pos * chunk2_per_chunk1, MIN((layer1_pos + 1) * chunk2_per_chunk1, layer2_size - 1));
    ULL counter = 0;
    const ULL target = i - layer1[layer1_pos] - layer2[layer2_pos];

    for (ULL j = 0; j < chunk2_size; j++) {
        counter += (*B)[j + layer2_pos * chunk2_size];
        if (counter == target) {
            return layer2_pos * chunk2_size + j + 1;
        }
    }
    return 0;
}
#undef MIN
