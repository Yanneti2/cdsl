#include "parentheses_tree_rmmq.hpp"
#include "parentheses_tree.hpp"

#define ULL unsigned long long

using namespace std;

#ifndef PTbenchmark
#define PTbenchmark

double run_rmmq_benchmark(ParenthesesTreeRMMQ &PTRMMQ, int order, string operation, vector<ULL>& indexes);
double run_naive_benchmark(ParenthesesTree& PT, int order, string operation, vector<ULL>& indexes);                    

#endif