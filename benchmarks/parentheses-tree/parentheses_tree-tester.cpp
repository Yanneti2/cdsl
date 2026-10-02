#include "parentheses_tree.hpp"
#include "parentheses_tree_rmmq.hpp"

#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <random>
#include <string>
#include <chrono>

using namespace std;

#define ULL unsigned long long

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

void shuffle(vector<ULL> &v) {
    // findex = size -1 (ok), lindex = 0 (1-1)
    for (int i = v.size() - 1; i > 0; i--) {
        ULL aux = v[i - 1];
        ULL rindex = rand() % i;
        v[i - 1] = v[rindex];
        v[rindex] = aux;
    }
}

int main(int argc, char *argv[])
{
    /*
        Run this test until the interquartil metric is acceptable...

        Create a vector response with the time durations and than sort
        or
        Include the value already sorted in O(logn)
    */

    ULL operations = 100000;

    // Control variables
    bool verbose = false;
    bool all = false;
    srand(time(0));

    // Operations performed
    
    // bool children = false;
    // bool builders = false;
    bool enclose = false;
    // bool subtree = false;
    // bool parent = false;
    // bool valid = false;
    bool close = false;
    // bool isl = false;
    // bool dpn = false;
    // bool lr = false;
    // bool ln = false;
    // bool ls = false;
    // bool cr = false;
    // bool lc = false;
    // bool ia = false;

    bool enclosestring = true;
    bool closestring = true;

    for(int i = 0; i < argc; i++)
    {
        const char* curr_arg = argv[i];

        if (strcmp(curr_arg, "-v") == 0 || strcmp(curr_arg,"--verbose") == 0) verbose = true;

        else if (strcmp(curr_arg, "-a") == 0 || strcmp(curr_arg, "--all") == 0)
            all = true;

        if (all) continue;

        // if (strcmp(curr_arg, "-c") || strcmp(curr_arg, "--constructors") == 0  ||
        //     strcmp(curr_arg, "-b") == 0 || strcmp(curr_arg, "--builders") == 0)
        //     builders = true;
        // else if (strcmp(curr_arg, "-isbp") == 0 || strcmp(curr_arg, "--valid") == 0)
        //     valid = true;
        else if (strcmp(curr_arg, "-en") == 0 || strcmp(curr_arg, "--enclose") == 0)
            enclose = true;
        // else if (strcmp(curr_arg, "-p") == 0 || strcmp(curr_arg, "--parent") == 0)
        //     parent = true;
        // else if (strcmp(curr_arg, "-isl") == 0 || strcmp(curr_arg, "--is_leaf") == 0)
        //     isl = true;
        // else if (strcmp(curr_arg, "-sbt") == 0 || strcmp(curr_arg, "--subtree") == 0)
        //     subtree = true;
        // else if (strcmp(curr_arg, "-lr") == 0 || strcmp(curr_arg, "--leafrank") == 0)
        //     lr = true;
        // else if (strcmp(curr_arg, "-ln") == 0 || strcmp(curr_arg, "--leafnum") == 0)
        //     ln = true;
        // else if (strcmp(curr_arg, "-ls") == 0 || strcmp(curr_arg, "--leafselect") == 0)
        //     ls = true;
        // else if (strcmp(curr_arg, "-ch") == 0 || strcmp(curr_arg, "--children") == 0)
        //     children = true;
        // else if (strcmp(curr_arg, "-cr") == 0 || strcmp(curr_arg, "--childrank") == 0)
        //     cr = true;
        // else if (strcmp(curr_arg, "-lc") == 0 || strcmp(curr_arg, "--lchild") == 0)
        //     lc = true;
        // else if (strcmp(curr_arg, "-ia") == 0 || strcmp(curr_arg, "--isancestor") == 0)
        //     ia = true;
        else if (strcmp(curr_arg, "-cl") == 0 || strcmp(curr_arg, "--close") == 0)
            close = true;
        // else if (strcmp(curr_arg, "-dpn") == 0 || strcmp(curr_arg, "--deepestnode") == 0)
        //     dpn = true;
    }

    if (verbose) cout << "ALL THE FOLLOWING TESTS WERE COMPUTED FROM ORDER 3 TO ORDER 9 BITVECTOR SIZE\n\n";

    for(int order = 3; order <= 9; order++)
    {
        ULL order_num = 1;
        for(int i = 0; i < order; i++) order_num *= 10;

        string valid_pt = "";
        generate_pt(valid_pt, order_num);

        ParenthesesTree *PT = new ParenthesesTree(valid_pt);
        ParenthesesTreeRMMQ *PTRMMQ = new ParenthesesTreeRMMQ(*PT);

        vector<ULL> rand_indexes(order_num);
        for(int i = 0; i < order_num; i++)
        {
            rand_indexes[i] = i;
        }
        shuffle(rand_indexes);

        if (all || enclose)
        {
            if (verbose && enclosestring)
            {
                cout << "This test consist in the backwards search operation in each and every position [0, 1,..., order_num-1] of the current PT BitVector in random order for naive and rmMq balanced parentheses tree implementation.\n\n";
                enclosestring = false;
            }

            chrono::high_resolution_clock::time_point start;
            chrono::high_resolution_clock::time_point end;

            // start = chrono::high_resolution_clock::now();
            // for(ULL i : rand_indexes)
            // {
            //     PT->enclose(i);
            // }
            // end = chrono::high_resolution_clock::now();
            
            // std::chrono::duration<double, nano> elapsed_times{end - start};
            // if (verbose) cout << "Size: " << order << " average naive enclose operation time: " << elapsed_times.count() / order_num << " ns\n\n";
            // else cout << order << ";" << elapsed_times.count() / order_num << "\n";

            start = chrono::high_resolution_clock::now();
            for(ULL i : rand_indexes)
            {
                PTRMMQ->enclose(i);
            }
            end = chrono::high_resolution_clock::now();

            // elapsed_times = end - start;
            std::chrono::duration<double, nano> elapsed_times{end - start};

            if (verbose) cout << "Size: " << order << " average rmMq enclose operation time: " << elapsed_times.count() / order_num << " ns\n\n";
            else cout << order << ";" << elapsed_times.count() / order_num << "\n";
        }
        else if (all || close)
        {
            if (verbose && closestring)
            {
                cout << "This test consist in the forward search operation for all positions of size order of a randomly created PT BitVector in random order for naive and rmMq balanced parentheses tree implementation.\n";
                closestring = false;
            }

            chrono::high_resolution_clock::time_point start;
            chrono::high_resolution_clock::time_point end;

            // vector<ULL>Nresults(order_num + 100000);
            // vector<ULL>Rresults(order_num + 100000);
            
            // start = chrono::high_resolution_clock::now();
            // for (ULL i : rand_indexes){                
            //     PT->close(i);
            // }
            // end = chrono::high_resolution_clock::now();

            // std::chrono::duration<double, nano> elapsed_times{end - start};

            // if (verbose) cout << "order: " << order << "Naive close average time per operation: " << elapsed_times.count()/order << "ns\n";
            // else cout << order << ";" << elapsed_times.count() / order << "\n\n";

            BitVectorJ Bv = PT->getBv();
            double vsize = Bv.size();
            vector<ULL> cindex(vsize); // close indexes
            for (ULL vindex: rand_indexes){ 
                if (Bv[vindex] == 0) cindex.push_back(vindex);
            }

            start = chrono::high_resolution_clock::now();
            for (ULL i : cindex){
                PTRMMQ->close(i);
            }
            end = chrono::high_resolution_clock::now();

            std::chrono::duration<double, nano> elapsed_times{end - start};

            // elapsed_times = end - start;

            if (verbose) cout << "order: " << order << "rmMq close average time per operation: " << elapsed_times.count() / vsize << "ns\n\n";
            else cout << order << ";" << elapsed_times.count() / vsize << "\n";

            // for (ULL i = 0; i < order_num; i++) assert(Nresults[i] == Rresults[i]);
        }
    }
    return 1;
}