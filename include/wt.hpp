#include "wtInterface.hpp"
#include "bitvectorj.hpp"

using namespace std;

#ifndef WT
#define WT

typedef struct wtNode{
    wtNode *dad;
    wtNode *lchild;
    wtNode *rchild;
    BitVectorJ* freq;
    string alphabet;
}wtNode;

class WaveletTree : public WTinterface {
    private:
    size_t _select(uint64_t c, size_t i, wtNode* node) const;
    
    protected:
    wtNode* root;

    public:
    wtNode* getRoot();
    WaveletTree(string S);
    wtNode* buildWT(string S, wtNode* dad);

    uint64_t access(size_t i) const override;
    size_t rank(uint64_t c, size_t i) const override;
    size_t select(uint64_t c, size_t i) const override;

    void print();
    ~WaveletTree();
};

#endif
