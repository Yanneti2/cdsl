#ifndef UVECTOR
#define UVECTOR

#include "bitvector.hpp"

class UVector {
    public:
        BitVector B;
        unsigned size;

    public:
        UVector(unsigned word_size);
        UVector(unsigned word_size, size_t size);

        long long operator[](size_t i);
        void push_back(long long x);
        void set(size_t i, long long x);
};

#endif

