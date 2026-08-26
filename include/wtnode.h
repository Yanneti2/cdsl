#include "bitvector.h"

//mudar o freq para não utilizar ponteiros

class WaveletTreeNode {
private:
    // uint64_t len; // b
    
    public:
    WaveletTreeNode *d;
    WaveletTreeNode *l;
    WaveletTreeNode *r;
    BitVector *freq;
    WaveletTreeNode(std::string S, WaveletTreeNode* dad = NULL);
    unsigned long long rank();
    ~WaveletTreeNode();

    //mudancas feitas
    char access(unsigned long long i);
    void teste();
    unsigned long long select_c(char c,  unsigned long long j);
    unsigned long long rank_c(char c, unsigned long long i);
    void print();
};
