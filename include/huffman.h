#ifndef ESTRUTURAS_DE_DADOS_COMPACTAS_HUFFMAN
#define ESTRUTURAS_DE_DADOS_COMPACTAS_HUFFMAN

#include "binary_tree.h"
#include "bitvector.h"
#include <string>
#include <vector>
#include <queue>
#include <map>
using namespace std;

double worst_case_entropy(double setSize);

priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> probabilities_frequency(string S, bool prob);

BinaryTree::Node* build_huffman(priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> heap);

void huffman_coding(BinaryTree::Node* root, map<char,string>& arr, string curr);

double average_length_codes(vector<unsigned long int> v1,vector<double> v2);

double minimum_average_code_length(vector<double> v2);

#endif

