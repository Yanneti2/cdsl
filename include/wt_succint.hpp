#ifndef WT_SUCCINT
#define WT_SUCCINT

#include "bitvectorj.hpp"
#include "bitvector.hpp"
#include "uvector.hpp"

#include <iostream>
#include <vector>
#include <map>

using namespace std;

class WaveletTreeSuccint {
    private:
        BitVectorJ B;
        UVector alpha;
        size_t str_size;
        size_t *rankc = nullptr;

        void get_alphabet(const UVector &S);
        void get_tree(const UVector &S, size_t alpha_begin, size_t alpha_end, size_t depth);

    public:
        WaveletTreeSuccint(const string &S);
        WaveletTreeSuccint(const UVector &S, unsigned letter_size);
        ~WaveletTreeSuccint();

        uint64_t access(size_t i) const;
        size_t rank(uint64_t c, size_t i) const;
        size_t select(uint64_t c, size_t i) const;

        void print();
};

#endif
