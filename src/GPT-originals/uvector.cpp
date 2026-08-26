/*
  Guilherme P. Telles, 2016, jul 2024.
  Definitions for uvector.cpp
*/

#include <cstdio>

#include "GPT-originals/uvector.h"
#include "masks/mask01.h"

using namespace std;


/**
   \brief Create a new uvector with n unsigned integers of width bits each.
   It expects width in [1,64]. 
**/
uvector::uvector(uint64_t size, int width) {

  this->vsize = size;
  this->width = width;
  this->V = new uint64_t[(size*width+63)/64];
}



uvector::~uvector() {

  delete [] V;
}



uint64_t uvector::size() {
  return vsize;
}



/**
   \brief Set V[i] = 0 for 0 <= i < n.
**/
void uvector::set0() {

  uint64_t k = (vsize*width+63)/64;
  for (uint64_t i=0; i<k; i++) 
    V[i] = 0x0000000000000000;
}


/**
   \brief Set V[i] = 2^w-1 for 0 <= i < n.
**/
void uvector::set1() {

  uint64_t k = (vsize*width+63)/64;
  for (uint64_t i=0; i<k; i++) 
    V[i] = 0xFFFFFFFFFFFFFFFF;
}



/**
   \brief Perform V[i] = x.
   
   In other words, set V[i] to the w least significant bits in x.
**/
void uvector::set(uint64_t i, uint64_t x) {

  // first bit, last bit:
  uint64_t fword = i*width;   
  uint64_t sword = fword+width-1;  
  uint8_t fbit = fword % 64;  
  uint8_t lbit = sword % 64;

  // first word, second word:
  fword /= 64; 
  sword /= 64;

  // Reset any leftover bit in value:
  x &= mask01[64-width];
  
  if (sword == fword) {
    V[fword] &= (~mask01[fbit] | mask01[lbit+1]);
    V[fword] |= x << (64-lbit-1);
  }
  else {
    V[fword] &= ~mask01[fbit];
    V[fword] |= x >> (lbit+1);
    V[sword] &= mask01[lbit+1];
    V[sword] |= x << (64-lbit-1);
  }
}



/**
   \brief Return V[i].

   In other words, set the w least significant bits of an uint64_t
   with V[i] and return it.
**/
uint64_t uvector::get(uint64_t i) {

  uint64_t fword = i*width;
  uint64_t sword = fword+width-1;
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



#if DEBUG

void uvector::print() {

  printf("size=%lu width=%d : ",vsize,width);
  for (uint64_t i=0; i<vsize; i++) {
    printf("%lu ",get(i));
  }
  printf("\n");
}
  


void uvector::print_bits() {

  int i,j;
  char buff[64];

  unsigned words = (vsize*width+63)/64;
  
  printf("n %lu, bits %hu, max value %lu, words %u\n",
         vsize,
	 width,
	 (static_cast<uint64_t>(1) << width)-1,
	 words);

  printf("A: ");
  for (i=0; i<vsize; i++)
    printf("%lu ", get(i));
  printf("\n");

  
  // Ruler:
  printf(" ");
  for (i=10; i<vsize*width; i+=10) 
    printf("%11d",i/10);
  printf("\n");

  for (i=0; i<vsize*width; i++) {
    printf("%d",i%10);

    if (i && (i+1)%8 == 0)
      printf(" ");
  }
  printf("\n");


  // The bits in msb->lsb order:
  for (i=0; i<words; i++) {
    uint64_t x = V[i];

    for (int j=0; j<64; j++) {

      printf("%d", (x & 0x8000000000000000 ? 1 : 0));
      x <<= 1;
      
      if (j && (j+1)%8 == 0)
	printf(" ");
    }
  }
  printf("\n");
  
  // The bits in the order of bytes in memory:
  //uint8_t* p = (uint8_t*) V;
  //
  //for (j=0; j<8*((words+7)/8); j++) {
  //  u8toa(*p,buff);
  //  printf("%s ",buff);
  //  p++;
  //}
}

#endif
