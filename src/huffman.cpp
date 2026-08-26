
/* Worst Case Entropy (WCE): minimum number of bits required by codes to unambiguously encode all 
 * of the u codes that belong into the universe U.
 * 	- If the codes have all the same lenght, then this lenght has to be at least ceil(WCE) bits long.
 * 	- If they have different lenghts, the longest ones still have ceil(WCE) bits. 
 *
 * Shannon Entropy: average lenght of the codes of a univer U, such that u E U and Pr(u) is known;
 */

#include "../include/binary_tree.h"
#include "../include/bitvector.h"
#include "../include/huffman.h"
#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <set>
#include <map>
using namespace std;

/* Precisa transformar a variavel curr em bitvector, alem de transformas os tamanhos de int para unsigned long long int e deixar o de double talvez??? */

// receives the size of the set with unique elements to encode
double worst_case_entropy(double setSize){ return log2(setSize); }

// function that receives a strings and return a map of that strings chars probabilities
priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> probabilities_frequency(string S, bool prob){
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
			if(prob)table[i]->frequency/=size;
			pq.push(table[i]);
		}
	}
	return pq;
}

// Builds a Huffman tree and returns its root
BinaryTree::Node* build_huffman(priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> heap){
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
	return heap.top();
}

// travels a huffman tree and encode the nodes
void huffman_coding(BinaryTree::Node* root, map<char,string>& arr, string curr){
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
double average_length_codes(vector<unsigned long int> v1,vector<double> v2){
	double result = 0.0;
	for(unsigned long int i = 0;i<v1.size();i++){ result += (v1[i]*v2[i]); }	
	return result;
}

// receives the map with the Code length and probability and returns the minumun average code lenght that can be decoded (unambiguos)
double minimum_average_code_length(vector<double> v1){
	double result = 0.0;
	for(unsigned long int i = 0;i<v1.size();i++){ result += v1[i]*log2(1/v1[i]);}
	return result;
}
