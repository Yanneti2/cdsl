#include "wt_succint.hpp"

#include <iostream>
#include <chrono>
#include <cmath>
#include <cstring>

#define ACCESS  (1 << 0)
#define RANKC   (1 << 1)
#define SELECTC (1 << 2)

using namespace std;

void parse_arg(int argc, char* argv[], uint64_t &args) {
    for (int i = 1; i < argc; i++) {
        const char* cur_arg = argv[i];
        if (!strcmp(cur_arg, "-a") || !strcmp(cur_arg, "--access"))
            args |= ACCESS;

        if (!strcmp(cur_arg, "-r") || !strcmp(cur_arg, "--rankc"))
            args |= RANKC;

        if (!strcmp(cur_arg, "-s") || !strcmp(cur_arg, "--selectc"))
            args |= SELECTC;
    }
}

void shuffle(vector<pair<size_t, size_t>> &V) {
    for (size_t i = V.size(); i > 0; i--) {
        size_t _i = rand() % i;
        pair<size_t, size_t> aux = V[_i];
        V[_i] = V[i - 1];
        V[i - 1] = aux;
    }
}

int main(int argc, char* argv[]) {
    uint64_t args = 0;
    parse_arg(argc, argv, args);
    srand(61027);

    for (size_t order = 3; order < 9; order++) {
        string s;
        unsigned long long n = pow(10, order);
        vector<pair<size_t, size_t>> rand_index(n);
        for (size_t i = 0; i < n; i++) {
            s.push_back(rand() % 128);
            rand_index[i] = {i, rand() % 128};
        }
        shuffle(rand_index);
        WaveletTreeSuccint T(s);

        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;

        cout << "\"Size\";\"Time\"\n";
        if (args & RANKC) {
            start = std::chrono::high_resolution_clock::now();
            for (pair<size_t, size_t> i : rand_index) {
                T.rank(i.second, i.first);
            }
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, nano> elapsed_time{end - start};
            cout << n << ";" << elapsed_time.count() / n << "\n";
        }

        if (args & SELECTC) {
            start = std::chrono::high_resolution_clock::now();
            for (pair<size_t, size_t> i : rand_index) {
                T.select(i.second, i.first);
            }
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, nano> elapsed_time{end - start};
            cout << n << ";" << elapsed_time.count() / n << "\n";
        }

        if (args & ACCESS) {
            start = std::chrono::high_resolution_clock::now();
            for (pair<size_t, size_t> i : rand_index) {
                T.access(i.second);
            }
            end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, nano> elapsed_time{end - start};
            cout << n << ";" << elapsed_time.count() / n << "\n";
        }
    }
}
