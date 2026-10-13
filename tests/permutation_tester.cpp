#include "naive_permutation.hpp"
#include "permutation.hpp"

#include <iostream>
#include <cassert>
#include <climits>
#include <stdexcept>
#include <system_error>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

// ---------------------------------------------------------------
// Permutation builders
// ---------------------------------------------------------------

vector<int> identity_permutation(size_t n)
{
    vector<int> p(n);
    iota(p.begin(), p.end(), 0);
    return p;
}

// 0 -> 1 -> 2 -> ... -> n-1 -> 0
vector<int> rotation(size_t n)
{
    vector<int> p(n);
    for (size_t i = 0; i < n; i++)
        p[i] = (i + 1) % n;
    return p;
}

// i -> n-1-i (only 2-cycles and at most one fixed point)
vector<int> reversal(size_t n)
{
    vector<int> p(n);
    for (size_t i = 0; i < n; i++)
        p[i] = n - 1 - i;
    return p;
}

// Builds a permutation whose cycles have exactly the given lengths,
// using the labels in a random order
vector<int> from_cycle_lengths(const vector<size_t> &lengths, mt19937 &rng)
{
    size_t n = 0;
    for (size_t l : lengths)
        n += l;

    vector<int> labels = identity_permutation(n);
    shuffle(labels.begin(), labels.end(), rng);

    vector<int> p(n);
    size_t pos = 0;
    for (size_t l : lengths)
    {
        for (size_t k = 0; k < l; k++)
            p[labels[pos + k]] = labels[pos + (k + 1) % l];
        pos += l;
    }
    return p;
}

vector<int> random_permutation(size_t n, mt19937 &rng)
{
    vector<int> p = identity_permutation(n);
    shuffle(p.begin(), p.end(), rng);
    return p;
}

// ---------------------------------------------------------------
// Comparing Permutation implementation vs reference
// ---------------------------------------------------------------

void check_all(const vector<int> &perm, size_t t)
{
    size_t n = perm.size();
    NaivePermutation naiveP;
    naiveP.p = perm;

    Permutation P(perm, t);

    assert(P.size() == n);

    for (size_t ui = 0; ui < n; ui++)
    {
        int i = (int)ui;

        // basic operations
        assert(P(i) == naiveP.apply(i));
        assert(P.inverse(i) == naiveP.inv(i));
        assert(P.power(i, 1) == naiveP.apply(i));
        assert(P.power(i, -1) == naiveP.inv(i));
        assert(P.power(i, 0) == i);

        // P and P^-1 undo each other
        assert(P(P.inverse(i)) == i);
        assert(P.inverse(P(i)) == i);

        // small exponents, positive and negative
        for (int k = -5; k <= 5; k++)
            assert(P.power(i, k) == naiveP.pow(i, k));

        // exponents around the cycle length (full turns, one less, one more)
        long long L = naiveP.cycle_length(i);
        for (long long k : {L - 1, L, L + 1, 2 * L, 2 * L + 1, 3 * L - 1})
        {
            assert(P.power(i, (int)k) == naiveP.pow(i, k));
            assert(P.power(i, (int)-k) == naiveP.pow(i, -k));
        }
        assert(P.power(i, (int)L) == i);
        assert(P.power(i, (int)-L) == i);

        // huge exponents
        for (int k : {1000003, -1000003, INT_MAX, -INT_MAX, INT_MIN})
            assert(P.power(i, k) == naiveP.pow(i, k));

        // power(power(i, a), b) == power(i, a + b)
        assert(P.power(P.power(i, 3), -7) == P.power(i, -4));
        assert(P.power(P.power(i, -2), 2) == i);
    }
}

// ---------------------------------------------------------------

int main(void)
{
    mt19937 rng(20261009);

    const vector<size_t> ts = {1, 2, 3, 4, 5, 7, 8, 16, 100, 1000};

    //---------------------------
    //     Empty permutation    |
    //---------------------------
    {
        try {
            Permutation P(vector<int>(), 3);
            assert(false); // if we get here, the exception has not be thrown
        } catch (::invalid_argument) {

        }
    }

    //---------------------------
    //     Size 1 (fixed pt)    |
    //---------------------------
    for (size_t t : ts)
    {
        try {
            check_all(vector<int>{0}, t);
            assert(false); // if we get here, the exception has not be thrown
        } catch (::invalid_argument) {
        }
    }

    //---------------------------
    //         Size 2           |
    //---------------------------
    for (size_t t : ts)
    {
        check_all(vector<int>{0, 1}, t); // identity
        check_all(vector<int>{1, 0}, t); // single swap
    }

    //---------------------------
    //   Identity (all fixed)   |
    //---------------------------
    for (size_t n : {2, 3, 10, 64, 65, 200})
        for (size_t t : ts)
            check_all(identity_permutation(n), t);

    //---------------------------
    //   Single cycle: rotation |
    //---------------------------
    for (size_t n : {2, 3, 4, 5, 10, 63, 64, 65, 129, 300})
        for (size_t t : ts)
            check_all(rotation(n), t);

    //---------------------------
    //   Reversal (involution)  |
    //---------------------------
    for (size_t n : {2, 3, 4, 5, 10, 11, 64, 65, 200})
        for (size_t t : ts)
            check_all(reversal(n), t);

    //---------------------------------------------------------------
    //  Single cycle of every length L vs every t (boundaries around
    //  L == t, L == t+1, L == k*t, L == k*t+1, t > L, t == 1)
    //---------------------------------------------------------------
    for (size_t L = 2; L <= 40; L++)
        for (size_t t = 2; t <= 45; t++)
            check_all(from_cycle_lengths({L}, rng), t);

    //---------------------------
    //   Hand-picked cycle mixes |
    //---------------------------
    {
        vector<vector<size_t>> mixes = {
            {1, 1, 1, 1},                // only fixed points
            {2, 2, 2, 2},                // only swaps
            {1, 2, 3, 4, 5},             // growing lengths
            {5, 4, 3, 2, 1},             // shrinking lengths
            {1, 50, 1},                  // fixed points around a long cycle
            {50, 1, 1, 1},               // long cycle first
            {1, 1, 1, 50},               // long cycle last
            {3, 3, 3, 3, 3},             // equal lengths
            {7, 1, 7, 1, 7},             // alternating
            {100},                       // one long cycle
            {64, 64},                    // lengths on a word boundary
            {65, 63},                    // lengths straddling a word boundary
            {1, 2, 4, 8, 16, 32, 64},    // powers of two
        };
        for (auto &mix : mixes)
            for (size_t t : ts)
                check_all(from_cycle_lengths(mix, rng), t);
    }

    //---------------------------
    //   Random permutations    |
    //---------------------------
    for (size_t n : {3, 4, 5, 7, 10, 31, 64, 65, 100, 257})
        for (size_t t : ts)
            for (int trial = 0; trial < 10; trial++)
                check_all(random_permutation(n, rng), t);

    //---------------------------
    //  Larger smoke test       |
    //---------------------------
    {
        vector<int> perm = random_permutation(5000, rng);
        NaivePermutation naiveP;
        naiveP.p = perm;
        for (size_t t : {1, 2, 10, 70, 5000})
        {
            Permutation P(perm, t);
            assert(P.size() == 5000);
            for (int i = 0; i < 5000; i++)
            {
                assert(P(i) == naiveP.apply(i));
                assert(P.inverse(i) == naiveP.inv(i));
            }
        }
    }

    cout << "All permutation tests passed" << endl;
    return 0;
}
