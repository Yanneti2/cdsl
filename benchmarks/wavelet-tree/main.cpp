#include "wt_succint.hpp"
#include "wt.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <chrono>
#include <cmath>

#define ACCESS  (1 << 0)
#define RANKC   (1 << 1)
#define SELECTC (1 << 2)

bool succinct = false;
bool verbose = false;
bool both = false;
bool all = false;

using namespace std;

void parse_args(int argc, char* argv[], uint64_t &args) {
    for (int i = 1; i < argc; i++)
    {
        const char* cur_arg = argv[i];

        if (!strcmp(cur_arg, "-v") || !strcmp(cur_arg, "--verbose"))
            verbose = true;
        else if (!strcmp(cur_arg, "-s") || !strcmp(cur_arg, "--succinct"))
            succinct = true;
        else if (!strcmp(cur_arg, "-b") || !strcmp(cur_arg, "--both"))
            both = true;

        if (all) continue;

        if (!strcmp(cur_arg, "-acs") || !strcmp(cur_arg, "--access"))
            args |= ACCESS;
        else if (!strcmp(cur_arg, "-rc") || !strcmp(cur_arg, "--rankc"))
            args |= RANKC;
        else if (!strcmp(cur_arg, "-sc") || !strcmp(cur_arg, "--selectc"))
            args |= SELECTC;
        else if (!strcmp(cur_arg, "-a") || !strcmp(cur_arg, "--all"))
            all = true;
    }
}

void shuffle(vector<pair<size_t, size_t>> &V) {
    for (size_t i = V.size(); i > 0; i--) {
        size_t _i = rand() % i;
        pair<size_t, size_t> aux = V[_i];
        V[_i] = V[i - 1];
        V[i - 1] = aux;
    }
}

// {"access", {10000 : 54.32}}
map<char, map<unsigned long long, double>> wtres;
map<char, map<unsigned long long, double>> wtsres;

int main(int argc, char* argv[])
{
    if (argc <= 1)
        throw invalid_argument("No benchmark flags were provided, check this folder's README.md for more informations.");

    uint64_t args = 0;
    parse_args(argc, argv, args);
    srand(61027);
    
    if (verbose) cout << "The following tests were made in a order 3-8 BitVector such that each and every operation for index [0...n-1] was made in random order.\n\n";
    else if (!verbose && !both) cout << "\"Size\";\"Time\"\n";

    for (size_t order = 3; order < 9; order++)
    {
        string s; char cur_op;
        unsigned long long n = pow(10, order);

        vector<pair<size_t, size_t>> rand_index(n);
        for (size_t i = 0; i < n; i++)
        {
            s.push_back(rand() % 128);
            rand_index[i] = {i, rand() % 128};
        }
        shuffle(rand_index);
        
        WaveletTree wt(s);
        WaveletTreeSuccint wts(s);

        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        
        if (all || (args & RANKC))
        {
            cur_op = 'r';
            if (!succinct || both)
            {
                start = std::chrono::high_resolution_clock::now();
                for (pair<size_t, size_t> i : rand_index)
                {
                    wt.rankc(i.second, i.first);
                }
                end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, nano> elapsed_time{end - start};

                wtres[cur_op].insert({n, (elapsed_time.count() / n)});
            }
            if (succinct || both)
            {
                start = std::chrono::high_resolution_clock::now();
                for (pair<size_t, size_t> i : rand_index)
                {
                    wts.rank(i.second, i.first);
                }
                end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, nano> elapsed_time{end - start};

                wtsres[cur_op].insert({n, (elapsed_time.count()/n)});
            }
        }
        if (all || (args & SELECTC))
        {
            cur_op = 's';
            if(!succinct || both)
            {
                start = std::chrono::high_resolution_clock::now();
                for (pair<size_t, size_t> i : rand_index) {
                    wt.selectc(i.second, i.first, wt.getRoot());
                }
                end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, nano> elapsed_time{end - start};

                wtres[cur_op].insert({n, (elapsed_time.count()/n)});
            }
            if (succinct || both)
            {
                start = std::chrono::high_resolution_clock::now();
                for (pair<size_t, size_t> i : rand_index) {
                    wts.select(i.second, i.first);
                }
                end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, nano> elapsed_time{end - start};

                wtsres[cur_op].insert({n, (elapsed_time.count()/n)});
            }
        }
        if (all || (args & ACCESS))
        {
            cur_op = 'a';
            if(!succinct || both)
            {
                start = std::chrono::high_resolution_clock::now();
                for (pair<size_t, size_t> i : rand_index) {
                    wt.access(i.second);
                }
                end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, nano> elapsed_time{end - start};

                wtres[cur_op].insert({n, (elapsed_time.count()/n)});
            }
            if (succinct || both)
            {
                start = std::chrono::high_resolution_clock::now();
                for (pair<size_t, size_t> i : rand_index) {
                    wts.access(i.second);
                }
                end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double, nano> elapsed_time{end - start};

                wtsres[cur_op].insert({n, (elapsed_time.count()/n)});
            }
        }
    }
    if (!succinct)
    {
        for (auto el : wtres)
        {
            for (auto res : el.second)
            {
                if (verbose)
                {
                    if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTree rankc operation time = " << res.second << " ns\n";
                    else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTree access operation time = " << res.second << " ns\n";
                    else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTree selectc operation time = " << res.second << " ns\n";
                }
                else cout << res.first << ";" << res.second << "\n";
            }   
            cout << "\n";
        }
    }
    if (succinct || both) {
        for (auto el : wtsres)
        {
            for (auto res : el.second)
            {
                if (verbose)
                {
                    if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTreeSuccinct rank operation time = " << res.second << " ns\n";
                    else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTreeSuccinct access operation time = " << res.second << " ns\n";
                    else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTreeSuccinct select operation time = " << res.second << " ns\n";
                }
                else cout << res.first << ";" << res.second << "\n";
            }
            cout << "\n";
        }
    }
}