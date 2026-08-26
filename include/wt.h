#include "wtnode.h"

class WaveletTree {
    private:

    public:
    WaveletTreeNode* root;
    std::string alpha;
    WaveletTree(std::string S, WaveletTreeNode* r);
    char Acess(unsigned long long i);
    unsigned long long rankc(char c, unsigned long long i);
    unsigned long long selectc(char c,  unsigned long long i, WaveletTreeNode* Node);
    void print();
};
