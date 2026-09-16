#include <cstdint>

#ifndef BITVECTOR
#define BITVECTOR

// TODO: check system word size and use word size accordingly
#if INTPTR_MAX == INT64_MAX
#define IS64BIT
#elif INTPTR_MAX == INT32_MAX
#define IS32BIT
#else
#error "Not a known processor"
#endif

#ifdef IS32BIT
#define NBITS 32
#define TYPE uint32_t
#else
#define NBITS 64
#define TYPE uint64_t
#endif

class bitVector {
  // TODO: *a should be unsigned long???
  private:
      TYPE * A;   // The bitvector itself
      TYPE cap;  // The number of words of A.
      TYPE len;  // The lenght of the bit sequence (logical). 
      float ratio;        // The growing factor;

  public:
    // Methods implemented post GPT (originals by stringers)
    unsigned long ceil(unsigned long ul);
    int grow(unsigned long ncap);
    unsigned long getLength();
    void extend(bitVector* B);
    unsigned long getCap();

    // Methods implemented by GPT (originals and modded)
    bitVector(unsigned long capacity, float growth_ratio);
    ~bitVector();

    void set1(unsigned long i);
    void set0(unsigned long i);
    int  access(unsigned long i);

    void put(bitVector* SRC, unsigned long k, unsigned long i);

    void append0();
    void append1();

    void append(unsigned long number, unsigned long k);


    void print();
};

#endif
