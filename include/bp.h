#include "binary_tree.h"
#include "general_tree.h"
#include <vector>
#include <string>

#ifndef ESTRUTURAS_DE_DADOS_COMPACTAS_BP
#define ESTRUTURAS_DE_DADOS_COMPACTAS_BP

/*
 * Faz sentido isso aq ter um header e nao ser so transferido como private
 * para dentro da classe que ele diz respeito (binary tree or general tree)?
 *
*/

void dfs_bt2(vector<int>& V, Tree::Node* node);
void dfs_bt(bitVector* B, Tree::Node* root);
void dfs_gt(bitVector* B, Gtree::gNode* root);
void bp_build(vector<char>& B, Gtree::gNode* node);

#endif
