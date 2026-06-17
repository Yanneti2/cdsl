#include "GPT-originals/bitvectorBS.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    bool rank0 = false;
    bool rank1 = false;
    bool select0 = false;
    bool select1 = false;
    bool access = false;
    vector<size_t> d(0);
    int order = 9;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--rank0") == 0 || std::strcmp(argv[i], "-r0") == 0) {
            rank0 = true;
        } else if (std::strcmp(argv[i], "--rank1") == 0 || std::strcmp(argv[i], "-r1") == 0) {
            rank1 = true;
        } else if (std::strcmp(argv[i], "--select0") == 0 || std::strcmp(argv[i], "-s0") == 0) {
            select0 = true;
        } else if (std::strcmp(argv[i], "--select1") == 0 || std::strcmp(argv[i], "-s1") == 0) {
            select1 = true;
        } else if (std::strcmp(argv[i], "--access") == 0 || std::strcmp(argv[i], "-a") == 0) {
            access = true;
        } else if (std::strcmp(argv[i], "--order") == 0 || std::strcmp(argv[i], "-o") == 0) {
            i++;
            order = atoi(argv[i]);
        } else {
            int density = atoi(argv[i]);
            if (density <= 100) d.push_back(density);
        }
    }

    std::cout << "\"Density\";\"Time\"\n";

    for (size_t density : d) {
        srand(time(0));
        size_t size = pow(10, order);
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        vector<size_t> idx(999999);
        size_t upper_bound = (select0 || select1) ? (size * density * 95) / 10000 : size;

        bitvectorBS B(size);
        for (size_t j = 0; j < size; j++) {
            if (rand() % 100 < density) {
                B.set(j);
            }
        }

        B.preprocess();

        for (size_t j = 0; j < 999999; j++) {
            idx[j] = rand() % upper_bound + 1;
        }

        if (rank0) {
            start = std::chrono::high_resolution_clock::now();
            for (size_t i : idx) {
                B.rank0(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (rank1) {
            start = std::chrono::high_resolution_clock::now();
            for (size_t i : idx) {
                B.rank1(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (select0) {
            start = std::chrono::high_resolution_clock::now();
            for (size_t i : idx) {
                B.select0(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (select1) {
            start = std::chrono::high_resolution_clock::now();
            for (size_t i : idx) {
                B.select1(i);
            }
            end = std::chrono::high_resolution_clock::now();
        } else if (access) {
            start = std::chrono::high_resolution_clock::now();
            for (size_t i : idx) {
                B.access(i);
            }
            end = std::chrono::high_resolution_clock::now();
        }

        std::chrono::duration<double, nano> elapsed_time{end - start};
        cout << density << ";" << elapsed_time.count() / (long double) 999999 << "\n";
    }
}
