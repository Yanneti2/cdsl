#include "bitvector.h"
#include "general_tree.h"
#include "binary_tree.h"
#include <vector>
#include <string>

#ifndef ESTRUTURAS_DE_DADOS_COMPACTAS_PARENTHESESTREE
#define ESTRUTURAS_DE_DADOS_COMPACTAS_PARENTHESESTREE

class ParenthesesTree {
private:
    bitVector T;

public:
    void bt_build(bitVector* B, Tree::Node* node);
    void gt_build(bitVector* B, Gtree::gNode* node);

    ParenthesesTree(string s);
    ParenthesesTree(Gtree t);
    ParenthesesTree(Tree t);
    ParenthesesTree(bitVector B);

    unsigned long long excess(unsigned long long i);
    bool is_bp();
    unsigned long long backward_search(unsigned long long i, unsigned long long d);
    unsigned long long forward_search(unsigned long long i, unsigned long long d);
    unsigned long long close(unsigned long long i);
    unsigned long long open(unsigned long long i);
    unsigned long long enclose(unsigned long long i);

    // Here, size_t refers to a position on the bitvector
    
    size_t root();

    size_t fchild(size_t v);
    size_t lchild(size_t v);
    size_t child(size_t v, unsigned long long t);
    unsigned long long children(size_t v);
    unsigned long long childrank(size_t v);

    size_t parent(size_t v);
    size_t nsibling(size_t v);
    size_t psibling(size_t v);

    bool isleaf(size_t v);
    size_t leafselect(unsigned long long i);
    unsigned long long leafnum(size_t v);
    unsigned long long leafrank(size_t v);

    unsigned long long nodemap(size_t v);
    size_t nodeselect(unsigned long long i);

    unsigned long long depth(size_t v);
    unsigned long long height(size_t v);
    size_t deepestnode(size_t v);

    unsigned long long subtree(unsigned long long v);

    size_t preorderselect(unsigned long long i);
    size_t postorderselect(unsigned long long i);
    unsigned long long preorder(size_t v);
    unsigned long long postorder(size_t v);

    bool isancestor(size_t u, size_t v);
    size_t levelancestor(size_t v, unsigned long long d);
    size_t lca(size_t u, size_t v);

    bitVector& getBv();
};

#endif
