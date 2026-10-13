#include <cstdlib>
#include "interquartil-stats.hpp"
#include "naive_permutation.hpp"
#include "permutation.hpp"

#include <bits/stdc++.h>
#include <stdexcept>
#include <iostream>
#include <cstring>

using ULL = unsigned long long;
using Clock = chrono::steady_clock;


//---------------------------
//          Parameters      |
//---------------------------
constexpr size_t SHORTCUT_T      = 16;        // 't' parameter of Permutation (tune/sweep this)
constexpr size_t MAX_QUERIES     = 1000000;   // queries per sample for the real structure
constexpr size_t NAIVE_QUERIES   = 1000;      // queries per sample for the naive baseline
constexpr int    NAIVE_MAX_ORDER = 5;         // naive inverse/power are O(n) per query
constexpr ULL    SEED            = 1791409268;

//---------------------------
//      Flags for control   |
//---------------------------
bool verbose    = false;
bool naive      = false;
bool comparison = false;
bool all        = false;

// Operations performed
bool permutation = false;
bool inverse     = false;
bool power       = false;

map<string, string> vstrings =
{
    {"permutation", "Results for P(i): one application of the permutation, random positions.\n\n"},
    {"inverse",     "Results for P^-1(i): inverse of the permutation, random positions.\n\n"},
    {"power",       "Results for P^k(i): k-th power of the permutation, random positions and random k in [1, n].\n\n"},
};

map<string, map<ULL, Statistics>> results;   // our implementation
map<string, map<ULL, double>>     nresults;  // naive

void parse_arguments(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        const char *a = argv[i];

        if      (!strcmp(a, "-v")   || !strcmp(a, "--verbose"))     verbose = true;
        else if (!strcmp(a, "-n")   || !strcmp(a, "--naive"))       naive = true;
        else if (!strcmp(a, "-cmp") || !strcmp(a, "--compare"))     comparison = true;
        else if (!strcmp(a, "-a")   || !strcmp(a, "--all"))         all = true;
        else if (!strcmp(a, "-p")   || !strcmp(a, "--permutation")) permutation = true;
        else if (!strcmp(a, "-i")   || !strcmp(a, "--inverse"))     inverse = true;
        else if (!strcmp(a, "--power"))                             power = true;
    }
}

template <class F>
double time_queries(F &&f, size_t count)
{
    long long acc = 0;
    auto start = Clock::now();
    for (size_t k = 0; k < count; k++) acc += f(k);
    auto end = Clock::now();

    volatile long long sink = acc;
    (void)sink;

    return chrono::duration<double, nano>(end - start).count() / (double)count;
}

double run_compact_benchmark(Permutation &P, const string &op,
                          const vector<int> &idx, const vector<int> &exps)
{
    size_t q = idx.size();
    if (op == "permutation") return time_queries([&](size_t k) { return P(idx[k]); }, q);
    if (op == "inverse")     return time_queries([&](size_t k) { return P.inverse(idx[k]); }, q);
    return time_queries([&](size_t k) { return P.power(idx[k], exps[k]); }, q);
}

double run_naive_benchmark(const NaivePermutation &P, const string &op,
                           const vector<int> &idx, const vector<int> &exps)
{
    size_t q = min(idx.size(), NAIVE_QUERIES);
    if (op == "permutation") return time_queries([&](size_t k) { return P.apply(idx[k]); }, q);
    if (op == "inverse")     return time_queries([&](size_t k) { return P.inv(idx[k]); }, q);
    return time_queries([&](size_t k) { return P.pow(idx[k], exps[k]); }, q);
}

void bench_op(const string &op, int order, ULL n,
              Permutation &P, const NaivePermutation &NP,
              const vector<int> &idx, const vector<int> &exps)
{
    if (order <= NAIVE_MAX_ORDER && (naive || comparison))
    {
        vector<double> nsamples;
        for (int i = 0; i < MIN_SAMPLES; i++)
            nsamples.push_back(run_naive_benchmark(NP, op, idx, exps));

        nresults[op].insert({n, get_median(nsamples)});
        if (verbose) cout << "#";
    }

    if (!naive || comparison)
    {
        for (int w = 0; w < MIN_WARMUP_SAMPLES; w++)
            run_compact_benchmark(P, op, idx, exps);

        int t = 0;
        bool ok = false;
        while (t < MAX_ATTEMPTS)
        {
            vector<double> samples;
            for (int i = 0; i < MIN_SAMPLES; i++)
                samples.push_back(run_compact_benchmark(P, op, idx, exps));

            Statistics s = calculate_interquartil(samples);
            if (s.is_valid)
            {
                results[op].insert({n, s});
                ok = true;
                break;
            }
            t++;
        }

        if (!ok)
            throw runtime_error("Permutation " + op + " operation failed for interquartile metrics.");
    }
    if (verbose) cout << "#";
}

int main(int argc, char *argv[])
{
    if (argc <= 1)

    {
        cout << "No benchmark flags were provided, check this folder's README.md for more informations." << endl;
        exit(EXIT_FAILURE);
    }

    parse_arguments(argc, argv);

    if (!all && !permutation && !inverse && !power)
    {
        cout << "No operation selected (-p, -i, --power or -a)." << endl;
        exit(EXIT_FAILURE);
    }

    mt19937_64 rng(SEED);

    if (verbose)
    {
        cout << "All the following tests were computed on permutations of size 10^4 to 10^8 for the "
                "implementation and 10^4 to 10^" << NAIVE_MAX_ORDER << " for naive operations :" << endl;
        cout << "[";
    }

    for (int order = 4; order <= 8; order++)
    {
        if (naive && !comparison && order > NAIVE_MAX_ORDER) continue;

        ULL n = 1;
        for (int i = 0; i < order; i++) n *= 10;

        // Random permutation of [0, n-1]
        vector<int> perm(n);
        iota(perm.begin(), perm.end(), 0);
        shuffle(perm.begin(), perm.end(), rng);

        NaivePermutation NP;
        if (order <= NAIVE_MAX_ORDER && (naive || comparison)) NP.p = perm;

        Permutation P(std::move(perm), SHORTCUT_T);

        // Random query positions (random order => realistic cache behaviour) and random exponents
        size_t q = min<size_t>(n, MAX_QUERIES);
        uniform_int_distribution<int> pos_dist(0, (int)n - 1);
        uniform_int_distribution<int> exp_dist(1, (int)n);

        vector<int> idx(q), exps(q);
        for (size_t i = 0; i < q; i++)
        {
            idx[i]  = pos_dist(rng);
            exps[i] = exp_dist(rng);
        }

        if (all || permutation) bench_op("permutation", order, n, P, NP, idx, exps);
        if (all || inverse)     bench_op("inverse",     order, n, P, NP, idx, exps);
        if (all || power)       bench_op("power",       order, n, P, NP, idx, exps);
    }

    if (verbose) cout << "]\n\n";
    else cout << "\"Order\"" << ";" << "\"Time\"" << endl;

    if (!naive || comparison)
    {
        for (auto &el : results)
        {
            if (verbose) cout << vstrings[el.first];
            for (auto &res : el.second)
            {
                if (verbose)
                {
                    cout << "Operations for size " << res.first << " :" << endl;
                    cout << "==============================" << endl;
                    cout << "q1: " << res.second.q1 << endl;
                    cout << "q2: " << res.second.median << endl;
                    cout << "q3: " << res.second.q3 << endl;
                    cout << "iqr: " << res.second.iqr << endl;
                    cout << "lower bound: " << res.second.lower_bound << endl;
                    cout << "upper bound: " << res.second.upper_bound << endl;
                    cout << "mean: " << res.second.mean << endl;
                    cout << "standard deviation: " << res.second.standard_deviation << endl;
                    cout << "coefficient_variation: " << res.second.coefficient_variation << endl;
                    cout << "valid samples: " << res.second.valid.size() << endl;
                    cout << "outlier samples: " << res.second.outliers.size() << endl;
                }
                else cout << res.first << ';' << res.second.median;
                cout << "\n";
            }
            cout << "\n";
        }
    }
    if (naive || comparison)
    {
        for (auto &el : nresults)
        {
            if (verbose) cout << "[naive] " << vstrings[el.first];
            for (auto &res : el.second)
                cout << res.first << ';' << res.second << "\n";
            cout << "\n";
        }
        cout << "\n\n";
    }
    return 0;
}
