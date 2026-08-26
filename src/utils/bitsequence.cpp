/**
  G.P.Telles, 2026.
  Definitions for bitsequence.h 
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "bitsequence.h"
#include "mask01.h"

// Masks with a single 1 at bit i:
#define masks1(i) (0x8000000000000000 >> (i))

// Masks with a single 0 at bit i:
#define masks0(i) (~(0x8000000000000000 >> (i)))


using namespace std;


/**
   \brief Create a new, empty bit-sequence.

   \param capacity The initial capacity (number of bits).
   \param growth_ratio
   
   The initial size is 0.
   
   During resizing, the bit sequence capacity will increase to the current
   capacity times the growth_ratio.  It doesn't shrink.
**/
bitsequence::bitsequence(uint64_t capacity, float growth_ratio) {

  cap = (capacity+63)/64;

  S = (uint64_t*) malloc(cap*8);
  if (!S)
    throw new bad_alloc();

  ratio = growth_ratio;
  ssize = 0;
}



bitsequence::~bitsequence() {
  free(S);
}



void bitsequence::grow(uint64_t capacity) {

  capacity = (capacity+63)/64;
  
  uint64_t* A = (uint64_t*) realloc(S,capacity*8);
  if (!A)
    throw new bad_alloc();

  S = A;
  cap = capacity;
}



uint64_t bitsequence::size() {

  return ssize;
}



/**
   \brief Perform S[i] = 0.
**/
void bitsequence::set0(uint64_t i) {

  S[i/64] &= masks0(i%64);
}



/**
   \brief Perform S[i] = 1.
**/
void bitsequence::set1(uint64_t i) {

  S[i/64] |= masks1(i%64);
}



/**
   \brief Return S[i].
**/
int bitsequence::get(uint64_t i) {

  return (S[i/64] & masks1(i%64)) ? 1 : 0;
}



/**
   \breaf Perform S[i..i+k-1] = x[63-k+1..63].
   
   In other words, copy the k least significant bits of x onto the
   bits i,i+1,...,i+k-1 of this bit-sequence.
**/
void bitsequence::set(uint64_t i, uint64_t x, unsigned k) {

  // first bit, last bit:
  uint64_t fword = i;
  uint64_t sword = fword+k-1;  
  uint8_t fbit = fword % 64;  
  uint8_t lbit = sword % 64;

  // first word, second word:
  fword /= 64; 
  sword /= 64;

  // Reset any leftover bit in x:
  x &= mask01[64-k];
  
  if (sword == fword) {
    S[fword] &= (~mask01[fbit] | mask01[lbit+1]);
    S[fword] |= x << (64-lbit-1);
  }
  else {
    S[fword] &= ~mask01[fbit];
    S[fword] |= x >> (lbit+1);
    S[sword] &= mask01[lbit+1];
    S[sword] |= x << (64-lbit-1);
  }
}



/**
   \brief Return x where x[63-k+1..63] = S[i..i+k-1].

   In other words, copy the bits i,i+1,...,i+k-1 of this bit-sequence
   onto the k least significant bits of an uint64_t an return it.
**/
uint64_t bitsequence::get(uint64_t i, unsigned k) {

  uint64_t fword = i;
  uint64_t sword = fword+k-1;
  uint8_t fbit = fword % 64;
  uint8_t lbit = sword % 64;
  fword /= 64;
  sword /= 64;

  if (sword == fword) {
    return (S[fword] & mask01[fbit] & ~mask01[lbit+1]) >> (64-lbit-1);
  }
  else {
    uint64_t l = (S[fword] & mask01[fbit]) << (lbit+1);
    uint64_t r = (S[sword] & ~mask01[lbit+1]) >> (64-lbit-1);
    return l|r;
  }
}



/**
   \brief Append 0 to S and increase |S| by one.
**/
void bitsequence::append0() {

  if (ssize == 64*cap)
    grow(cap*ratio);

  set0(ssize++);
}



/**
   \brief Append 0 to S and increase |S| by one.
**/
void bitsequence::append1() {

  if (ssize == 64*cap)
    grow(cap*ratio);

  set1(ssize++);
}



/**
   \brief Append x[63-k+1..63] to S and increase |S| by k.

   In other words, append the k least significant bits of x to this
   bit-sequence and increase its size by k.
**/
void bitsequence::append(uint64_t x, unsigned k) {

  if (ssize+k > 64*cap)
    grow(64*cap*ratio);

  set(ssize,x,k);
  ssize += k;
}



#if DEBUG
void bitsequence::print() {

  printf("size: %lu, cap: %lu, ratio: %f\n", ssize, cap, ratio);
  
  // ruler:
  printf(" ");
  for (int i=0; i<64*cap; i++) {
    if (i>0 && i%10 == 0)
      printf("%10d",i/10);
    //else
    //  printf(" ");
    
    //if (i%8 == 7)
    //  printf(" ");
  }
  printf("\n");
  
  for (int i=0; i<64*cap; i++) {
    printf("%d",i%10);
  
    //if (i%8 == 7)
    //  printf(" ");
  }
  printf("\n");
  
  // A:
  for (int i=0; i<64*cap; i++) {
    printf("%d",get(i));

    //if (i%8 == 7) 
    //  printf(" ");
  }
  printf("\n\n");
}
#endif
