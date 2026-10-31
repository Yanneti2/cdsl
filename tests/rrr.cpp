#include "bitvectorj.hpp"
#include "rrr.hpp" 

#include <assert.h>
#include <chrono>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <math.h>

using namespace std;

template <typename F>
double bench_ns(const vector<ULL>& queries, F f) {
    ULL sink = 0;
    auto t0 = chrono::steady_clock::now();
    for (ULL x : queries) sink += f(x);
    auto t1 = chrono::steady_clock::now();
    volatile ULL keep = sink; (void)keep;
    return chrono::duration<double, nano>(t1 - t0).count() / queries.size();
}

int main(void) {
    for (long long unsigned size = 1000; size < 100000000000000000; size *= 10) {
        BitVectorJ B(64, 2.0);
        int order = log10(size);

        for (TYPE i = 0; i < size; i++) {
            if (rand() % 2 == 0) {
                B.append1();
            } else {
                B.append0();
            }
        }
        rrr R(&B);
        B.init();
        if (!B.is_initialized()){
            printf("Bv not initialized\n\n");
            return 0;
        }
        ULL N = B.size();
        vector<ULL> pre(N + 1, 0);
        for (ULL k = 0; k < N; k++) pre[k + 1] = pre[k] + B[k];

    //     for (ULL j = 0; j + 1 < R.total_blocks; j++) {
    //         uint32_t expected = 0;
    //         for (unsigned p = 0; p < R.block_size; p++)
    //             expected = (expected << 1) | B[j * R.block_size + p];   // first bit becomes the highest
    //         if (R.decode_block(j) != expected) { printf("block %llu differs\n", j); break; }
    //     }

    //     for (ULL k = 0; k <= size; k++)
    //         if (R.rank1(k) != pre[k]) {
    //             printf("rank1(%llu)=%llu expected %llu\n", k, R.rank1(k), pre[k]);
    //             break;
    //         }
    // correctness first: timing broken code means nothing
    for (ULL k = 0; k <= N; k++) {
        assert(R.rank1(k) == pre[k]);
        assert(R.rank0(k) == k - pre[k]);
    }
    ULL ones_seen = 0, zeros_seen = 0;
    for (ULL k = 0; k < N; k++) {
        if (B[k]) assert(R.select1(&B, ++ones_seen)  == k + 1);   // position + 1, your convention
        else      assert(R.select0(&B, ++zeros_seen) == k + 1);
    }

    // benchmark
    const ULL Q = 1000000;
    mt19937_64 rng(42);
    ULL total_zeros = N - R.total_ones;
    vector<ULL> pos(Q), t1(Q), t0(Q);
    for (ULL q = 0; q < Q; q++) {
        pos[q] = rng() % N;                       // random position
        t1[q]  = 1 + rng() % R.total_ones;        // random i-th one
        t0[q]  = 1 + rng() % total_zeros;         // random i-th zero
    }
    printf("n=%-8llu  raw B[k] %6.1f | access %6.1f | rank1 %6.1f | rank0 %6.1f"
        " | select1 %6.1f | select0 %6.1f   (ns/op)\n", N,
        bench_ns(pos, [&](ULL k) { return (ULL) B[k]; }),
        bench_ns(pos, [&](ULL k) { return R.rank1(k); }),
        bench_ns(pos, [&](ULL k) { return R.rank0(k); }),
        bench_ns(t1,  [&](ULL t) { return R.select1(&B, t); }),
        bench_ns(t0,  [&](ULL t) { return R.select0(&B, t); }));
    }
}