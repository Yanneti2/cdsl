#ifndef BPTREE
#define BPTREE

#include "general_tree.hpp"
#include "binary_tree.hpp"
#include "bitvectorj.hpp"

class ParenthesesTree {
protected:
    BitVectorJ T;

public:
    // Here, size_t refers to a position on the bitvector
    ParenthesesTree(string s);
    ParenthesesTree(Gtree t);
    ParenthesesTree(BinaryTree t);
    ParenthesesTree(BitVectorJ& B);

    virtual ~ParenthesesTree(){};
    
    // Private func range start ==============
    void bt_build(BinaryTree::Node* node);
    void gt_build(Gtree::gNode* node);
    bool is_bp();

    virtual unsigned long long backward_search(size_t i, long long d);
    virtual unsigned long long forward_search(size_t i, long long d);
    // virtual unsigned long long min_count(size_t i, size_t j);
    // virtual unsigned long long min_select(size_t i, size_t j);
    // virtual unsigned long long max(size_t i, size_t j);
    unsigned long long excess(size_t i);
    unsigned long long close(unsigned long long i);
    unsigned long long open(unsigned long long i); ///
    unsigned long long enclose(unsigned long long i);

    // Private func rang end ====================

    size_t root();

    size_t fchild(size_t v); ///
    size_t lchild(size_t v);
    size_t child(size_t v, unsigned long long t); ///
    unsigned long long children(size_t v);
    unsigned long long childrank(size_t v);

    size_t parent(size_t v);
    size_t nsibling(size_t v); ///
    size_t psibling(size_t v); ///

    bool isleaf(size_t v);
    size_t leafselect(unsigned long long i);
    unsigned long long leafnum(size_t v);
    unsigned long long leafrank(size_t v);

    unsigned long long nodemap(size_t v); ///
    size_t nodeselect(unsigned long long i); ///

    unsigned long long depth(size_t v); //
    unsigned long long height(size_t v); ///
    size_t rMq_naive(size_t i, size_t j); ///
    size_t deepestnode(size_t v);

    size_t subtree(size_t v);

    size_t preorderselect(unsigned long long i); // 
    size_t postorderselect(unsigned long long i); ///
    unsigned long long preorder(size_t v); ///
    unsigned long long postorder(size_t v); ///

    bool isancestor(size_t u, size_t v);
    size_t levelancestor(size_t v, unsigned long long d); /// 
    size_t lca(size_t u, size_t v); ///
    
    unsigned long long rank10(size_t i); ///
    size_t select10(unsigned long long i); ///

    BitVector& getBv(); ///
};

#endif
