#include "binary_tree.h"
#include "bitvector.h"
#include "general_tree.h"
#include "huffman.h"
#include "parentheses-tree.hpp"
#include <vector>
#include <stdio.h>
#include<stdlib.h>

#define ULI unsigned long int

/* Balanced Parentheses tree representationf using closing and opening parenthesis
 * The idea is, when we arrive at a node for the first time, during a dfs, we 
 * append an opening parenthesis (1) to the resulting bitVector B. When we finally
 * leave that subtree of a node, we append a closing parenthesis (0) to B.
 * 
 * bp_build and dfs_gt respective outputs:
 *
 * 		((()())((((()()()))())((()(())()())))())
 * 		1110100111110101000100111011001010000100
 */

// BP compact representation using c++ vector lib of a binary tree
//ParenthesesTree::ParenthesesTree(vector<int>& V, Tree::Node* node){
//	if (!node){
//		return;
//	}else{
//		V.push_back(1);
//		dfs_bt2(V,node->left);
//		dfs_bt2(V,node->right);
//		V.push_back(0);
//	}
//}

// Explicit BP representation from a Existent Binary Tree
//ParenthesesTree::ParenthesesTree(vector<char>& V, Tree::Node* node){
//	if (!node){
//		return;
//	}else{
//		V.push_back('(');
//		dfs_bt2(V,node->left);
//		dfs_bt2(V,node->right);
//		V.push_back(')');
//	}
//}

// Binary compressed representation for a general binary tree
void ParenthesesTree::bt_build(Tree::Node* node){
	if (!node){
		return;
	}else{
		this->T.append1();
		bt_build(node->left);
		bt_build(node->right);
		this->T.append0();
	}
}

// Binary compressed representation of a general tree (BP in binary)
void ParenthesesTree::gt_build(Gtree::gNode* node){
	if (!node){
		return;	
	}else{
		this->T.append1();
		for(ULI i=0; i<node->Children.size(); i++)gt_build(node->Children[i]);	
		this->T.append0();
	}	
}

// Balanced Parenthesis Explicit Build for General Trees
//ParenthesesTree::ParenthesesTree(vector<char>& V, Gtree::gNode* node){
//	if (!node){
//		return;	
//	}else{
//		V.push_back('(');
//		for(ULI i=0;i<node->Children.size();i++)bp_build(V,node->Children[i]);
//		V.push_back(')');
//	}	


// Balanced Parenthesis build for General Trees
//ParenthesesTree::ParenthesesTree(vector<int>& V, Gtree::gNode* node){
//	if (!node){
//		return;	
//	}else{
//		V.push_back(1);
//		for(ULI i=0;i<node->Children.size();i++)bp_build(V,node->Children[i]);
//		V.push_back(0);
//	}	
//}/}

// Balanced Parentheses build for string == "(()())"
ParenthesesTree::ParenthesesTree(string s){
	this->T = bitVector();
	for(unsigned long i=0; i<s.size(); i++){
		if (s[i] == '(')T.append1();
		else if (s[i] == ')')T.append0();
	}
}

// Constructor for a General Tree
ParenthesesTree::ParenthesesTree(Gtree t){
	this->T = bitVector();
	gt_build(t.getRoot());
}

// Constructor for a Binary Tree
ParenthesesTree::ParenthesesTree(Tree t){
	this->T = bitVector();
	bt_build(t.getRoot());
}

// Constructor for a Bitvector
ParenthesesTree::ParenthesesTree(bitVector& B){
	this->T = B;
}

/* 
 * The number of opening minus closing parenthesis in B[1,i]
 * undefined behavior for out of bounds i value or if the bitvector is not a BP
*/
unsigned long long ParenthesesTree::excess(size_t i)
{
    return 2 * this->T.naive_rank1(i) - i;
}

// returns true if bp and false if not bp 
bool ParenthesesTree::is_bp(){
	unsigned long size = T.size();
	if(size <= 0 || T[0]==0 || size % 2 != 0) return false;
	if(T[size-1]==0 && excess(size-1)==1){
		unsigned long c0 = 0;
		unsigned long c1 = 0;
		for(unsigned long i = 0; i < size; i++){
			if(T[i]==0)c0+=1;
			else if(T[i]==1)c1+=1;
			if(c0 > c1)return false;	
		}	
		return true;
	}
       	return false;	
}

// Searches for the greatest j < i | excess(B, j) == excess(B,i) + d
// if not found, returns 0 (should i change this behavior?)
unsigned long long ParenthesesTree::backward_search(size_t i, unsigned long long d)
{
    if (i == 0)
    {
        return 0;
    }

    unsigned long long target_depth = excess(i) + d;

    for (unsigned long long j = i - 1; j > 0; j--)
    {
        if(excess(j) == target_depth)
        {
            return j;
        }
    }

    return 0;
}

unsigned long long ParenthesesTree::forward_search(size_t i, unsigned long long d) {
	    if (i == 0)
    {
        return 0;
    }

    unsigned long long target_depth = excess(i) + d;

    for (unsigned long long j = i + 1; j < T.size(); j++)
    {
        if(excess(j) == target_depth)
        {
            return j;
        }
    }
    return 0;
}

//unsigned long long ParenthesesTree::fwd_search(size_t i, unsigned long long d) {
//	unsigned long long n = T.size(); // num de nós da tree
//	unsigned long long b = pow(log2(T.size()), 2); // número de elementos num bloco
//	unsigned long long r = (n + b - 1)/b; // num total de chunks
//	pair<unsigned long long, unsigned long long> a = fwdblock(i, d); 
//	unsigned long long d2 = a.first;
//	unsigned long long j = a.second;
//	if(d2 == d) return j;
//	unsigned long long v = leafnum((i + 1 + b - 1)/b);
//	// while (v + 1 < pow(2, (log2(v+1) + 1)/1) && d2 + R[v + 1] > d) {
//	// 	if (v % 2 == 0) d2 += R[v + 1].e;
//	// 	v /= 2;
//	// }
//	// if (v + 1 == pow(2, (log2(v+1) + 1)/1)) return n + 1;
//	// v ++;
//	// while (v < r){
//	// 	if (d2 + R[2*v].m <= d) v *= 2;
//	// 	else {
//	// 		d2 += R[2*v].e;
//	// 		v = 2*v + 1;
//	// 	}
//	// }
//	unsigned long long k = numleaf(v);
//	a = fwdblock((k - 1)*b, d - d2);
//	j = a.second;
//	return j;
//}

//pair<unsigned long long, unsigned long long> ParenthesesTree::fwdblock(size_t i, unsigned long long d) { 
//	// i = pos inicial do pedaço que será analisado
//	// B[s,e]
//	// C[x].e == excess(e) - excess(s - 1)
//	// C[x].m  == min(excess(p) - excess(s - 1), s <= p <= e)
//	unsigned long long p;
//	unsigned long long b = pow(log2(T.size()), 2); // número de elementos num bloco
//	unsigned long long c = log2(T.size()); // número de elementos num sub-bloco
//	unsigned long long f = (i + c - 1)/c; // chunk que contém o i e que será analisada bit a bit
//	//sendo explícito aqui, primeiro cálculo feito para achar o bloco em que i + 1 está contido e o segundo é para converter para chunk, não sei o motivo de fazer isso ao inves de fazer o teto de floor
//	unsigned long long t = (i + 1 + b - 1)/b *b/c; // número da última chunk do bloco
//	
//	long long int d2 = 0; // excesso acumulado de 1s
//	for (unsigned long long j = i + 1; j <= f * c; j++){ // percorre a chunk toda apartir de i
//		if ((*B)[j]) d2++;
//		else d2--;
//		if (d2 == d) {
//			pair<unsigned long long, unsigned long long> ret;
//			ret.first = d;
//			ret.second = j; // retorna o valor desejado e a posição encontrada do valor desejado
//			return ret;
//	}
//	}
//	// quero uma f p ler mais de um bit
//	for (p = f + 1; p <= t; p++) { // percorre as chunks do bloco até achar o valor requerido(d) em alguma delas
//		unsigned long long beg = (p - 1)*c + 1; // i que a chunk p começa
//		unsigned long long end = p*c; // i que a chunk p termina
//		// TYPE x[end - beg];
//		unsigned long long m; // valor mínimo do excess dentro da chunk, var provisória 
//		for(unsigned long long z = beg; z <= end; z++) { // for feito pra lidar provisoriamente com a falta dos valores pré comp.
//			// x[z] = (*B)[z + beg];
//			unsigned long long cur = excess(z);
//			if (cur < m) {
//				m = cur;
//			}
//		}
//
//		if (d2 + m - excess( beg - 1) <= d) break; // d2 não pode ser menor que d antes de entrar nesse for
//		d2 += excess(end) - excess(beg - 1); //soma o excess da chunk p em d2, subtraindo o excess da chunk anterior no cálculo, não entendi o motivo de calcular isso
//	}	
//
//	if (p > t) {
//		pair<unsigned long long, unsigned long long> ret;
//		ret.first = d2;
//		ret.second = t*c + 1; // caso não encontre, retorna o excesso do intervalo e a posição inicial do próximo bloco e o excesso do intervalo
//		return ret;
//	}
//
//	for (unsigned long long j = (p - 1) * c + 1; j <= p*c; j++) {
//		if ((*B)[j]) d2++;
//		else d2--;
//		if (d2 == d) {
//			pair<unsigned long long, unsigned long long> ret;
//			ret.first = d;
//			ret.second = j; // retorna a posição encontrada
//			return ret;
//		}
//	}
//	return 0;
//}


// Returns the position of the closing parenthesis of the opening one at i.
unsigned long long ParenthesesTree::close(unsigned long long i) {
    return forward_search(i, 0) - 1;
}

// Returns the position of the opening parentheses of the closing one at i.
unsigned long long ParenthesesTree::open(unsigned long long i) {
    return backward_search(i, -1);
}

// returns the rightmost position k, k < i and 1-indexed, of the closest k'th segment that contains the position i
unsigned long long ParenthesesTree::enclose(unsigned long long i) {
    return backward_search(i, -1);
}

// Returns the bitVector associated to this BP instance
bitVector& ParenthesesTree::getBv(){ return this->T; }

// returns the position of the direct/first parent of node in v'th index
size_t ParenthesesTree::parent(size_t v){
	if(v <= 0 || !is_bp())return -1;
	return enclose(v);
}

// if next element == 0 => true, else false
bool ParenthesesTree::isleaf(size_t v){
	if(T[v+1])return false;
	return true;
}

// returns the index of the deepest node in the subtree of v
size_t ParenthesesTree::deepestnode(size_t v){
	// returns the maximum excess forward	
	return v;
}

// returns the number os nodes inside a subtree with root = v counting v (inclusive) 
size_t ParenthesesTree::subtree(size_t v){
	return((close(v) -  v + 1)/2);
}

// Iterates linearly in a bitvector and returns the rank10 in position i
unsigned long long ParenthesesTree::rank10(size_t i){
	if(!is_bp() || i >= T.size())return -1;
	if(i <= 1) return 0;
	unsigned long long counter = 0;
	int prev = 1;
	for(size_t j = 1; j < i; j++){
		if(prev==1 && T[j]==0)counter++;
		prev = T[j];	
	}
	return counter; 
}

// in O(n), returns the position of the i'th 10==leaf (T[j]==1) 
size_t ParenthesesTree::select10(unsigned long long i){
	if(!is_bp() || i <= 1)return -1;
	unsigned long long counter = 0;
	int prev = 1;
	for(size_t j = 1; j < T.size()-1; j++){
		if(prev == 1 && T[j] == 0){
		       	++counter;
			--i;		
		}
		if(i == 0)return j-1;
		prev = T[j];
	}
	return -1; // i > 0
}

unsigned long long ParenthesesTree::leafrank(size_t v){
	return (rank10(v) + 1);
}

unsigned long long ParenthesesTree::leafnum(size_t v){
 	return (leafrank(close(v)) - leafrank(v));
}

size_t ParenthesesTree::leafselect(unsigned long long i){
 	return select10(i);
}
