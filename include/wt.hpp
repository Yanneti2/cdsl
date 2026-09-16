#include "bitvector.hpp"

#include <iostream>
#include <map>

using namespace std;

typedef struct wtNode{
    wtNode *dad;
    wtNode *lchild;
    wtNode *rchild;
    BitVector* freq;
    string alphabet;
}wtNode;

class WaveletTree {
    protected:
    wtNode* root;

    public:
    wtNode* getRoot();
    WaveletTree(string S);
    wtNode* buildWT(string S, wtNode* dad);

    char access(unsigned long long i);
    unsigned long long rankc(char c, unsigned long long i);
    unsigned long long selectc(char c,  size_t i, wtNode* Node);

    void print();
    ~WaveletTree();
};
