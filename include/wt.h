#include "bitvector.h"

#include <iostream>
#include <map>

using namespace std;

typedef struct wtNode{
    wtNode *d;
    wtNode *l;
    wtNode *r;
    bitVector *freq;
}wtNode;

class WaveletTree {
    protected:
    wtNode* root;

    public:
    std::string alpha;

    WaveletTree(string S, WaveletTree* r);
    WaveletTree(string S, wtNode* dad);

    char access(unsigned long long i);
    unsigned long long rankc(char c, unsigned long long i);
    unsigned long long selectc(char c,  unsigned long long i, wtNode* Node);
    
    void print();
};