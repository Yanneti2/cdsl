#include "wavelet-tree_benchmark.hpp"
#include "interquartil-stats.hpp"
#include "wtInterface.hpp"

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

double run_benchmark(WTinterface &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes)
{
    if (indexes.empty())
        throw std::invalid_argument("Cannot benchmark an empty index vector.");

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    long ct = 0;
    if (operation == 'r')
    {   
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wt.rank(i.second, i.first);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    else if (operation == 's')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wt.select(i.second, i.first);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    else if (operation == 'a')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wt.access(i.second);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    std::chrono::duration<double, nano> elapsed_time{end - start};

    cerr << ct << endl;

    return (elapsed_time.count() / indexes.size());
}

Statistics run_interquartile_benchmark(StatsConfig config, WTinterface &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes)
{
    for (int i = 0; i < config.min_warmup_samples; i++)
        run_benchmark(wt, order, operation, indexes);

    int t = 0;
    do
    {
        vector<double> samples;

        for (int i = 0; i < config.min_samples; i++)
            samples.push_back(run_benchmark(wt, order, operation, indexes));

        if (samples.empty()){
            t += 1;
            continue;
        }

        Statistics s = calculate_interquartil(samples, config);
        if (s.is_valid) return s;

        t += 1;
    } while (t < config.max_attempts);

    if (t == config.max_attempts)
    {
        if (operation == 'r'){
            throw invalid_argument("WT rankc operation failed for interquartile metrics.");
            // else
            //     throw invalid_argument("WTS rank operation failed for interquartile metrics.");
        }
        else if (operation == 's'){
            throw invalid_argument("WT selectc operation failed for interquartile metrics.");
            // else
            //     throw invalid_argument("WTS select operation failed for interquartile metrics.");
        }
        else if (operation == 'a'){
            throw invalid_argument("WT access operation failed for interquartile metrics.");
            // else
            //     throw invalid_argument("WTS access operation failed for interquartile metrics.");
        }
        else
            terminate();
    }
}

double run_mean_benchmark(WTinterface &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes)
{
    double at = 0;
    for (int i = 0; i < SIMPLE_REPETITIONS; i++)
        at += run_benchmark(wt, order, operation, indexes);
    return (at/SIMPLE_REPETITIONS);
};

string read_file(string path)
{
    ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return "";
    }
    string content((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());

    file.close();

    return content;
}