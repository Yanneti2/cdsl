#include "huffman.h"

#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <queue>
#include <cmath>
#include <map>

using namespace std;

void run_test(const string& label, const string& S) {
    auto start = std::chrono::high_resolution_clock::now();
    HuffmanTree tree(S);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = end - start;
    cout << label << " build time: " << ms_double.count() << " ms" << endl;

    map<char, string> codes;
    tree.huffman_coding(tree.getRoot(), codes, "");

    for (auto const& kv : codes) {
        cout << kv.first << " - " << kv.second << endl;
    }

    map<char, double> freq;
    for (char c : S) freq[c] += 1.0;
    double size = S.size();

    vector<unsigned long long> lens;
    vector<double> probs;
    for (auto& kv : freq) {
        kv.second /= size;
        lens.push_back(codes[kv.first].size());
        probs.push_back(kv.second);
    }

    cout << label << " Average Code Length: " << tree.average_length_codes(lens, probs) << endl;
    cout << label << " Minimum Average Code Length: " << tree.minimum_average_code_length(probs) << endl;
    cout << label << " Worst Case Entropy: " << tree.worst_case_entropy((double)lens.size()) << endl << endl;
}

int main(void) {
    HuffmanTree dummy("a");

    cout << "T1: bracadabra" << endl
         << "T2: bananaabacate" << endl
         << "T3: abracadabra" << endl
         << "teste1: aaaaaaa" << endl
         << "teste2: a$b$$c$$$" << endl
         << "teste3: 10000*a + bcde" << endl
         << "teste4: 100*(all ascii printable chars)" << endl
         << "teste5: 1000000*y + x" << endl;
        //  << "teste6: file with string of order 1000000 (1M chars)" << endl << endl;

    run_test("T1", "bracadabra");
    run_test("T2", "bananaabacate");
    run_test("T3", "abracadabra");
    run_test("teste1", "aaaaaaa");
    run_test("teste2", "a$b$$c$$$");

    string teste3 = string(10000, 'a') + string(1, 'b') + string(1, 'c') + string(1, 'd') + string(1, 'e');
    run_test("teste3", teste3);

    string teste4 = "";
    for (int c = 32; c <= 126; c++) teste4 += string(100, (char)c);
    run_test("teste4", teste4);

    string teste5 = string(1000000, 'y') + string(1, 'x');
    run_test("teste5", teste5);

    // ifstream f("../tests/input_1M.txt");
    // string teste6((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    // run_test("teste6", teste6);

    HuffmanTree hfhf(teste3);
    BitVector* B = new BitVector();
    map<char, BitVector *> codes;
    hfhf.huffman_coding(hfhf.getRoot(), codes, B);

    for (auto const& kv : codes) {
        cout << kv.first << " - ";
        for (int pos = 0; pos < kv.second->size(); pos++){
            cout << (*kv.second)[pos];
        }
        cout << endl;
    }

    return 0;
}
