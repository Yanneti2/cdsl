#include "..\include\wt.h"
#include <iostream>
#include <chrono>
using namespace std;

int main(void){
    string S = "ab";
    auto start = std::chrono::high_resolution_clock::now();
    WaveletTree *W = new WaveletTree(S);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time = end - start;
    cout << "Tempo de construção do da Wavelet Tree de mississippi: " << time.count() << "ms\n\n";
    // W->teste();

    // for(auto i = 0; i < S.size(); i++){
    //     start = std::chrono::high_resolution_clock::now();
    //     cout << W->access(i);
    //     end = std::chrono::high_resolution_clock::now();
    //     time = end - start;
    //     cout << "\nTempo de Access: " << time.count() << "ms\n";

    //     start = std::chrono::high_resolution_clock::now();
    //     cout << W->rank_c(S[i], i);
    //     end = std::chrono::high_resolution_clock::now();
    //     time = end - start;
    //     cout << "\nTempo de Rank: " << time.count() << "ms\n";

    //     start = std::chrono::high_resolution_clock::now();
    //     cout << W->select_c(S[i], i);
    //     end = std::chrono::high_resolution_clock::now();
    //     time = end - start;
    //     cout << "\nTempo de Select: " << time.count() << "ms\n";
    // }
    cout << W->select_c('s', 3) << endl; 
    cout << W->select_c('i', 4) << endl; 
    cout << W->rank_c('s', 6) << endl;
    cout << W->rank_c('i', 6) << endl;

}