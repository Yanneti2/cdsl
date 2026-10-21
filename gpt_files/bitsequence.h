/**
   \file bitsequence.h
   \internal G.P.Telles, 2026.
   
   \brief A sequence of bits S that starts empty and grows by when bits are
   appended to it.

   S is implemented on a dynamic vector of uint64_t.
   Operations set and get are safe only in the current size range, [0..n-1].
**/


#ifndef BITSEQUENCE_H
#define BITSEQUENCE_H

#include <cinttypes>

class bitsequence {

 private:
  uint64_t* S;
  uint64_t cap;   // The number of words of S.
  uint64_t ssize; // The current size of this bit-sequence, initially zero.
  float ratio;    // The growth ratio.

  void grow(uint64_t newcap);
  
 public:
  bitsequence(uint64_t capacity, float growth_ratio);
  ~bitsequence();

  uint64_t size();
 
  void set0(uint64_t i);
  void set1(uint64_t i);
  int get(uint64_t i);

  void set(uint64_t i, uint64_t x, unsigned k);
  uint64_t get(uint64_t i, unsigned k);
  
  void append0();
  void append1();
  void append(uint64_t x, unsigned k);

  #if DEBUG
  void print();
  #endif
};

#endif
