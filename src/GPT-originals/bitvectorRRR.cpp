/**
  G.P.Telles, 2026.
  Definitions for bitvector-RRR.h 
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "GPT-originals/bitvectorRRR.h"
#include "masks/mask01.h"
#include "masks/mask10.h"

using namespace std;

// Integer %:
#define intmod(i,j) (((i)+(j)-1)/(j))

// 64 bit masks with a single 1 at bit i:
#define masks1(i) (0x8000000000000000 >> (i))

// 64 bit masks with a single 0 at bit i:
#define masks0(i) (~(0x8000000000000000 >> (i)))



/**
   \brief Create a new bitvector with size bits.
   Size may not be too small. Size 1 will surely break.
**/
bitvectorRRR::bitvectorRRR(uint64_t size) {

  vsize_req = size;
  
  bsize = ceil(log2(vsize_req)/2);
  ssize = (unsigned) floor(log2(vsize_req));

  vsize = bsize * intmod(vsize_req,bsize); 
  
  V = new uint64_t[intmod(vsize,64)]; 
  T = NULL;
}



bitvectorRRR::~bitvectorRRR() {
  delete [] V;

  if (T) {
    for (uint64_t k=0; k<=bsize; k++) 
      delete T[k];
    delete [] T;
    delete Tbits;
    delete C;
    delete R;
    delete SBsum;
    delete SBidx;
  }
}



uint64_t bitvectorRRR::size() {

  return vsize;
}



/**
   \brief Set all bits in B to 0.
**/
void bitvectorRRR::set0() {

  uint64_t n = intmod(vsize,64); 
  for (uint64_t i=0; i<n; i++) 
    V[i] = 0x0000000000000000;
}



/**
   \brief Set all bits in B to 1.
**/
void bitvectorRRR::set1() {

  uint64_t n = intmod(vsize,64); 
  for (uint64_t i=0; i<n; i++) 
    V[i] = 0xFFFFFFFFFFFFFFFF;
}



/**
   \brief Perform B[i] = 0.
**/
void bitvectorRRR::set0(uint64_t i) {

  V[i/64] &= masks0(i%64);
}



/**
   \brief Perform B[i] = 1.
**/
void bitvectorRRR::set1(uint64_t i) {

  V[i/64] |= masks1(i%64);
}



/**
   \brief Return B[i].
**/
int bitvectorRRR::get(uint64_t i) {

  return (V[i/64] & masks1(i%64)) ? 1 : 0;
}



/**
   \brief Return x where x[63-k+1..63] = B[i..i+k-1].

   In other words, copy the bits i,i+1,...,i+k-1 of this bit-sequence
   onto the k least significant bits of an uint64_t an return it.
**/
uint64_t bitvectorRRR::get(uint64_t i, unsigned k) {

  uint64_t fword = i;
  uint64_t sword = fword+k-1;
  uint8_t fbit = fword % 64;
  uint8_t lbit = sword % 64;
  fword /= 64;
  sword /= 64;

  if (sword == fword) {
    return (V[fword] & mask01[fbit] & ~mask01[lbit+1]) >> (64-lbit-1);
  }
  else {
    uint64_t l = (V[fword] & mask01[fbit]) << (lbit+1);
    uint64_t r = (V[sword] & ~mask01[lbit+1]) >> (64-lbit-1);
    return l|r;
  }
}



/**
   \brief Replace the bitvector with the data structures for rank/select/access.
   Using set0, set1, get after preprocessing will break.
**/
void bitvectorRRR::preprocess() {

  uint64_t bnum = intmod(vsize,bsize);  // # of blocks
  uint64_t snum = intmod(vsize,ssize*bsize);  // # of superblocks

  uint64_t words = intmod(vsize,64); //(vsize+63)/64;

  // Reset excess bits in V, i.e. in the last word of V:
  unsigned slack = words*64-vsize_req;

  if (slack <= 64)
    V[((vsize+63)/64)-1] &= ~mask01[vsize_req%64];
  else {
    V[((vsize+63)/64)-2] &= ~mask01[vsize_req%64];
    V[((vsize+63)/64)-1] = 0x0000000000000000;
  }    

  // Create T, empty, and counters for the size of each row of T, zeroed:
  T = new uvector*[bsize+1]; 
  unsigned* Tsize = new unsigned[bsize+1];  
  
  T[0] = new uvector(1,bsize);
  Tsize[0] = 0;
  
  double c = 1;
  for (int k=1; k<bsize; k++) {
    c = c * ((bsize+1-k)/(double) k);
    T[k] = new uvector((unsigned) c, bsize);
    Tsize[k] = 0;
  }

  T[bsize] = new uvector(1,bsize);
  Tsize[bsize] = 0;

  // A map from each bit pattern of lenght bsize to its rank-in-class:
  unsigned bound = 1U << bsize;
  uvector* bits2rank = new uvector(bound,bsize);
  
  // Fill T and bits2rank:
  for (unsigned i=0; i<bound; i++) {
    int nbits = popcount(i);
    T[nbits]->set(Tsize[nbits],i);
    bits2rank->set(i,Tsize[nbits]);
    Tsize[nbits]++;
  }
  
  // Set Tbits:
  Tbits = new uvector(bsize+1,ceil(log2(Tsize[(bsize+1)/2]+1)));  

  Tbits->set(0,1);
  
  for (int k=1; k<bsize; k++) 
    Tbits->set(k,(uint64_t) ceil(log2(Tsize[k])));

  Tbits->set(bsize,1);

  // Eval the length of R and the number of 1s in V:
  uint64_t rlen = 0;
  uint64_t ones = 0;
  for (uint64_t i=0; i<bnum; i++) {
    unsigned o = popcount(get(i*bsize,bsize));
    rlen += Tbits->get(o);
    ones += o;
  }
  
  // Traverse V by blocks, filling C, R, SBsum and SBidx:
  C = new uvector(bnum,bsize);
  R = new bitsequence(rlen,1.5);

  SBsum = new uvector(snum+1,(unsigned) ceil(log2(ones+1)));
  SBidx = new uvector(snum+1,(unsigned) ceil(log2(rlen+1)));
  
  uint64_t sum = 0;
  uint64_t index = 0;

  SBsum->set(0,0);
  SBidx->set(0,0);
  
  uint64_t i;
  for (i=0; i<bnum; i++) {

    if (i>0 && i%ssize == 0) {
      SBsum->set(i/ssize,sum);
      SBidx->set(i/ssize,index);
    }

    uint64_t x = get(i*bsize,bsize); // The i-th block.
    uint64_t k = popcount(x);

    C->set(i,k);
    R->append(bits2rank->get(x),Tbits->get(k));

    sum += k;
    index += Tbits->get(k);
  }

  SBsum->set(snum,sum);
  SBidx->set(snum,index);
  
  delete [] Tsize;
  delete bits2rank;
  // delete [] V;
}




unsigned bitvectorRRR::access(uint64_t i) {

  uint64_t sb = i/(ssize*bsize);
  uint64_t rpos = SBidx->get(sb);

  uint64_t blk = i/bsize;
  unsigned bits = i%bsize;
  unsigned c;

  uint64_t b;
  for (b=sb*ssize; b<blk; b++) {
    c = C->get(b);
    rpos += Tbits->get(c);
  }

  c = C->get(b);
  uint64_t classrank = R->get(rpos,Tbits->get(c));
  uint64_t block = T[c]->get(classrank);

  return (block & masks1(64-bsize+bits) ? 1 : 0);
}




uint64_t bitvectorRRR::rank1(uint64_t i) {

  // From superblock:
  unsigned sb = i/(ssize*bsize);
  uint64_t rank = SBsum->get(sb);
  uint64_t rpos = SBidx->get(sb);

  // From blocks:
  uint64_t b = i/bsize;
  unsigned bits = i%bsize;
  unsigned c;
  uint64_t k;
  
  if (bits == bsize-1) {
    // i matches a block:
    for (k=sb*ssize; k<=b; k++) {
      c = C->get(k);
      rank += c;
    }
  }
  else {
    // From whole blocks:
    for (k=sb*ssize; k<b; k++) {
      c = C->get(k);
      rank += c;
      rpos += Tbits->get(c);
    }

    // From the last, partial block:
    c = C->get(k);
    unsigned classrank = R->get(rpos,Tbits->get(c));
    uint64_t block = T[c]->get(classrank);
    block >>= (bsize-bits-1);
    rank += popcount(block);
  }
 
  return rank;
}




/*
  A binary search to get the leftmost positon of V with value larger
  than or equal to x.
*/
uint64_t bitvectorRRR::lbsearch(uvector* V, uint64_t x) {

  uint64_t left = 0;
  uint64_t right = V->size()-1;
  uint64_t middle;

  while (left < right) {
    middle = (left+right)/2;
    if (x > V->get(middle))
      left = middle + 1;
    else
      right = middle;
  }
  
  if (V->get(left) >= x)
    return left;
  else
    return UINT64_MAX;
}




uint64_t bitvectorRRR::select1(uint64_t j) {

  uint64_t sb = lbsearch(SBsum,j);

  if (sb == UINT64_MAX)
    return vsize_req;

  sb--;
  
  uint64_t pos = sb*ssize*bsize;
  uint64_t ones = SBsum->get(sb);
  uint64_t rpos = SBidx->get(sb);

  uint64_t k = (sb)*ssize;
  unsigned c, tbits;
  while (ones < j) {
    c = C->get(k);
    ones += c;
    tbits = Tbits->get(c);
    rpos += tbits;
    pos += bsize;
    k++;
  }

  ones -= c;
  rpos -= tbits;
  pos -= bsize;
  k--;

  c = C->get(k);
  unsigned classrank = R->get(rpos,Tbits->get(c));
  uint64_t block = T[c]->get(classrank);

  block <<= (64-bsize);
  
  while (ones < j) {
    ones += (block & 0x8000000000000000 ? 1 : 0);
    pos++;
    block <<= 1;
  }

  return pos-1;
}




#if DEBUG

/**
   \brief Print the bit vector on the screen.
**/
void bitvectorRRR::print() {
  
  int i,j;

  uint64_t bnum = intmod(vsize,bsize); //(vsize+bsize-1)/bsize;
  uint64_t snum = intmod(bnum,ssize); //(bnum+ssize-1)/ssize;

  uint64_t n = (bsize*bnum+63)/64;

  printf("\nbitvectorRRR size_req=%lu, size=%lu words=%lu blk size=%u blk num=%lu SB size=%u SB num=%lu\n",
	 vsize_req,vsize,n,bsize,bnum,ssize,snum);
  
  n *= 64;
  
  // ruler:


  for (i=0; i<n; i++) {
    if (i%10 == 0)
      printf("%d",(i/100)%100);
    else
      printf(" ");
    if (i%bsize == bsize-1)
      printf(" ");
  }
  printf("\n");

  for (i=0; i<n; i++) {
    if (i%10 == 0)
      printf("%d",(i/10)%10);
    else
      printf(" ");
    if (i%bsize == bsize-1)
      printf(" ");
  }
  printf("\n");
  
  for (i=0; i<n; i++) {
    printf("%d",i%10);
  
    if (i%bsize == bsize-1)
      printf(" ");
  }
  printf("\n");

  // A:
  for (i=0; i<n; i++) {
    printf("%c",'0'+get(i));

    if (i%bsize == bsize-1) {
      printf(" ");
    }
  }
  printf("\n\n");


  if (T) {
    printf("T\n");
    for (int k=0; k<=bsize; k++) {
      printf("%d: ",k);
      T[k]->print();
    }
    printf("\n");

    printf("Tbits\n");
    Tbits->print();
    printf("\n");
  
    printf("C\n");
    C->print();
    printf("\n");
  
    printf("R\n");
    R->print();

    printf("SBsum\n");
    SBsum->print();
    printf("\n");
  
    printf("SBidx\n");
    SBidx->print();
    printf("\n");
  }
}


void bitvectorRRR::set_random() {

  uint64_t bnum = intmod(vsize,bsize); //(vsize+bsize-1)/bsize;
  uint64_t n = intmod(bnum*bsize,64); //((bnum*bsize)+63)/64;
  n *= 4;

  uint16_t* p = (uint16_t*) V;
  for (uint64_t i=0; i<n; i++) 
    p[i] = rand() % 65536;
}



/**
   \brief Return rank1(B,i), the number of ones in B[0..i].
**/
uint64_t bitvectorRRR::rank1_raw(uint64_t i) {

  uint64_t r = 0;

  int j = 0;
  for ( ; j<i/64; j++) 
    r += std::popcount(V[j]);

  r += std::popcount(V[j] & mask10[i%64+1]);
  
  return r;
}



/**
   \brief Return select1(B,i), the index of the i-th one in B,
   or |B| if there are not i ones in B.
**/
uint64_t bitvectorRRR::select1_raw(uint64_t j) {

  uint64_t w = (63+vsize_req)/64;
  uint64_t zeros = 0;

  int64_t i = 0;
  while (i<w && zeros<j) {
    zeros += std::popcount(V[i]);
    i++;
  }

  if (i == w  && zeros < j)
    return vsize_req;

  i--;
  zeros -= std::popcount(V[i]);

  w = V[i];
  i = i*64-1;
  while (zeros<j && i<(int64_t)vsize_req) {
    zeros += (w & 0x8000000000000000 ? 1 : 0);
    i++;
    w <<= 1;
  }

  return i;
}


#endif
