#include "parentheses-tree_benchmark.hpp"
#include "interquartil-stats.hpp"
#include "general_tree.hpp"

#include <bits/stdc++.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cassert>
#include <random>
#include <string>

using namespace std;

#define ULL unsigned long long

// Control variables :
bool comparison = false;
bool verbose = false;
bool naive = false;
int completion = 0;
bool all = false;

// Operations performed :
bool closeop = false;
bool enclose = false;

// Verbose String control :
map<string, string> vstrings = 
{
    {"enclose" , "The following test results consist in the backwards search operation\n\n"},
    {"close" , "This test consist in the forward search operation for all positions of size order of a randomly created PT BitVector in random order.\n\n"},
};

map<string, map<ULL, Statistics>> results;  // rmmq
map<string, map<ULL, double>> nresults; // naive

void generate_pt(string& s, int n)
{
    int open = 0;
    int close = 0;

    bool canOpen = false;
    bool canClose = false;

    while(true)
    {
        if (open == (n/2) && close == (n/2)) return;

        if (open == (n/2)) canOpen = false;
        else canOpen  = open < (n/2);
        
        canClose = close < open  - 1;

        if (canOpen && canClose){
            if (rand() % 2 == 0)
            {
                s += ')';
                close++;
            }
            else
            {
                s += '(';
                open++;
            } 
        }
        else if (canOpen)
        {
            s += '(';
            open++;
        }
        else
        {
            s += ')';
            close++;
        }
    }
}

void shuffle(vector<ULL> &v)
{
    for (int i = v.size() - 1; i > 0; i--)
    {
        ULL aux = v[i - 1];
        ULL rindex = rand() % i;
        v[i - 1] = v[rindex];
        v[rindex] = aux;
    }
}

void parse_arguments(int argc, char *argv[])
{
    for(int i = 1; i < argc; i++)
    {
        const char* curr_arg = argv[i];

        if (strcmp(curr_arg, "-v") == 0 || strcmp(curr_arg,"--verbose") == 0)
            verbose = true;
        else if (strcmp(curr_arg, "-n") == 0 || strcmp(curr_arg, "--naive") == 0)
            naive = true;
        else if (strcmp(curr_arg, "-cmp") == 0 || strcmp(curr_arg, "--compare") == 0)
            comparison = true;
        else if (strcmp(curr_arg, "-a") == 0 || strcmp(curr_arg, "--all") == 0)
            all = true;

        if (all) continue;

        if (strcmp(curr_arg, "-en") == 0 || strcmp(curr_arg, "--enclose") == 0)
            enclose = true;
        else if (strcmp(curr_arg, "-cl") == 0 || strcmp(curr_arg, "--close") == 0)
            closeop = true;
    }
}

Gtree *rand_tree(ULL n) {
    Gtree *GT = new Gtree();
    vector<Gtree::gNode *> nodes;
    nodes.push_back(GT->getRoot());

    for (ULL i = 0; i < n - 1; i++) {
        Gtree::gNode *node = nodes[rand() % nodes.size()];
        Gtree::gNode *new_node = GT->create_node();
        GT->add_node(node, new_node);
        nodes.push_back(new_node);
    }
    return GT;
}

int main(int argc, char *argv[])
{
    if (argc == 1) throw invalid_argument("No benchmark flags were provided, check this folder's README.md for more informations.");

    parse_arguments(argc, argv);

    srand(1791409268);

    if (verbose)
    {
        cout << "All the following tests were computed on order 4-8 BitVector size for rmMq operations and order 4-6 BitVector size for naive operations :" << endl;
        cout << "[";
    }

    for(int order = 4; order <= 8; order++)
    {
        ULL order_num = 1;
        for(int i = 0; i < order; i++) order_num *= 10;

        string cur_op;

        Gtree* gt = rand_tree(order_num);

        // string valid_pt = "";
        // generate_pt(valid_pt, order_num);

        ParenthesesTree *PT = new ParenthesesTree(*gt);
        BitVectorJ Bv = PT->getBv();
        ParenthesesTreeRMMQ *PTRMMQ = new ParenthesesTreeRMMQ(*PT);

        vector<ULL> rand_indexes(order_num);
        for(ULL i = 0; i < order_num; i++)
        {
            rand_indexes[i] = i;
        }
        shuffle(rand_indexes);

        if (all || enclose)
        {
            cur_op = "enclose";
            vector<ULL> endex;
            for (ULL i : rand_indexes) if (i!=0 && Bv[i] == 1) endex.push_back(i);

            if (order <= 6 && (naive || comparison))
            {
                vector<double> nsamples;

                for(int i = 0; i < MIN_WARMUP_SAMPLES; i++)
                    nsamples.push_back(run_naive_benchmark(*PT,order,cur_op,endex));

                nresults["enclose"].insert({order_num, get_median(nsamples)});
                
                if (verbose) cout << "#";
            }
            if (!naive || comparison)
            {
                for (int w = 0; w < MIN_WARMUP_SAMPLES; w++)
                {
                    run_rmmq_benchmark(*PTRMMQ,order,cur_op,endex);
                }
                int t = 0;

                do
                {
                    vector<double> samples;

                    for(int i = 0; i < MIN_SAMPLES; i++)
                        samples.push_back(run_rmmq_benchmark(*PTRMMQ,order,cur_op,endex));

                    if (samples.empty()){
                        t += 1;
                        continue;
                    }

                    Statistics s = calculate_interquartil(samples);
                    if (s.is_valid) {
                        results["enclose"].insert({order_num, s});
                        break;
                    }
                    t += 1;
                } while (t < MAX_ATTEMPTS);

                if (t == MAX_ATTEMPTS)
                    throw invalid_argument("RmMq enclose operation failed fot interquartile metrics."); 
            }
            if (verbose) cout << "#";
        }
        if (all || closeop)
        {
            cur_op = "close";

            double vsize = Bv.size();
            vector<ULL> cindex; // '(' indexes
            cindex.reserve(vsize);

            for (ULL vindex: rand_indexes) {if (Bv[vindex] == 1) cindex.push_back(vindex);}

            if (order <= 6 && (naive || comparison))
            {
                vector<double> nsamples;

                for(int i = 0; i < MIN_WARMUP_SAMPLES; i++)
                    nsamples.push_back(run_naive_benchmark(*PT,order,cur_op,cindex));

                nresults["close"].insert({order_num, get_median(nsamples)});

                if (verbose) cout << "#";
            }
            if (!naive || comparison)
            {
                for (int w = 0; w < MIN_WARMUP_SAMPLES; w++)
                {
                    run_rmmq_benchmark(*PTRMMQ,order,cur_op,cindex);
                }
                int t = 0;

                do
                {
                    vector<double> samples;

                    for(int i = 0; i < MIN_SAMPLES; i++)
                        samples.push_back(run_rmmq_benchmark(*PTRMMQ,order,cur_op,cindex));

                    if (samples.empty()){
                        t += 1;
                        continue;
                    }

                    Statistics s = calculate_interquartil(samples);
                    if (s.is_valid) {
                        results["close"].insert({order_num, s});
                        break;
                    }
                    t += 1;
                } while (t < MAX_ATTEMPTS);

                if (t == MAX_ATTEMPTS)
                    throw invalid_argument("RmMq enclose operation failed fot interquartile metrics."); 
            }
            if (verbose) cout << "#";
        }
        // delete gt;
        delete PT;
        delete PTRMMQ;
    }
    if (verbose) cout << "]\n\n";
    else cout << "\"Order\"" << ";" << "\"Time\"" << endl;

    if (!naive || comparison)
    {
        for (auto el : results)
        {
            if (verbose) cout << vstrings[el.first];
            for (auto res : el.second)
            {
                if(verbose) cout << "RmMq operations for order " << res.first << " :" << endl;
                else cout << res.first;

                if (verbose){
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
                } else {
                    cout << ';' << res.second.median;
                }
                cout << "\n";
            }
            cout << "\n";
        }
    }
    if (naive || comparison)
    {
        for (auto el: nresults)
        {
            if (verbose) cout << vstrings[el.first];

            for (auto res: el.second)
                cout << res.first <<  ';' << nresults[el.first][res.first] << "\n";
            cout << "\n";
        }
        cout << "\n\n";
    }
    return 0;
}