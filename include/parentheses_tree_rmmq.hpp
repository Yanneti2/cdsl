#ifndef BPTREERMMQ
#define BPTREERMMQ

#include "general_tree.hpp"
#include "binary_tree.hpp"
#include "bitvector.hpp"
#include "parentheses_tree.hpp"

class ParenthesesTreeRMMQ : public ParenthesesTree {
    private:
        struct RMMQNode {
            long long e;
            long long max;
            long long min;
            long long min_count;
        };

        RMMQNode *rmmq_tree = nullptr;
        size_t b = 32;

        size_t leafnum(size_t k);
        size_t numleaf(size_t v);

    public:
        // Here, size_t refers to a position on the bitvector
        ParenthesesTreeRMMQ(string s) : ParenthesesTree(s) {init();};
        ParenthesesTreeRMMQ(ParenthesesTree T) : ParenthesesTree(T) {init();};
        ~ParenthesesTreeRMMQ() override;

        void init();
        unsigned long long backward_search(size_t i, long long d) override;
        unsigned long long forward_search(size_t i, long long d) override;
        unsigned long long forward_block(size_t i, long long &d);
        // unsigned long long min_count(size_t i, size_t j) override;
        // unsigned long long min_select(size_t i, size_t j) override;
        // unsigned long long max(size_t i, size_t j) override;
};

#endif
