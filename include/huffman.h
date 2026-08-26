#include "binary_tree.h"
#include "bitvector.h"

#include <string>
#include <vector>
#include <queue>
#include <map>

using namespace std;

#ifndef HUFFMAN_BinaryTREE_H
#define HUFFMAN_TREE_H

class HuffmanTree {
    BinaryTree::Node *T;
public:
    double worst_case_entropy(double setSize);

    HuffmanTree(string S);

    void huffman_coding(BinaryTree::Node *root, map<char,string>& arr, string curr);

    double average_length_codes(vector<unsigned long int> v1, vector<double> v2);
    
    double minimum_average_code_length(vector<double> v2);

private:
    priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> probabilities_frequency(string S, bool prob);
};

#endif

