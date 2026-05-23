#include "../include/binary_tree.h"
#include "../include/bitvector.h"
#include "../include/bp.h"
#include "../include/general_tree.h"
#include "../include/huffman.h"
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
void dfs_bt2(vector<int>& V, Tree::Node* node){
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
void dfs_bt(bitVector* B, Tree::Node* node){
	if (!node){
		B->append0();
		return;
	}else{
		B->append1();
		dfs_bt(B,node->left);
		dfs_bt(B,node->right);
		B->append0();
	}
}

// Binary compressed representation of a general tree (BP in binary)
void dfs_gt(bitVector* B, Gtree::gNode* node){
	if (!node){
		B->append0();
		return;	
	}else{
		B->append1();
		for(ULI i=0; i<node->Children.size(); i++)dfs_gt(B,node->Children[i]);	
		B->append0();
	}	
}

// Balanced Parenthesis Explicit Build for General Trees
void bp_build(vector<char>& B, Gtree::gNode* node){
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
unsigned long long excess(bitVector* B, unsigned long long i)
{
    return 2 * B->naive_rank1(i) - i;
}

// returns true if bp and false if not bp 
bool is_bp(bitVector& B){
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
unsigned long long backward_search(bitVector* B, unsigned long long i, unsigned long long d)
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

unsigned long long forward_search(bitVector* B, unsigned long long i, unsigned long long d) {}

unsigned long long close(bitVector* B, unsigned long long i) {
    return forward_search(B, i, 0) - 1;
}

unsigned long long open(bitVector* B, unsigned long long i) {
    return backward_search(B, i, -1);
}

// returns the rightmost position k, k < i and 1-indexed, of the closest k'th segment that contains the position i
unsigned long long enclose(bitVector* B, unsigned long long i) {
    return backward_search(B, i, -1);
}
