#include "binary_tree.h"
#include "general_tree.h"
#include "bitvector.h"
#include <vector>

#ifndef ESTRUTURAS_DE_DADOS_COMPACTAS_BP
#define ESTRUTURAS_DE_DADOS_COMPACTAS_BP

/*
 * Faz sentido isso aq ter um header e nao ser so transferido como private
 * para dentro da classe que ele diz respeito (binary tree or general tree)?
 *
*/

void dfs_bt2(vector<int>& V, BinaryTree::Node* node);
void dfs_bt(bitVector* B, BinaryTree::Node* root);
void dfs_gt(bitVector* B, Gtree::gNode* root);
void bp_build(vector<char>& B, Gtree::gNode* node);
unsigned long long excess(bitVector* B, unsigned long long i);
bool is_bp(bitVector& B);
unsigned long long backward_search(bitVector* B, unsigned long long i, unsigned long long d);
unsigned long long enclose(bitVector* B, unsigned long long i);
#endif
