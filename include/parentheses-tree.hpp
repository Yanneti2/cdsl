#include "bitvector.h"
#include "general_tree.h"

class ParenthesesTree {
private:
    bitVector T;

public:
    // Here, size_t refers to a position on the bitvector
    ParenthesesTree(const char *s);
    ParenthesesTree(Gtree *T);
    ~ParenthesesTree();

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
};
