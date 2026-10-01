#include "wt.hpp"

#include <iostream>
#include <cstring>
#include <string>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]){
    int operations = 100000;

    bool verbose = false;
    bool selectc = false;
    bool ss = false;
    bool access = false;
    bool acss = false;
    bool rankc = false;
    bool rcs = false;
    bool all = false;

    for (int i = 0; i < argc; i++){
        char *arg = argv[i];
        if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-v") == 0) verbose = true;
        else if (strcmp(arg, "--selectc") == 0 || strcmp(arg, "-sc") == 0) selectc = true;
        else if (strcmp(arg, "--access") == 0 || strcmp(arg, "-acs") == 0) access = true;
        else if (strcmp(arg, "--rankc") == 0 || strcmp(arg, "rc") == 0) rankc = true;
        else if (strcmp(arg, "--all") == 0 || strcmp(arg, "-a") == 0) all = true;
    }

    for (int order = 3; order < 9; order++) {
        long long order_num = 1;
        for (int aux = 0; aux < order; aux++) order_num *= 10;

        string s = "";
        for (long long i = 0; i < order_num; i++) s += char((rand() % 94) + 33);

        WaveletTree wt(s);

        if (all || selectc){
            if (verbose && ss){
                cout << "Selectc for all chars and positions in the given string" << endl;
                ss = false;
            }

            chrono::high_resolution_clock::time_point start;
            chrono::high_resolution_clock::time_point end;

            start = std::chrono::high_resolution_clock::now();

            end = std::chrono::high_resolution_clock::now();

            chrono::duration<double, nano> elapsed_time{end - start};   
        }
        else if (all || rankc){

        }
        else if (all || access){
            
        }
    }
}