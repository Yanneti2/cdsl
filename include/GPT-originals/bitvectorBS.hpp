/**
   \file bitvector.h
   \internal Guilherme P. Telles, 2026.

   \brief A bitvector with rank and select support.

   Rank and select are implemented as a binary search on the array of
   positions of 1s in the bitvector.
   
   After creating a bitvector of size n and performing set, reset and
   test as needed, invoking preprocess() will build the structures and
   release the bitvector.  After preprocessing, access, rank, and
   select may be used; set, reset and test may not.

   After preprocessing, the bitvector B is replaced by an array with
   the positions of the 1s and size (n1+1) * ceil(log2(n+1)), where n
   is the size of B and n1 is the number of ones in B.

   access, rank0, rank1 and select0 are O(log n1) time and
   select1 is O(1) time.
**/


#ifndef BITVECTORBS_H
#define BITVECTORBS_H

#include "GPT-originals/uvector.h"

class bitvectorBS {

 private:
  uint64_t* B;    // The bitvector
  uint64_t bsize; // and its size in bits.

  uvector* V;  // The vector of positions of 1s in B.
    
 public:
  bitvectorBS(uint64_t size);
  ~bitvectorBS();

  uint64_t size();
  void set_all();
  void reset_all();

  void set(uint64_t i);
  void reset(uint64_t i);
  unsigned test(uint64_t i);

  void preprocess();

  unsigned access(uint64_t i);

  uint64_t rank0(uint64_t i);
  uint64_t rank1(uint64_t i);

  uint64_t select0(uint64_t j);
  uint64_t select1(uint64_t j);
  
  #if DEBUG
  void print();
  void set_random();
  #endif
};

#endif
