#include "../include/binary_tree.h"
#include "../include/bitvector.h"
#include "../include/bp.h"
#include "../include/general_tree.h"
#include <vector>
#include <string>

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

