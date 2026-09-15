
/* Worst Case Entropy (WCE): minimum number of bits required by codes to unambiguously encode all 
 * of the u codes that belong into the universe U.
 * 	- If the codes have all the same lenght, then this lenght has to be at least ceil(WCE) bits long.
 * 	- If they have different lenghts, the longest ones still have ceil(WCE) bits. 
 *
 * Shannon Entropy: average lenght of the codes of a univer U, such that u E U and Pr(u) is known;
 */

#include "binary_tree.h"
#include "bitvector.h"
#include "huffman.h"

#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <set>
#include <map>

using namespace std;

// function that receives a strings and return a map of that strings chars probabilities
priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> probabilities_frequency(string S) {
	BinaryTree::Node* table[256] = {};
	double size = S.size();
	for(char c : S){
		unsigned char index = (unsigned char) c;
		if(table[index])table[index]->frequency+=1.0;
		else table[index] = new BinaryTree::Node(c,1.0);
	}
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> pq;
	for(int i = 0; i<256;i++){
		if(table[i]){
			// if(prob)
			table[i]->frequency/=size;
			pq.push(table[i]);
		}
	}
	return pq;
}

// Builds a Huffman tree and returns its root
HuffmanTree::HuffmanTree(string S) {
 	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> heap = probabilities_frequency(S);
	while(heap.size() > 1){
		BinaryTree::Node* lc = heap.top();
		heap.pop();
		BinaryTree::Node* rc = heap.top();
		heap.pop();
		BinaryTree::Node* aux = new BinaryTree::Node('$', lc->frequency + rc->frequency);
		aux->left = lc;
		aux->right = rc;
		heap.push(aux);
	}
	delete this->root;
    this->root = heap.top();
}

// travels a ht and encode the nodes into its respetifully bitvector
void HuffmanTree::huffman_coding(BinaryTree::Node *root, map<char,BitVector*>& arr, BitVector* B){
	if (!root) return;
	if ((!root->left) && (!root->right)){
		if(B->size() != 0) {
			arr.insert({root->data, B});
		}
		else {
			BitVector* dmy = new BitVector();
			dmy->append0();
			arr.insert({root->data, dmy});
		}
		return;
	}
	BitVector* BL = new BitVector();
	BL->extend(B);
	BL->append0();
	
	BitVector* BR = new BitVector();
	BR->extend(B);
	BR->append1();

	huffman_coding(root->left,arr,BL);
	huffman_coding(root->right,arr,BR);
}

// travels a huffman tree and encode the nodes
void HuffmanTree::huffman_coding(BinaryTree::Node *root, map<char,string>& arr, string curr){
	if(!root)return;
	if(!root->left && !root->right){ 
		if(curr!="")arr.insert({root->data,curr});
		else arr.insert({root->data,"0"});
		return;
	} 
	huffman_coding(root->left,arr,curr+'0');
	huffman_coding(root->right,arr,curr+'1');
}

// map<encoding_length,char_probability>
double HuffmanTree::average_length_codes(vector<ULL> v1,vector<double> v2){
	double result = 0.0;
	for(ULL i = 0;i<v1.size();i++){ result += (v1[i]*v2[i]); }	
	return result;
}

// receives the map with the Code length and probability and returns the minumun average code lenght that can be decoded (unambiguos)
double HuffmanTree::minimum_average_code_length(vector<double> v1){
	double result = 0.0;
	for(ULL i = 0;i<v1.size();i++){ result += v1[i]*log2(1/v1[i]);}
	return result;
}

double HuffmanTree::worst_case_entropy(double setSize){ 
	return log2(setSize); 
}
