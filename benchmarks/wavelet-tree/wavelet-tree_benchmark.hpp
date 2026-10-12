#include "wt_succint.hpp"
#include "wt.hpp"

#define ULL unsigned long long

using namespace std;

#ifndef WTbenchmark
#define WTbenchmark

double run_wt_benchmark(WaveletTree &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes);
double run_wts_benchmark(WaveletTreeSuccint &wts, int order, char operation, vector<pair<size_t, size_t>> & indexes);

#endif