/*
  Guilherme P. Telles, 2026.
*/

#include <cstdio>
#include <cinttypes>
#include <cmath>

#include "bitvectorBS.h"

#ifndef bitMask
#define bitMask(i) mask10[(i)]
#endif

const uint64_t mask10[65] = {
  0x0000000000000000,0x8000000000000000,0xC000000000000000,0xE000000000000000,
  0xF000000000000000,0xF800000000000000,0xFC00000000000000,0xFE00000000000000,
  0xFF00000000000000,0xFF80000000000000,0xFFC0000000000000,0xFFE0000000000000,
  0xFFF0000000000000,0xFFF8000000000000,0xFFFC000000000000,0xFFFE000000000000,
  0xFFFF000000000000,0xFFFF800000000000,0xFFFFC00000000000,0xFFFFE00000000000,
  0xFFFFF00000000000,0xFFFFF80000000000,0xFFFFFC0000000000,0xFFFFFE0000000000,
  0xFFFFFF0000000000,0xFFFFFF8000000000,0xFFFFFFC000000000,0xFFFFFFE000000000,
  0xFFFFFFF000000000,0xFFFFFFF800000000,0xFFFFFFFC00000000,0xFFFFFFFE00000000,
  0xFFFFFFFF00000000,0xFFFFFFFF80000000,0xFFFFFFFFC0000000,0xFFFFFFFFE0000000,
  0xFFFFFFFFF0000000,0xFFFFFFFFF8000000,0xFFFFFFFFFC000000,0xFFFFFFFFFE000000,
  0xFFFFFFFFFF000000,0xFFFFFFFFFF800000,0xFFFFFFFFFFC00000,0xFFFFFFFFFFE00000,
  0xFFFFFFFFFFF00000,0xFFFFFFFFFFF80000,0xFFFFFFFFFFFC0000,0xFFFFFFFFFFFE0000,
  0xFFFFFFFFFFFF0000,0xFFFFFFFFFFFF8000,0xFFFFFFFFFFFFC000,0xFFFFFFFFFFFFE000,
  0xFFFFFFFFFFFFF000,0xFFFFFFFFFFFFF800,0xFFFFFFFFFFFFFC00,0xFFFFFFFFFFFFFE00,
  0xFFFFFFFFFFFFFF00,0xFFFFFFFFFFFFFF80,0xFFFFFFFFFFFFFFC0,0xFFFFFFFFFFFFFFE0,
  0xFFFFFFFFFFFFFFF0,0xFFFFFFFFFFFFFFF8,0xFFFFFFFFFFFFFFFC,0xFFFFFFFFFFFFFFFE,
  0xFFFFFFFFFFFFFFFF
};

// Masks with a single 1 at bit i:
#define masks1(i) (0x8000000000000000 >> (i))

// Masks with a single 0 at bit i:
#define masks0(i) (~(0x8000000000000000 >> (i)))

using namespace std;



/**
   \brief Create a bitvector B with capacity for size bits.
   The actual bitvector size will be ceil(n/64)*64 bits.
**/
bitvectorBS::bitvectorBS(uint64_t size) {

  B = new uint64_t[(size-1)/64+1];
  bsize = size;
  V = NULL;
}



bitvectorBS::~bitvectorBS() {

  if (B)
    delete [] B;
  if (V)
    delete V;
}



uint64_t bitvectorBS::size() {

  return bsize;
}



/**
   \brief Turn all bits in B to 0.
**/
void bitvectorBS::reset_all() {

  uint64_t n = (bsize+63)/64;
  for (uint64_t i=0; i<n; i++) 
    B[i] = 0x0000000000000000;
}



/**
   \brief Turn all bits in B to 1.
**/
void bitvectorBS::set_all() {

  uint64_t n = (bsize+63)/64;
  for (uint64_t i=0; i<n; i++) 
    B[i] = 0xFFFFFFFFFFFFFFFF;
}



/**
   \brief Perform B[i] = 0.
**/
void bitvectorBS::reset(uint64_t i) {

  B[i/64] &= masks0(i%64);
}



/**
   \brief Perform B[i] = 1.
**/
void bitvectorBS::set(uint64_t i) {

  B[i/64] |= masks1(i%64);
}



/**
   \brief Return B[i].
**/
unsigned bitvectorBS::test(uint64_t i) {

  return (B[i/64] & masks1(i%64)) ? 1 : 0;
}



void bitvectorBS::preprocess() {

  // Reset padding bits in B:
  uint64_t words = (bsize+63)/64; 
  unsigned padded = words*64-bsize;
  if (padded) 
    B[words-1] &= mask10[64-padded];

  // Count ones:
  uint64_t vsize = 0;
  for (uint64_t i=0; i<words; i++) 
    vsize += std::__popcount(B[i]);

  // Fill V:
  V = new UVector(vsize+1,ceil(log2(bsize+1)));
  vsize = 0;
  for (uint64_t i=0; i<bsize; i++) 
    if (test(i))
      V->set(vsize++,i);

  // Add the size of B as sentinel:
  V->set(vsize,bsize);
  
  delete [] B;
  B = NULL;
}



/*
  A binary search to get the leftmost positon of V with value larger
  than or equal to x. It returns UINT64_MAX if no such position exists.
*/
 uint64_t lbsearch(UVector* V, uint64_t x) {

  uint64_t left = 0;
  uint64_t right = V->size()-1;
  uint64_t middle;

  while (left < right) {
    middle = (left+right)/2;
    if (x > V->operator[](middle))
      left = middle + 1;
    else
      right = middle;
  }
  
  if (V->operator[](left) >= x)
    return left;
  else
    return UINT64_MAX;
}



/**
   \brief Return B[i].
**/
unsigned bitvectorBS::access(uint64_t i) {

  uint64_t pos = lbsearch(V,i);

  if (pos == UINT64_MAX || V->operator[](pos) != i)
    return 0;

  return 1;
}

/**
   \brief Return 0 if V[i] == 0 or 1 if V[i] == 1.

   If i > |B|-1 then the behavior is undefined.
**/
unsigned bitvectorBS::operator[](uint64_t i) const {
    return (B[i / NBITS] & (bitMask(i % NBITS) ^ bitMask(i % NBITS + 1))) ? 1 : 0;
}


/**
   \brief Return rank1(B,i).
   Return the number of 1s in B[0..i].
**/
uint64_t bitvectorBS::rank1(uint64_t i) {

  uint64_t pos = lbsearch(V,i);

  if (pos == UINT64_MAX)
    return 0;

  return pos + (V->operator[](pos) == i); 
}



/**
   \brief Return rank0(B,i).
   Return the number of 0s in B[0..i].
**/
uint64_t bitvectorBS::rank0(uint64_t i) {

  return (i+1 - rank1(i));
}



/*
  A binary search to get the minimum i such that V[i]-i is larger than
  or equal to x.  It returns UINT64_MAX if no such position exists.
*/
uint64_t lbsearch0(UVector* V, uint64_t x) {

  uint64_t last = V->size()-1;
  
  uint64_t left = 0;
  uint64_t right = last;
  uint64_t middle, zeros;

  while (left < right) {
    middle = (left+right)/2;

    if (middle == last)
      zeros = V->operator[](last) - last;
    else
      zeros = V->operator[](middle) - middle;
    
    if (x > zeros)
      left = middle + 1;
    else
      right = middle;
  }

  zeros = V->operator[](left) - left;
  
  if (zeros >= x)
    return left;
  else
    return UINT64_MAX;
}



/**
   \brief Return select0(B,j).
   Return the index of the j-th 0 in B, or |B| if there are not j 0s in B.
**/
uint64_t bitvectorBS::select0(uint64_t j) {

  // The element of V with the index of the first 1 to the right of j 0s: 
  uint64_t i = lbsearch0(V,j);

  //printf("pos=%lu\n",i);
  
  if (i == UINT64_MAX)
    return bsize;

  // The number of 0s up to V[i] is V[i]-i, so return V[i] - (V[i] - i - j) - 1:
  return i + j - 1;
}



/**
   \brief Return select1(B,j).
   Return the index of the j-th 1 in B, or |B| if there are not j 1s in B.
**/
uint64_t bitvectorBS::select1(uint64_t j) {
  if(j==0) return 0;
  if (j >= V->size())
    return bsize;
  
  return V->operator[](j-1);
}



#if DEBUG

/**
   \brief Print the bit vector.
**/
void bitvectorBS::print() {

  unsigned i;  
  uint64_t w = ((bsize+63)/64);

  if (V) {
    printf("bitvector size=%lu words=%lu\n",bsize,w);

    // ruler:
    printf(" ");
    for (i=0; i<64*w; i++) {
      if (i>0 && i%10 == 0)
	printf("%10d",i/10);
      if (i%8 == 7)
	printf(" ");
    }
    printf("\n");
    
    for (i=0; i<64*w; i++) {
      printf("%d",i%10);
      if (i%8 == 7)
	printf(" ");
    }
    printf("\n");
    
    // B:
    for (i=0; i<bsize; i++) {
      printf("%u",0+test(i));
      if (i%8 == 7) 
	printf(" ");
    }
    for ( ; i<64*w ; i++) {
      printf("-");
      if (i%8 == 7) 
	printf(" ");
    }
    printf("\n");
  }

  if (V)
    V->print();
}



void bitvectorBS::set_random() {

  uint64_t n = (bsize+63)/64;
  n *= 4;
  
  uint16_t* p = (uint16_t*) B;
  for (uint64_t i=0; i<n; i++) 
    p[i] = rand() % 65536;
}

#endif
