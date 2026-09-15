#ifndef BITVECTORJACOBSON
#define BITVECTORJACOBSON

#include "bitvector.h"

class BitVectorJ : public BitVector {
private:
    size_t layer1_size;
    size_t layer2_size;

    unsigned chunk1_size;
    unsigned chunk2_size;
    unsigned chunk2_per_chunk1;

    size_t *layer1;
    short *layer2;

    size_t *select_vector0;
    size_t *select_vector1;
    unsigned select_j;

    bool init_status = false;

public:
    explicit BitVectorJ();
    explicit BitVectorJ(size_t size);
    explicit BitVectorJ(size_t size, int init);
    explicit BitVectorJ(size_t size, bool (*fn)(size_t));
    explicit BitVectorJ(std::string s);
    explicit BitVectorJ(const BitVector &B);
    ~BitVectorJ();

    void init();
    bool is_initialized() const;

    size_t rank0(size_t i) const;
    size_t rank1(size_t i) const;

    void build_select0();
    void build_select1();
    size_t select0(size_t i) const;
    size_t select1(size_t i) const;
};

#endif
