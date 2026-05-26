#include "general_tree.h"
#include "binary_tree.h"
#include "bitvector.h"
#include <string>

class LOUDS {
private:
    bitVector T;

public:
    // Here, size_t refers to a position on the bitvector
    LOUDS(string s);
    LOUDS(Gtree* G);
    LOUDS(Tree* BT);

    bool is_louds(string s);
    bool is_louds();
    void print();

    size_t succ0(size_t v);
    size_t succ1(size_t v);
    size_t pred0(size_t v);
    size_t pred1(size_t v);

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

    unsigned long long nodemap(size_t v);
    size_t nodeselect(unsigned long long i);
};
