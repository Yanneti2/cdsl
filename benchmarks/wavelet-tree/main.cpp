#include "wavelet-tree_benchmark.hpp"
#include "interquartil-stats.hpp"
#include "wt_implicit.hpp"
#include "malloc_count.h"
#include "wt_succint.hpp"
#include "wt.hpp"

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cmath>

bool interquartile = false;
bool influenza = false;
bool implicit = false;
bool succinct = false;
bool proteins = false;
bool verbose = false;
bool selectc = false;
bool hcoding = false;
bool access = false;
bool memory = false;
bool omnia = false;
bool naive = false;
bool rankc = false;
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
        else if (!strcmp(cur_arg, "-mem") || !strcmp(cur_arg, "--memory"))
            memory = true;

        if (!strcmp(cur_arg, "-om") || !strcmp(cur_arg, "--omnia"))
            omnia = true;
        else if (!strcmp(cur_arg, "-s") || !strcmp(cur_arg, "--succinct"))
            succinct = true;
        else if (!strcmp(cur_arg, "-n") || !strcmp(cur_arg, "--naive"))
            naive = true;
        else if (!strcmp(cur_arg, "-im") || !strcmp(cur_arg, "--implicit"))
            implicit = true;

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

// {"access", {10000 : 54.32}}, BR: double or Statistiscs
map<char, map<unsigned long long, BenchmarkResult>> wtres;
map<char, map<unsigned long long, BenchmarkResult>> wtsres;
map<char, map<unsigned long long, BenchmarkResult>> wtires;

// build memory used per order
map<unsigned long long, size_t> wtb;
map<unsigned long long, size_t> wtsb;
map<unsigned long long, size_t> wtib;

int main(int argc, char* argv[])
{
    if (argc <= 1)
        throw invalid_argument("No benchmark flags were provided, check this folder's README.md for more informations.");

    uint64_t args = 0;
    parse_args(argc, argv, args);
    srand(61027);

    if (proteins || dna || influenza)
    {
        string f;

        if (proteins)
            f = read_file(rel_path + "proteins");
        else if (dna)
            f = read_file(rel_path + "dna");
        else if (influenza)
            f = read_file(rel_path + "influenza");

        if (verbose) cout << "Read file...\n";

        unsigned long long fsize = f.size();
        WaveletTree wt(f);

        if (verbose){
            map<char, BitVector*> encodings = get_huffman_encoding_dict(f);
            for (auto duo : encodings)
            {
                cout << duo.first << " - ";
                duo.second->print(verbose=false);
                cout << "\n";
            }
            cout << "\n\n";
        }

        double at = 0; long double rt = 0; long double st = 0;
        vector<pair<size_t, size_t>> indexes(OPERATIONS);

        for (size_t i = 0; i < OPERATIONS; i++)
        {
            size_t cindex = rand() % fsize;
            indexes[i] = {cindex, f[cindex]};
        }

        // RUN THE MEAN ALGORITHM
        at += run_mean_benchmark(wt, 'a', indexes);
        rt += run_mean_benchmark(wt, 'r', indexes);
        st += run_mean_benchmark(wt, 's', indexes);

        if (verbose)
        {
            cout << "For the dataset: ";

            if (dna) cout << "'dna' :\n";
            else if (influenza) cout << "'influenza' :\n";
            else if (proteins) cout << "'proteins' :\n";

            cout << "Average time for rank_c operation: " << rt << " ns\n" <<
                    "Average time for select_c operation: " << st << " ns\n" <<
                    "Average time for access operation: " << at << " ns\n\n";
        } else {
            cout << rt << "\n" << st << "\n" << at << "\n";
        }
        return 1;
    }

    if (!memory && (!interquartile && !mean))
        throw invalid_argument("No time calculation method provided, check this folder's README.md for more informations.");

    if (verbose) cout << "The following tests were made in a order 3-8 BitVector such that each and every operation for index [0...n-1] was made in random order.\n\n";
    // else if (!verbose && !((naive != succinct) != implicit)) cout << "\"Size\";\"Time\"\n";

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
        
        size_t mem_prev = malloc_count_peak();

        WaveletTree wt(s);

        if (memory && (naive || omnia))
        {
            size_t curr_mem = malloc_count_peak();
            wtb[order] = curr_mem - mem_prev;
        }

        malloc_count_reset_peak();
        mem_prev = malloc_count_peak();

        WaveletTreeSuccint wts(s);

        if (memory && (succinct || omnia))
        {
            size_t curr_mem = malloc_count_peak();
            wtsb[order] = curr_mem - mem_prev;
        }

        malloc_count_reset_peak();
        mem_prev = malloc_count_peak();

        WaveletTreeImplicit wti(s);

        if (memory && (implicit || omnia))
        {
            size_t curr_mem = malloc_count_peak();
            wtib[order] = curr_mem - mem_prev;
        }

        if (verbose)
        {
            map<char, BitVector*> encodings = get_huffman_encoding_dict(s);
            for (auto duo : encodings)
            {
                cout << duo.first << " - ";
                duo.second->print(verbose=false);
                cout << "\n";
            }
            cout << "\n\n";
        }

        vector<char> operations;

        if (all || rankc) operations.push_back('r');
        if (all || selectc) operations.push_back('s');
        if (all || access) operations.push_back('a');

        if (!memory) // remove after benchmarking
        {
            for (char cur_op : operations)
            {
                if (interquartile)
                {
                    if (naive || omnia)
                        wtres[cur_op][n].stats = run_interquartile_benchmark(config, wt, cur_op, rand_index);
                    if (succinct || omnia)
                        wtsres[cur_op][n].stats = run_interquartile_benchmark(config, wts, cur_op, rand_index);
                    if (implicit || omnia)
                        wtires[cur_op][n].stats = run_interquartile_benchmark(config, wti, cur_op, rand_index);
                }
                else if (mean)
                {
                    if (naive || omnia)
                        wtres[cur_op][n].time = run_mean_benchmark(wt, cur_op, rand_index);
                    if (succinct || omnia)
                        wtsres[cur_op][n].time = run_mean_benchmark(wts, cur_op, rand_index);
                    if (implicit || omnia)
                        wtires[cur_op][n].time = run_mean_benchmark(wti, cur_op, rand_index);
                }
            }
        }
    }
    if (verbose) cout << "\n\n";
    if (memory)
    {
        if (naive || omnia)
        {
            cout << "\"Size\";\"Memory\"\n";
            for (auto duo : wtb)
                cout << duo.first << ";" << duo.second << endl;
            cout << "\n\n";
        }
        if (succinct || omnia)
        {
            cout << "\"Size\";\"Memory\"\n";
            for (auto duo : wtsb)
                cout << duo.first << ";" << duo.second << endl;
            cout << "\n\n";
        }
        if (implicit || omnia)
        {
            cout << "\"Size\";\"Memory\"\n";
            for (auto duo : wtib)
                cout << duo.first << ";" << duo.second << endl;
            cout << "\n\n";
        }
    }
    if (naive || omnia || (!succinct && !implicit))
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
    if (succinct || omnia) {
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
    if (implicit || omnia) {
        for (auto el : wtires)
        {
            for (auto res : el.second)
            {
                if (interquartile)
                {
                    if (verbose)
                    {
                        if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTreeImplicit rankc operation time = " << res.second.stats->median << " ns\n";
                        else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTreeImplicit access operation time = " << res.second.stats->median << " ns\n";
                        else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTreeImplicit selectc operation time = " << res.second.stats->median << " ns\n";
                        cout << "q1 : " << res.second.stats->q1 << "\n q3 : " << res.second.stats->q3 << "\n iqr : " << res.second.stats->iqr << "\n lower_bound : " << res.second.stats->lower_bound << "\n upper_bound : " << res.second.stats->upper_bound << "\n mean : " << res.second.stats->mean << "\n standard deviation : "  << res.second.stats->standard_deviation << "\n coefficient variation : " << res.second.stats->coefficient_variation << "\n\n"; 
                    }
                    else cout << res.first << ";" << res.second.stats->median << "\n";
                }
                else if (mean)
                {
                    if (verbose)
                    {
                        if (el.first == 'r') cout << "For order " << res.first << " Average WaveletTreeImplicit rankc operation time = " << res.second.time << " ns\n";
                        else if (el.first == 'a') cout << "For order " << res.first << " Average WaveletTreeImplicit access operation time = " << res.second.time << " ns\n";
                        else if (el.first == 's') cout << "For order " << res.first << " Average WaveletTreeImplicit selectc operation time = " << res.second.time << " ns\n";
                    }
                    else cout << res.first << ";" << res.second.time << "\n";
                }
            }
            cout << "\n";
        }
    }
    return 1;
}