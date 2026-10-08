#include "bitvector.hpp"
#include "wt_implicit.hpp"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <cmath>
#include <set>

void WaveletTreeSuccint::get_alphabet(const UVector &S) {
    set<uint64_t> alpha_set;
    vector<uint64_t> alpha_vec;
    for (size_t i = 0; i < S.size(); i++) {
        uint64_t letter = S[i];

        if (alpha_set.find(letter) != alpha_set.end())
            continue;

        alpha_set.insert(letter);
        alpha_vec.push_back(letter);
    }

    sort(alpha_vec.begin(), alpha_vec.end());

    for (uint64_t letter : alpha_vec) {
        alpha.push_back(letter);
    }
}

void WaveletTreeSuccint::get_tree(const UVector &S, size_t alpha_begin, size_t alpha_end, size_t depth) {
    size_t alpha_size = alpha_end - alpha_begin;
    if (alpha_size <= 1) return;

    UVector l(S.word_size);
    UVector r(S.word_size);

    size_t l_size = alpha_size / 2;
    size_t r_size = alpha_size - r_size;

    size_t offset = depth * str_size + rankc[alpha_begin];
    for (size_t i = 0; i < S.size(); i++) {
        if (S[i] < alpha[alpha_begin + l_size]) {
            B.set0(offset + i);
            l.push_back(S[i]);
        } else {
            B.set1(offset + i);
            r.push_back(S[i]);
        }
    }

    get_tree(l, alpha_begin, alpha_begin + l_size, depth + 1);
    get_tree(r, alpha_begin + l_size, alpha_end, depth + 1);
}

WaveletTreeSuccint::WaveletTreeSuccint(const string &S) : alpha(8), str_size(S.size()) {
    UVector str(8);

    for (char c : S) {
        str.push_back(c);
    }

    get_alphabet(str);

    B = BitVectorJ(ceil(log2(alpha.size())) * str.size(), 0);

    rankc = (size_t *) calloc(alpha.size() + 1, sizeof(size_t));
    size_t *alpha_inverse = (size_t *) malloc(256 * sizeof(size_t));

    for (size_t i = 0; i < alpha.size(); i++) {
        alpha_inverse[alpha[i]] = i;
    }

    for (size_t i = 0; i < str.size(); i++) {
        rankc[alpha_inverse[S[i]] + 1]++;
    }

    for (size_t i = 0; i < alpha.size(); i++) {
        rankc[i + 1] += rankc[i];
    }

    get_tree(str, 0, alpha.size(), 0);
    B.init();
    B.build_select0();
    B.build_select1();
}

size_t WaveletTreeSuccint::rank(uint64_t c, size_t i) const {
    size_t beg = 0;
    size_t end = alpha.size();
    size_t depth = 0;
    size_t offset = 0;

    while (beg + 1 != end) {
        size_t mid = (beg + end) / 2;
        offset = rankc[beg] + depth * str_size;
        if (c < alpha[mid]) {
            i = B.rank0(i + offset) - B.rank0(offset);
            end = mid;
        } else {
            i = B.rank1(i + offset) - B.rank1(offset);
            beg = mid;
        }
        size_t _i = i;
        depth++;
    }
    return i;
}

size_t WaveletTreeSuccint::select(uint64_t c,  size_t i) const {
    size_t beg = 0;
    size_t end = alpha.size();
    size_t mid;
    pair<size_t, bool> *stack = (pair<size_t, bool> *) malloc(ceil(log2(alpha.size())) * sizeof(pair<size_t, bool>));
    size_t depth = 0;

    stack[0].first = beg;
    while (beg + 1 != end) {
        mid = (beg + end) / 2;
        if (c < alpha[mid]) {
            end = mid;
            stack[depth].second = false;
        } else {
            beg = mid;
            stack[depth].second = true;
        }
        stack[++depth].first = beg;
    }

    while (depth--) {
        if (stack[depth].second) {
            i = B.select1(B.rank1(depth * str_size + rankc[stack[depth].first]) + i) - depth * str_size;
        } else {
            i = B.select0(B.rank0(depth * str_size + rankc[stack[depth].first]) + i) - depth * str_size;
        }
    }
    free(stack);
    return i;
}

uint64_t WaveletTreeSuccint::access(size_t i) const {
    size_t beg = 0;
    size_t end = alpha.size();
    size_t mid;
    size_t depth = 0;
    size_t offset = 0;

    while (beg + 1 != end) {
        mid = (beg + end) / 2;
        offset = rankc[beg] + depth * str_size;

        if (B[i + offset]) {
            i = B.rank1(i + offset) - B.rank1(offset);
            beg = mid;
        } else {
            i = B.rank0(i + offset) - B.rank0(offset);
            end = mid;
        }
        depth++;
    }
    return alpha[beg];
}

WaveletTreeSuccint::~WaveletTreeSuccint() {
    if (rankc) free(rankc);
}
