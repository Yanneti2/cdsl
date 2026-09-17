#include <cstdint>
#include <cstdlib>
#include <string>

#ifndef BITVECTOR
#define BITVECTOR

#define NBITS __WORDSIZE

#if NBITS == 64
#define IS64BIT
#define TYPE uint64_t
#elif NBITS == 32
#define IS32BIT
#define TYPE uint32_t
#else
#error "Not a known processor"
#endif

class BitVector {
private:
    TYPE *A;       // The bitvector itself
    size_t _cap;   // The number of words of A.
    size_t _size;  // The lenght of the bit sequence (logical). 

public:
    void grow(size_t ncap);
    size_t size() const;
    size_t cap() const;

    explicit BitVector();
    explicit BitVector(size_t size);
    explicit BitVector(size_t size, int init);
    explicit BitVector(size_t size, bool (*fn)(size_t));
    explicit BitVector(std::string s);
    explicit BitVector(const BitVector &B);
    virtual ~BitVector();

    static BitVector *deserialize(const char *path);
    void serialize(const char *path) const;

    void append0();
    void append1();
    void set0(size_t i);
    void set1(size_t i);

    bool operator==(const BitVector &B) const;
    BitVector &operator=(const BitVector &B);
    int  operator[](size_t i) const;

    TYPE accessWord(size_t i) const;
    TYPE accessWord(size_t i, unsigned wordSize) const;

    void print() const;

    size_t naive_popcount() const;
    size_t naive_rank0(size_t i) const;
    size_t naive_rank1(size_t i) const;
    size_t naive_select0(size_t i) const;
    size_t naive_select1(size_t i) const;

    // The following methods are outdated and should be updated
    void extend(BitVector *B);
    void put(BitVector *B, unsigned long i);
    BitVector *slice(unsigned long i, unsigned long k) const;
    void put(BitVector *SRC, unsigned long k, unsigned long i);
    void append(unsigned long number, unsigned long k);
};

#endif
