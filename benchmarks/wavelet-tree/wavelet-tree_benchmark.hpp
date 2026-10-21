#include "interquartil-stats.hpp"
#include "wtInterface.hpp"

#include <string>

#define ULL unsigned long long

using namespace std;

#ifndef WTbenchmark
#define WTbenchmark

double run_benchmark(WTinterface &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes);
Statistics run_interquartile_benchmark(StatsConfig config, WTinterface &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes);
double run_mean_benchmark(WTinterface &wt, int order, char operation, vector<pair<size_t, size_t>> & indexes);
string read_file(string path);

#endif