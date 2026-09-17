#include "parentheses_tree.hpp"

#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <random>
#include <string>
#include <chrono>

using namespace std;

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
        
        canClose = close < open;

        if (canOpen && canClose){
            if ((rand() % 2) == 0)
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

int main(int argc, char *argv[])
{
    /*
        Run this test until the interquartil metric is acceptable...

        Create a vector response with the time durations and than sort
        or
        Include the value already sorted in O(logn)
    */

    unsigned long long operations = 100000;

    // Control variables
    bool verbose = false;
    bool all = false;
    srand(time(0));

    // Operations performed
    bool children = false;
    bool builders = false;
    bool enclose = false;
    bool subtree = false;
    bool parent = false;
    bool valid = false;
    bool close = false;
    bool isl = false;
    bool dpn = false;
    bool lr = false;
    bool ln = false;
    bool ls = false;
    bool cr = false;
    bool lc = false;
    bool ia = false;

    bool enclosestring = true;
    bool parentstring = true;

    for(int i = 0; i < argc; i++)
    {
        const char* curr_arg = argv[i];

        if (strcmp(curr_arg, "-v") == 0 || strcmp(curr_arg,"--verbose") == 0) verbose = true;

        else if (strcmp(curr_arg, "-a") == 0 || strcmp(curr_arg, "--all") == 0)
            all = true;

        if (all) continue;

        if (strcmp(curr_arg, "-c") || strcmp(curr_arg, "--constructors") == 0  ||
            strcmp(curr_arg, "-b") == 0 || strcmp(curr_arg, "--builders") == 0)
            builders = true;
        else if (strcmp(curr_arg, "-isbp") == 0 || strcmp(curr_arg, "--valid") == 0)
            valid = true;
        else if (strcmp(curr_arg, "-en") == 0 || strcmp(curr_arg, "--enclose") == 0)
            enclose = true;
        else if (strcmp(curr_arg, "-p") == 0 || strcmp(curr_arg, "--parent") == 0)
            parent = true;
        else if (strcmp(curr_arg, "-isl") == 0 || strcmp(curr_arg, "--is_leaf") == 0)
            isl = true;
        else if (strcmp(curr_arg, "-sbt") == 0 || strcmp(curr_arg, "--subtree") == 0)
            subtree = true;
        else if (strcmp(curr_arg, "-lr") == 0 || strcmp(curr_arg, "--leafrank") == 0)
            lr = true;
        else if (strcmp(curr_arg, "-ln") == 0 || strcmp(curr_arg, "--leafnum") == 0)
            ln = true;
        else if (strcmp(curr_arg, "-ls") == 0 || strcmp(curr_arg, "--leafselect") == 0)
            ls = true;
        else if (strcmp(curr_arg, "-ch") == 0 || strcmp(curr_arg, "--children") == 0)
            children = true;
        else if (strcmp(curr_arg, "-cr") == 0 || strcmp(curr_arg, "--childrank") == 0)
            cr = true;
        else if (strcmp(curr_arg, "-lc") == 0 || strcmp(curr_arg, "--lchild") == 0)
            lc = true;
        else if (strcmp(curr_arg, "-ia") == 0 || strcmp(curr_arg, "--isancestor") == 0)
            ia = true;
        else if (strcmp(curr_arg, "-cl") == 0 || strcmp(curr_arg, "--close") == 0)
            close = true;
        else if (strcmp(curr_arg, "-dpn") == 0 || strcmp(curr_arg, "--deepestnode") == 0)
            dpn = true;
    }

    if (verbose) cout << "ALL THE FOLLOWING TESTS WERE COMPUTED FROM ORDER 3 TO ORDER 9 BITVECTOR SIZE\n\n";

    for(int order = 3; order <= 9; order++)
    {
        vector<string> vs; // array de valid bp strings
        vector<ParenthesesTree> vpt;

        unsigned long long order_num = 1;
        for(int i = 0; i < order; i++) order_num *= 10;

        string valid_pt = "";
        generate_pt(valid_pt, order_num);
        // cout << valid_pt << "\n";

        ParenthesesTree PT = ParenthesesTree(valid_pt);

        // for(int i = 0; i <= 1000000; i++)
        // {
        //     string valid_pt = "";
        //     generate_pt(valid_pt, order_num);
        //     vs.push_back(valid_pt);
        //     vpt.push_back(ParenthesesTree(valid_pt));
        // }

        // if (builders || all)
        // {
        //     if (verbose) cout << "This Test consist in the average time of construction of 1M ParenthesesTree objects from random valid PT strings with size == current order\n\n";
            
        //     chrono::high_resolution_clock::time_point start;
        //     chrono::high_resolution_clock::time_point end;

        //     start = chrono::high_resolution_clock::now();
        //     for(string cs: vs) ParenthesesTree pt = ParenthesesTree(cs);
        //     end = chrono::high_resolution_clock::now();

        //     std::chrono::duration<double, nano> elapsed_times{end - start};

        //     if (verbose) { 
        //         cout << "Size: " << order << endl;
        //         cout << "Time per string build: " << elapsed_times.count() / (long double) 1000000 << " ns\n";
        //     } else {
        //         cout << order << ";" << elapsed_times.count() / (long double) 1000000 << "\n";
        //     }
        // }
        // else if (valid || all)
        // {
        //     if (verbose)
        //     {
        //         cout << "This test iterates through all the created valid bp string and verifies if they consist in a valid ParenthesesTree\n\n";
        //     }

        //     chrono::high_resolution_clock::time_point start;
        //     chrono::high_resolution_clock::time_point end;

        //     start = chrono::high_resolution_clock::now();
        //     for(ParenthesesTree pt: vpt) pt.is_bp();
        //     end = chrono::high_resolution_clock::now();

        //     std::chrono::duration<double, nano> elapsed_times{end - start};

        //     if (verbose)
        //     {
        //         cout << "Size: " << order << "IsBp operation average time: " << (elapsed_times.count() / (long double) 1000000) << "\n\n";
        //     }
        //     else
        //     {
        //         cout << order << ";" << (elapsed_times.count() / (long double) 1000000) << " ns\n";
        //     }
        // }
        if (enclose || all)
        {
            if (verbose && enclosestring)
            {
                cout << "This test consist in the enclose operation in each and every position [1...order_num-1] of the current PT BitVector in random order.\n\n";
                enclosestring = false;
            }

            chrono::high_resolution_clock::time_point start;
            chrono::high_resolution_clock::time_point end;

            vector<unsigned long long> rand_indexes(operations);

            for(int i = 0; i < operations; i++)
            {
                rand_indexes[i] = rand() % operations;
            }

            start = chrono::high_resolution_clock::now();
            for(unsigned long long i : rand_indexes)
            {
                PT.enclose(i);
            }
            end = chrono::high_resolution_clock::now();
            
            std::chrono::duration<double, nano> elapsed_times{end - start};
            
            if (verbose) cout << "Size: " << order << " average enclose operation time: " << elapsed_times.count() / operations << " ns\n\n";
            else cout << order << ";" << elapsed_times.count() / operations << "\n";
        }
        else if (all)
        {
            if (verbose && parentstring)
            {

            }
        }
    }
    return 1;
}