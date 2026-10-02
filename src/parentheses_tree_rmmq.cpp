#include "parentheses_tree_rmmq.hpp"
#include "cmath"
#include "climits"
#include "iostream"

#define max(a, b) ((a) > (b) ? (a) : (b))

using namespace std;

size_t ParenthesesTreeRMMQ::leafnum(size_t k) {
    size_t leaves = (T.size() + b) / b;
    size_t p = pow(2, ceil(log2(leaves)));
    if (k < 2 * leaves - p)
        return p + k - 1;

    return p + k - leaves - 1;
}

size_t ParenthesesTreeRMMQ::numleaf(size_t v) {
    size_t leaves = (T.size() + b) / b;
    size_t p = pow(2, ceil(log2(leaves)));
    v++;
    if (v >= p)
        return v - p;

    return v - p + leaves;
}

ParenthesesTreeRMMQ::~ParenthesesTreeRMMQ() {
    if (rmmq_tree) {
        free(rmmq_tree);
    }
}

void ParenthesesTreeRMMQ::init() {
    size_t size = T.size();
    size_t leaves = (size + b) / b;

    rmmq_tree = (RMMQNode *) malloc((2 * leaves - 1) * sizeof(RMMQNode));

    for (size_t leaf = 0; leaf < leaves; leaf++) {
        size_t leaf_pos = leafnum(leaf);
        RMMQNode *cur = rmmq_tree + leaf_pos;
        cur->e = 0;
        cur->max = LLONG_MIN;
        cur->min = LLONG_MAX;
        cur->min_count = 0;

        for (size_t i = b * leaf; i < min(b * (leaf + 1), size + 1); i++) {
            cur->e += i ? (T[i - 1] ? 1 : -1) : 0;
            if (cur->e < cur->min) {
                cur->min = cur->e;
                cur->min_count = 1;
            } else if (cur->e == cur->min) {
                cur->min_count++;
            }

            if (cur->e > cur->max) {
                cur->max = cur->e;
            }
        }
    }

    for (long long i = leaves - 2; i != -1; i--) {
        RMMQNode *cur = rmmq_tree + i;
        RMMQNode *l = rmmq_tree + 2 * i + 1;
        RMMQNode *r = rmmq_tree + 2 * i + 2;
        cur->e = l->e + r->e;

        if (l->max > r->max) {
            cur->max = l->max;
        } else {
            cur->max = r->max;
        }

        if (l->min == r->min) {
            cur->min = l->min;
            cur->min_count = l->min_count + r->min_count;
        } else if (l->min < r->min) {
            cur->min = l->min;
            cur->min_count = l->min_count;
        } else {
            cur->min = r->min;
            cur->min_count = r->min_count;
        }

    }
    for (int i = 0; i < 2 * leaves - 1; i ++) {
        RMMQNode *cur = &(rmmq_tree[i]);
    }

}
// 111000
// b = 4
//

unsigned long long ParenthesesTreeRMMQ::forward_search(size_t i, long long d) {
    for (size_t j = i + 1; j < min((i / b + 1) * b, T.size() + 1); j++) {
        d -= T[j - 1] ? 1 : -1;
        if (d == 0) return j;
    }

    size_t leaf = leafnum(i / b);

    while (__popcount(leaf + 2) != 0 && rmmq_tree[leaf + 1].min > d) {
        if (leaf % 2) {
            d -= rmmq_tree[leaf + 1].e;
        }

        leaf = (leaf - 1) / 2;
    }

    if (__popcount(leaf + 2) == 0) return -1;

    leaf++;

    while (leaf < (T.size() + b) / b - 1) {
        if (rmmq_tree[2 * leaf + 1].min <= d) {
            leaf = 2 * leaf + 1;
        } else {
            d -= rmmq_tree[2 * leaf + 1].e;
            leaf = 2 * leaf + 2;
        }
    }

    size_t k = numleaf(leaf);

    size_t j;
    for (j = k * b; j < min((k + 1) * b, T.size() + 1); j++) {
        if (j != 0) d -= T[j - 1] ? 1 : -1;
        if (d == 0) return j;
    }

    return -1;
}

unsigned long long ParenthesesTreeRMMQ::backward_search(size_t i, long long d) {
    for (size_t j = i; j + 1 > max((i / b) * b, 1); j--) {
        d += T[j - 1] ? 1 : -1;
        if (d == 0) return j - 1;
    }

    size_t leaf = leafnum(i / b);

    while (__popcount(leaf + 1) != 0 && rmmq_tree[leaf - 1].min - rmmq_tree[leaf - 1].e > d) {
        if (leaf % 2 == 0) {
            d += rmmq_tree[leaf - 1].e;
        }

        leaf = (leaf - 1) / 2;
    }

    if (__popcount(leaf + 1) == 0) return -1;

    leaf--;

    while (leaf < (T.size() + b) / b - 1) {
        if (rmmq_tree[2 * leaf + 1].min - rmmq_tree[leaf - 1].e <= d) {
            leaf = 2 * leaf + 2;
        } else {
            d += rmmq_tree[2 * leaf + 2].e;
            leaf = 2 * leaf + 1;
        }
    }

    size_t k = numleaf(leaf);

    size_t j;
    for (j = (k + 1) * b - 1; j > k * b; j--) {
        d += T[j - 1] ? 1 : -1;
        if (d == 0) return j - 1;
    }

    return -1;
}
