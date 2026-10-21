#include "wavelet-tree_benchmark.hpp"
#include "interquartil-stats.hpp"
#include "wt_succint.hpp"
#include "wt.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cmath>

bool interquartile = false;
bool influenza = false;
bool succinct = false;
bool proteins = false;
bool verbose = false;
bool selectc = false;
bool access = false;
bool rankc = false;
bool both = false;
bool mean = false;
bool all = false;
bool dna = false;

StatsConfig config; // can modify

string rel_path = "../../../../";

using namespace std;

// interquartil or simple median
void parse_args(int argc, char* argv[], uint64_t &args) {
    for (int i = 1; i < argc; i++)
    {
        const char* cur_arg = argv[i];

        if (!strcmp(cur_arg, "-v") || !strcmp(cur_arg, "--verbose"))
            verbose = true;

        // make so its necessary to specify the implementation (Explicit, implicit, succint) LATER!!!
        if (!strcmp(cur_arg, "-s") || !strcmp(cur_arg, "--succinct"))
            succinct = true;
        else if (!strcmp(cur_arg, "-b") || !strcmp(cur_arg, "--both"))
            both = true;

        if (!strcmp(cur_arg, "-i") || !strcmp(cur_arg, "--interquartile"))
            interquartile = true;
        else if (!strcmp(cur_arg, "-m") || !strcmp(cur_arg, "--mean"))
            mean = true;

        if (!strcmp(cur_arg, "--dna"))
            dna = true;
        else if (!strcmp(cur_arg, "--influenza"))
            influenza = true;
        else if (!strcmp(cur_arg, "--proteins"))
            proteins = true;

        if (all) continue;

        if (!strcmp(cur_arg, "-acs") || !strcmp(cur_arg, "--access"))
            access = true;
        else if (!strcmp(cur_arg, "-rc") || !strcmp(cur_arg, "--rankc"))
            rankc = true;
        else if (!strcmp(cur_arg, "-sc") || !strcmp(cur_arg, "--selectc"))
            selectc = true;
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
map<char, map<unsigned long long, BenchmarkResult>> wtres;
map<char, map<unsigned long long, BenchmarkResult>> wtsres;

int main(int argc, char* argv[])
{
    if (argc <= 1)
        throw invalid_argument("No benchmark flags were provided, check this folder's README.md for more informations.");

    uint64_t args = 0;
    parse_args(argc, argv, args);
    srand(61027);

    if (!interquartile && !mean)
        throw invalid_argument("No time calculation method provided, check this folder's README.md for more informations.");

    if (verbose) cout << "The following tests were made in a order 3-8 BitVector such that each and every operation for index [0...n-1] was made in random order.\n\n";
    else if (!verbose && !both) cout << "\"Size\";\"Time\"\n";

    if (proteins || dna || influenza)
    {
        if (proteins)
        {
            string f = read_file(rel_path + "proteins");
            cout << f;
        }
        else if (dna)
        {
            string f = read_file(rel_path + "dna");
            cout << f;
        }
        else if (influenza)
        {
            string f = read_file(rel_path + "influenza");
            cout << f;
        }
        return 1;
    }

    for (size_t order = 3; order < 9; order++)
    {
        string s;
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

        vector<char> operations;

        if (all || rankc) operations.push_back('r');
        else if (all || selectc) operations.push_back('s');
        else if (all || access) operations.push_back('a');

        for (char cur_op : operations)
        {
            if (interquartile)
            {
                if (!succinct || both)
                    wtres[cur_op][n].stats = run_interquartile_benchmark(config, wt, order, cur_op, rand_index);
                else
                    wtsres[cur_op][n].stats = run_interquartile_benchmark(config, wts, order, cur_op, rand_index);
            }
            else if (mean)
            {
                if (!succinct || both)
                    wtres[cur_op][n].time = run_mean_benchmark(wt, order, cur_op, rand_index);
                else
                    wtsres[cur_op][n].time = run_mean_benchmark(wts, order, cur_op, rand_index);
            }
        }
    }
    if (verbose) cout << "\n\n";
    if (!succinct)
    {
        for (auto el : wtres)
        {
            for (auto res : el.second)
            {
                if (interquartile)
                {
                    if (verbose)
                    {
                        if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTree rankc operation time = " << res.second.stats->median << " ns\n";
                        else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTree access operation time = " << res.second.stats->median << " ns\n";
                        else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTree selectc operation time = " << res.second.stats->median << " ns\n";
                        cout << "q1 : " << res.second.stats->q1 << "\n q3 : " << res.second.stats->q3 << "\n iqr : " << res.second.stats->iqr << "\n lower_bound : " << res.second.stats->lower_bound << "\n upper_bound : " << res.second.stats->upper_bound << "\n mean : " << res.second.stats->mean << "\n standard deviation : "  << res.second.stats->standard_deviation << "\n coefficient variation : " << res.second.stats->coefficient_variation << "\n\n"; 
                    }
                    else cout << res.first << ";" << res.second.stats->median << "\n";
                }
                else if (mean)
                {
                    if (verbose)
                    {
                        if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTree rankc operation time = " << res.second.time << " ns\n";
                        else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTree access operation time = " << res.second.time << " ns\n";
                        else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTree selectc operation time = " << res.second.time << " ns\n";
                    }
                    else cout << res.first << ";" << res.second.time << "\n";
                }
            }   
            cout << "\n";
        }
    }
    if (succinct || both) {
        for (auto el : wtsres)
        {
            for (auto res : el.second)
            {
                if (interquartile)
                {
                    if (verbose)
                    {
                        if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTreeSuccinct rankc operation time = " << res.second.stats->median << " ns\n";
                        else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTreeSuccinct access operation time = " << res.second.stats->median << " ns\n";
                        else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTreeSuccinct selectc operation time = " << res.second.stats->median << " ns\n";
                        cout << "q1 : " << res.second.stats->q1 << "\n q3 : " << res.second.stats->q3 << "\n iqr : " << res.second.stats->iqr << "\n lower_bound : " << res.second.stats->lower_bound << "\n upper_bound : " << res.second.stats->upper_bound << "\n mean : " << res.second.stats->mean << "\n standard deviation : "  << res.second.stats->standard_deviation << "\n coefficient variation : " << res.second.stats->coefficient_variation << "\n\n"; 
                    }
                    else cout << res.first << ";" << res.second.stats->median << "\n";
                }
                else if (mean)
                {
                    if (verbose)
                    {
                        if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTreeSuccinct rankc operation time = " << res.second.time << " ns\n";
                        else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTreeSuccinct access operation time = " << res.second.time << " ns\n";
                        else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTreeSuccinct selectc operation time = " << res.second.time << " ns\n";
                    }
                    else cout << res.first << ";" << res.second.time << "\n";
                }
            }
            cout << "\n";
        }
    }
    return 1;
}