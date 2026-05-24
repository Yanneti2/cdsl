#include <cstdint>
#include <string>
#include "jacobsonrank.h"

#ifndef BITVECTOR
#define BITVECTOR

// TODO: check system word size and use word size accordingly
#if INTPTR_MAX == INT64_MAX
#define IS64BIT
#define NBITS 64
#define TYPE uint64_t
#elif INTPTR_MAX == INT32_MAX
#define IS32BIT
#define NBITS 32
#define TYPE uint32_t
#else
#error "Not a known processor"
#endif

using namespace std;

class JacobsonRank;

class bitVector {
    // TODO: *a should be unsigned long???
private:
    TYPE *A;   // The bitvector itself
    size_t _cap;  // The number of words of A.
    size_t _size;  // The lenght of the bit sequence (logical). 
    JacobsonRank *rank = NULL; // The rank structure.

public:
    // Methods implemented post GPT (originals by stringers)
    void grow(size_t ncap);
    size_t size() const;
    size_t cap() const;

    // Methods implemented by GPT (originals and modded)
    bitVector();
    bitVector(size_t size);
    bitVector(size_t size, int init);
    bitVector(size_t size, bool (*fn)(size_t));
    bitVector(string s);
    bitVector(long int num);
    ~bitVector();
    bitVector &operator=(bitVector &B);

    static bitVector* deserialize(const char* path);
    void serialize(const char* path);

    void append0();
    void append1();
    void set0(size_t i);
    void set1(size_t i);

    bool operator==(bitVector B) const;
    int  operator[](size_t i) const;
    TYPE accessWord(size_t i) const;
    TYPE accessWord(size_t i, unsigned wordSize) const;

    void print() const;

    size_t naive_rank1(size_t i);
    size_t naive_rank0(size_t i);
    size_t naive_select1(size_t i);
    size_t naive_select0(size_t i);
    size_t popcount();

    size_t select1(size_t i);
    size_t select0(size_t i);
    void JacobsonRank_build();
    size_t rank0(size_t i);
    size_t rank1(size_t i);
    void print_rank();

    void build_select0();
    void build_select1();

    // The following methods are outdated and should be updated
    void extend(bitVector *B);
    void put(bitVector *B, unsigned long i);
    bitVector *slice(unsigned long i, unsigned long k) const;
    void put(bitVector *SRC, unsigned long k, unsigned long i);
    void append(unsigned long number, unsigned long k);

};

#endif
