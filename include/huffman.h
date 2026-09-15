#include "binary_tree.h"
#include "bitvector.h"

#include <string>
#include <vector>
#include <queue>
#include <map>

using namespace std;

#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#define ULL unsigned long long

priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> probabilities_frequency(string S);

class HuffmanTree : public BinaryTree {
    BinaryTree::Node *T;    
public:
    double worst_case_entropy(double setSize);

    HuffmanTree(string S);

    void huffman_coding(BinaryTree::Node *root, map<char,string>& arr, string curr);
    void huffman_coding(BinaryTree::Node *root, map<char,bitVector*>& arr, bitVector* Bv);

    double average_length_codes(vector<ULL> v1,vector<double> v2);
    
    double minimum_average_code_length(vector<double> v2);
};

#endif
