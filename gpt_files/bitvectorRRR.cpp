/**
  G.P.Telles, 2026.
  Definitions for bitvector-RRR.h 
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "bitvectorRRR.h"
#include "mask01.h"


using namespace std;

// 64 bit masks with a single 1 at bit i:
#define masks1(i) (0x8000000000000000 >> (i))

// 64 bit masks with a single 0 at bit i:
#define masks0(i) (~(0x8000000000000000 >> (i)))



/**
   \brief Create a new bitvector with size bits.
   Size may not be too small. Size 1 will surely break.
**/
bitvectorRRR::bitvectorRRR(uint64_t size) {

  vsize = size;
  
  bsize = ceil(log2(vsize)/2);
  uint64_t bnum = (vsize+bsize-1)/bsize;

  ssize = (unsigned) floor(log2(vsize));
  
  V = new uint64_t[((bnum*bsize)+63)/64];
  T = NULL;
}



bitvectorRRR::~bitvectorRRR() {
  delete [] V;
}



uint64_t bitvectorRRR::size() {

  return vsize;
}



/**
   \brief Set all bits in B to 0.
**/
void bitvectorRRR::set0() {

  uint64_t n = (vsize+63)/64;
  for (uint64_t i=0; i<n; i++) 
    V[i] = 0x0000000000000000;
}



/**
   \brief Set all bits in B to 1.
**/
void bitvectorRRR::set1() {

  uint64_t n = (vsize+63)/64;
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

  uint64_t bnum = (vsize+bsize-1)/bsize;  // # of blocks
  uint64_t snum = (vsize+(bsize*ssize)-1)/(bsize*ssize);  // # of superblocks
  
  // Reset excess bits in V, i.e. in the last word of V:
  V[(((bnum*bsize)+63)/64)-1] &= ~mask01[vsize%64];
  
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
    int nbits = std::__popcount(i);
    T[nbits]->set(Tsize[nbits],i);
    bits2rank->set(i,Tsize[nbits]);
    Tsize[nbits]++;
  }
  
  // Set Tbits:
  Tbits = new uvector(bsize+1,ceil(log2(Tsize[(bsize+1)/2]+1)));  

  Tbits->set(0,1);
  
  for (int k=1; k<bsize; k++) 
    Tbits->set(k,(uint64_t) ceil(log2(Tsize[k]+1)));

  Tbits->set(bsize,1);

  // Eval the length of R and the number of 1s in V:
  uint64_t rlen = 0;
  uint64_t ones = 0;
  for (uint64_t i=0; i<bnum; i++) {
    unsigned o = std::__popcount(get(i*bsize,bsize));
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
      //printf("set SBsum at %lu to %lu\n",i/ssize,sum);
      SBsum->set(i/ssize,sum);
      SBidx->set(i/ssize,index);
    }

    uint64_t x = get(i*bsize,bsize); // The i-th block.
    uint64_t k = std::__popcount(x);

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



uint64_t bitvectorRRR::rank1(uint64_t i) {

  // From superblock:
  unsigned sb = i/(ssize*bsize);
  uint64_t rank = SBsum->get(sb);
  uint64_t rpos = SBidx->get(sb);

  // From blocks:
  unsigned b = i/bsize;
  unsigned bits = i%bsize;
  unsigned c, k;
  
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
    rank += std::__popcount(block);
  }
 
  return rank;
}



#if DEBUG

/**
   \brief Print the bit vector on the screen.
**/
void bitvectorRRR::print() {
  
  int i,j;

  uint64_t bnum = (vsize+bsize-1)/bsize;
  uint64_t snum = (vsize+(bsize*ssize)-1)/(bsize*ssize); 

  uint64_t n = ((bnum*bsize)+63)/64;

  printf("bitvectorRRR size=%lu words=%lu blk size=%u blk num=%lu SB size SB=%u num=%lu\n",
	 vsize,n,bsize,bnum,ssize,snum);
  
  n *= 64;
  
  // ruler:
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
  printf("\n");


  if (T) {
    printf("T\n");
    for (int k=0; k<=bsize; k++) {
      printf("%d \n",k);
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

  uint64_t bnum = (vsize+bsize-1)/bsize;
  uint64_t n = ((bnum*bsize)+63)/64;
  n *= 4;

  uint16_t* p = (uint16_t*) V;
  for (uint64_t i=0; i<n; i++) 
    p[i] = rand() % 65536;
}

#endif
