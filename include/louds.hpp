#include "bitvector.h"
#include "general_tree.h"

class LOUDS {
private:
    bitVector T;

public:
    // Here, size_t refers to a position on the bitvector
    LOUDS(const char *s);
    LOUDS(Gtree *T);
    ~LOUDS();

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
