/**
   \file bitvector-RRR.h
   \internal G.P.Telles, 2026.
   
   \brief A bitvector with fast and compact rank and select suport.

   An implementation of: Rajeev Raman, Venkatesh Raman, Srinivasa Rao
   Satti.  Succinct Indexable Dictionaries with Applications to
   Encoding k-ary Trees, Prefix Sums and Multisets.  ACM Transactions
   on Algorithms, 3, 2007.
**/


#ifndef BITVECTORRRR_H
#define BITVECTORRRR_H

#include <cinttypes>
#include <cmath>

#include "uvector.h"
#include "bitsequence.h"


using namespace std;


class bitvectorRRR {

 private:
  uint64_t* V;
  uint64_t vsize;  // The number of bits in V.
  uint64_t vsize_req;  // The number of bits for V requisted on creation.

  unsigned bsize; // The number of bits in a block.
  unsigned ssize; // The number of blocks in a superblock.

  // The parallel vectors that replace B, namely class and rank-in-class:
  uvector* C;
  bitsequence* R;

  // The table that maps [class][index] to a pattern of bsize bits:
  uvector** T;

  // The number of bits of the elements in each row of T:
  uvector* Tbits;

  // The superblocks. For each, the sum of 1s prior to its begining and
  // the index in R where it starts:
  uvector* SBsum;
  uvector* SBidx;
  
  uint64_t get(uint64_t i, unsigned k);
  uint64_t lbsearch(uvector* V, uint64_t x);
  
 public:
  bitvectorRRR(uint64_t size);
  ~bitvectorRRR();

  uint64_t size();
  
  void set0();
  void set1();

  void set0(uint64_t i);
  void set1(uint64_t i);
  int get(uint64_t i);

  void preprocess();

  uint64_t rank1(uint64_t i);
  uint64_t select1(uint64_t j);

  unsigned access(uint64_t i);

  /*
  uint64_t rank0(uint64_t i);

  uint64_t select0(uint64_t j);

  */

  #if DEBUG
  void print();
  void set_random();

  uint64_t rank1_raw(uint64_t i);
  uint64_t select1_raw(uint64_t j);
  #endif
};

#endif
