#include "binary_tree.h"
#include "bitvector.h"
#include "general_tree.h"
#include "huffman.h"
#include "parentheses-tree.hpp"
#include <vector>
#include <stdio.h>


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

// BP compact representation using c++ vector lib
void ParenthesisTree::dfs_bt2(vector<int>& V, Tree::Node* node){
	if (!node){
		V.push_back(0);
		return;
	}else{
		V.push_back(1);
		dfs_bt2(V,node->left);
		dfs_bt2(V,node->right);
		V.push_back(0);
	}
}

// Binary compressed representation for a general binary tree
 void ParenthesisTree::dfs_bt(Tree::Node* node){
	if (!node){
		T->append0();
		return;
	}else{
		T->append1();
		dfs_bt(T,node->left);
		dfs_bt(T,node->right);
		T->append0();
	}
}

// Binary compressed representation of a general tree (BP in binary)
 void ParenthesisTree::dfs_gt(Gtree::gNode* node){
	if (!node){
		T->append0();
		return;	
	}else{
		T->append1();
		for(ULI i=0; i<node->Children.size(); i++)dfs_gt(T,node->Children[i]);	
		T->append0();
	}	
}

// Balanced Parenthesis Explicit Build for General Trees
 void ParenthesisTree::bp_build(vector<char>& B, Gtree::gNode* node){
	if (!node){
		B.push_back(')');
		return;	
	}else{
		B.push_back('(');
		for(ULI i=0;i<node->Children.size();i++)bp_build(B,node->Children[i]);
		B.push_back(')');
	}	
}

// The number of opening minus closing parenthesis in B[1,i]
// undefined behavior for out of bounds i value or if the bitvector is not a BP
unsigned long long ParenthesesTree::excess(size_t i)
{
    return 2 * B->naive_rank1(i) - i;
}

// returns true if bp and false if not bp 
bool ParenthesesTree::is_bp(bitVector& B){
	unsigned long size = B.size();
	if(size <= 0 || B[0]==0 || size % 2 != 0) return false;
	if(B[size-1]==0 && excess(&B,size-1)==1){
		unsigned long select0 = 0;
		unsigned long select1 = 0;
		for(unsigned long i = 0; i < size; i++){
			if(B[i]==0)select0+=1;
			else if(B[i]==1)select1+=1;
			if(select0 > select1)return false;	
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

    unsigned long long target_depth = excess(B, i) + d;

    for (unsigned long long j = i - 1; j > 0; j--)
    {
        if(excess(B,j) == target_depth)
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

unsigned long long ParenthesesTree::fwd_search(size_t i, unsigned long long d) {
	unsigned long long n = T.size(); // num de nós da tree
	unsigned long long b = pow(log2(T.size()), 2); // número de elementos num bloco
	unsigned long long r = (n + b - 1)/b; // num total de chunks
	pair<unsigned long long, unsigned long long> a = fwdblock(i, d); 
	unsigned long long d2 = a.first;
	unsigned long long j = a.second;
	if(d2 == d) return j;
	unsigned long long v = leafnum((i + 1 + b - 1)/b);
	// while (v + 1 < pow(2, (log2(v+1) + 1)/1) && d2 + R[v + 1] > d) {
	// 	if (v % 2 == 0) d2 += R[v + 1].e;
	// 	v /= 2;
	// }
	// if (v + 1 == pow(2, (log2(v+1) + 1)/1)) return n + 1;
	// v ++;
	// while (v < r){
	// 	if (d2 + R[2*v].m <= d) v *= 2;
	// 	else {
	// 		d2 += R[2*v].e;
	// 		v = 2*v + 1;
	// 	}
	// }
	unsigned long long k = numleaf(v);
	a = fwdblock((k - 1)*b, d - d2);
	j = a.second;
	return j;
}

pair<unsigned long long, unsigned long long> ParenthesesTree::fwdblock(size_t i, unsigned long long d) { 
	// i = pos inicial do pedaço que será analisado
	// B[s,e]
	// C[x].e == excess(e) - excess(s - 1)
	// C[x].m  == min(excess(p) - excess(s - 1), s <= p <= e)
	unsigned long long p;
	unsigned long long b = pow(log2(T.size()), 2); // número de elementos num bloco
	unsigned long long c = log2(T.size()); // número de elementos num sub-bloco
	unsigned long long f = (i + c - 1)/c; // chunk que contém o i e que será analisada bit a bit
	//sendo explícito aqui, primeiro cálculo feito para achar o bloco em que i + 1 está contido e o segundo é para converter para chunk, não sei o motivo de fazer isso ao inves de fazer o teto de floor
	unsigned long long t = (i + 1 + b - 1)/b *b/c; // número da última chunk do bloco
	
	long long int d2 = 0; // excesso acumulado de 1s
	for (unsigned long long j = i + 1; j <= f * c; j++){ // percorre a chunk toda apartir de i
		if ((*B)[j]) d2++;
		else d2--;
		if (d2 == d) {
			pair<unsigned long long, unsigned long long> ret;
			ret.first = d;
			ret.second = j; // retorna o valor desejado e a posição encontrada do valor desejado
			return ret;
	}
	}
	// quero uma f p ler mais de um bit
	for (p = f + 1; p <= t; p++) { // percorre as chunks do bloco até achar o valor requerido(d) em alguma delas
		unsigned long long beg = (p - 1)*c + 1; // i que a chunk p começa
		unsigned long long end = p*c; // i que a chunk p termina
		// TYPE x[end - beg];
		unsigned long long m; // valor mínimo do excess dentro da chunk, var provisória 
		for(unsigned long long z = beg; z <= end; z++) { // for feito pra lidar provisoriamente com a falta dos valores pré comp.
			// x[z] = (*B)[z + beg];
			unsigned long long cur = excess(z);
			if (cur < m) {
				m = cur;
			}
		}

		if (d2 + m - excess( beg - 1) <= d) break; // d2 não pode ser menor que d antes de entrar nesse for
		d2 += excess(end) - excess(beg - 1); //soma o excess da chunk p em d2, subtraindo o excess da chunk anterior no cálculo, não entendi o motivo de calcular isso
	}	

	if (p > t) {
		pair<unsigned long long, unsigned long long> ret;
		ret.first = d2;
		ret.second = t*c + 1; // caso não encontre, retorna o excesso do intervalo e a posição inicial do próximo bloco e o excesso do intervalo
		return ret;
	}

	for (unsigned long long j = (p - 1) * c + 1; j <= p*c; j++) {
		if ((*B)[j]) d2++;
		else d2--;
		if (d2 == d) {
			pair<unsigned long long, unsigned long long> ret;
			ret.first = d;
			ret.second = j; // retorna a posição encontrada
			return ret;
		}
	}
	return 0;
}

unsigned long long ParenthesesTree::close(size_t i) {
    return forward_search(i, 0) - 1;
}

unsigned long long ParenthesesTree::open(size_t i) {
    return backward_search(i, -1);
}

// returns the rightmost position k, k < i and 1-indexed, of the closest k'th segment that contains the position i
unsigned long long ParenthesesTree::enclose(size_t i) {
    return backward_search(i, -1);
}
