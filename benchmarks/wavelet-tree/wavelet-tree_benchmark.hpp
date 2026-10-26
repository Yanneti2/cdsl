#include "interquartil-stats.hpp"
#include "wtInterface.hpp"
#include "bitvector.hpp"

#include <string>
#include <map>

#define ULL unsigned long long

using namespace std;

#ifndef WTbenchmark
#define WTbenchmark

double run_benchmark(WTinterface &wt, char operation, vector<pair<size_t, size_t>> & indexes);
Statistics run_interquartile_benchmark(StatsConfig config, WTinterface &wt, char operation, vector<pair<size_t, size_t>> & indexes);
double run_mean_benchmark(WTinterface &wt, char operation, vector<pair<size_t, size_t>> & indexes);
string read_file(string path);
map<char,BitVector*> get_huffman_encoding_dict(string original);

#endif