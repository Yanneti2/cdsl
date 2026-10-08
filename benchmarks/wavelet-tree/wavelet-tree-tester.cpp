#include "wt.hpp"

#include <iostream>
#include <chrono>
#include <cmath>

using namespace std;

void shuffle(vector<pair<size_t, size_t>> &V) {
    for (size_t i = V.size(); i > 0; i--) {
        size_t _i = rand() % i;
        pair<size_t, size_t> aux = V[_i];
        V[_i] = V[i - 1];
        V[i - 1] = aux;
    }
}

int main(void){
    for (size_t order = 3; order < 9; order++) {

        string s;
        vector<pair<size_t, size_t>> rand_index(pow(10, order));
        for (size_t i = 0; i < pow(10, order); i++) {
            s.push_back(rand() % 128);
            rand_index[i] = {i, rand() % 128};
        }
        shuffle(rand_index);
        WaveletTree T(s);

        auto start = std::chrono::high_resolution_clock::now();
        for (pair<size_t, size_t> i : rand_index) {
            T.rankc(i.second, i.first);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, nano> elapsed_time{end - start};
        cout << elapsed_time / pow(10, order) << endl;
    }
}

