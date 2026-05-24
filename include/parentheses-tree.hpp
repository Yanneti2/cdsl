#include "bitvector.h"
#include "general_tree.h"
#include "binary_tree.h"

class ParenthesesTree {
private:
    bitVector T;

public:
    // Here, size_t refers to a position on the bitvector
    ParenthesesTree(string s);
    ParenthesesTree(Gtree t);
    ParenthesesTree(Tree t);
    ParenthesesTree(bitVector& B);
    
    // Private func range start ==============
    void bt_build(Tree::Node* node);
    void gt_build(Gtree::gNode* node);
    bool is_bp();

    unsigned long long backward_search(size_t i, unsigned long long d);
    unsigned long long forward_search(size_t i, unsigned long long d);
    pair<unsigned long long, unsigned long long> fwdblock(size_t i, unsigned long long d);
    unsigned long long fwd_search(size_t i, unsigned long long d);
    unsigned long long excess(size_t i);
    unsigned long long close(unsigned long long i);
    unsigned long long open(unsigned long long i);
    unsigned long long enclose(unsigned long long i);

    // Private func rang end ====================

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

    size_t subtree(size_t v);

    size_t preorderselect(unsigned long long i);
    size_t postorderselect(unsigned long long i);
    unsigned long long preorder(size_t v);
    unsigned long long postorder(size_t v);

    bool isancestor(size_t u, size_t v);
    size_t levelancestor(size_t v, unsigned long long d);
    size_t lca(size_t u, size_t v);
    
    unsigned long long rank10(size_t i);
    size_t select10(unsigned long long i);

    bitVector& getBv();
};
