#include "wavelet-tree_benchmark.hpp"

#include <chrono>
#include <iostream>

double run_wt_benchmark(WaveletTree &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes)
{
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    long ct = 0;
    if (operation == 'r')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wt.rankc(i.second, i.first);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    else if (operation == 's')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wt.selectc(i.second, i.first, wt.getRoot());
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

    return (elapsed_time.count() / indexes.size());
}

double run_wts_benchmark(WaveletTreeSuccint &wts, int order, char operation, vector<pair<size_t, size_t>> & indexes)
{
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    long ct = 0;

    if (operation == 'r')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wts.rank(i.second, i.first);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    else if (operation == 's')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes)
        {
            ct += wts.select(i.second, i.first);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    else if (operation == 'a')
    {
        start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : indexes) 
        {
            ct += wts.access(i.second);
        }
        end = std::chrono::high_resolution_clock::now();
    }
    std::chrono::duration<double, nano> elapsed_time{end - start};

    return (elapsed_time.count() / indexes.size());
}