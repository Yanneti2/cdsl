#include "bitvector.h"

#include <iostream>
#include <map>

using namespace std;

typedef struct wtNode{
    wtNode *dad;
    wtNode *lchild;
    wtNode *rchild;
    bitVector freq;
    char symbol; //leaf
}wtNode;

class WaveletTree {
    protected:
    wtNode* root;
    string alphabet;

    public:
    wtNode* getRoot();
    WaveletTree(string S);
    wtNode* buildWT(string S, wtNode* dad);

    char access(unsigned long long i);
    unsigned long long rankc(char c, unsigned long long i);
    unsigned long long selectc(char c,  unsigned long long i, wtNode* Node);

    void print();
    ~WaveletTree();
};