#ifndef NAIVE_PERMUTATION_HPP
#define NAIVE_PERMUTATION_HPP

#include <vector>

// Baseline used for benchmarks and as the reference in the tester.
// Every query walks the cycle, so it is O(cycle length) per call.
struct NaivePermutation
{
    std::vector<int> p;

    int apply(int i) const { return p[i]; }

    // Length of the cycle containing i
    long long cycle_length(int i) const
    {
        long long len = 1;
        for (int j = p[i]; j != i; j = p[j]) len++;
        return len;
    }

    // P^-1(i): walk the cycle until the element just before i
    int inv(int i) const
    {
        int j = p[i];
        while (p[j] != i) j = p[j];
        return j;
    }

    // P^k(i) for any integer k (positive, negative, huge)
    int pow(int i, long long k) const
    {
        long long len = cycle_length(i);
        long long r = ((k % len) + len) % len;
        for (long long s = 0; s < r; s++) i = p[i];
        return i;
    }
};

#endif
