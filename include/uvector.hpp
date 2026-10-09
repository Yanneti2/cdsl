#ifndef UVECTOR
#define UVECTOR

#include "bitvector.hpp"

class UVector {
    public:
        BitVector B;
        unsigned word_size;

    public:
        UVector(unsigned word_size);
        UVector(unsigned word_size, size_t size);

        long long operator[](size_t i) const;
        void push_back(long long x);
        void set(size_t i, long long x);
        size_t size() const;
};

#endif

