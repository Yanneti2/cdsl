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
    unsigned long _cap;  // The number of words of A.
    unsigned long _size;  // The lenght of the bit sequence (logical). 
    JacobsonRank *rank; // The rank structure.

public:
    // Methods implemented post GPT (originals by stringers)
    int grow(unsigned long ncap);
    unsigned long size() const;
    unsigned long cap() const;

    // Methods implemented by GPT (originals and modded)
    bitVector();
    bitVector(unsigned long size);
    bitVector(unsigned long size, int init);
    bitVector(unsigned long size, bool (*fn)(unsigned long));
    bitVector(string s);
    bitVector(long int num);
    ~bitVector();
    bitVector* deserialize(const char* path);
    void serialize(const char* path);

    void append0();
    void append1();
    void set0(unsigned long i);
    void set1(unsigned long i);
    void extend(bitVector *B);
    void put(bitVector *B, unsigned long i);

    bool operator==(bitVector B) const;
    int  operator[](unsigned long i) const;
    TYPE accessWord(unsigned long i) const;
    TYPE accessWord(unsigned long i, unsigned wordSize) const;
    bitVector *slice(unsigned long i, unsigned long k) const;

    void put(bitVector *SRC, unsigned long k, unsigned long i);


    void append(unsigned long number, unsigned long k);

    void print() const;

    unsigned long naive_rank1(unsigned long long i);
    unsigned long naive_rank0(unsigned long long i);
    unsigned long naive_select1(unsigned long long i);
    unsigned long naive_select0(unsigned long long i);
    unsigned long popcount();

    unsigned long long select1(unsigned long long i);
    unsigned long long select0(unsigned long long i);
    void JacobsonRank_build();
    unsigned long long rank0(unsigned long long i);
    unsigned long long rank1(unsigned long long i);
    void print_rank();

    void build_select0();
    void build_select1();
};

#endif
