/*
  Original by G.P. Telles, 2013-2016.
  Edited in March 2026.
*/

/*
    TODO:
    Uma funcao para appendar um long e/ou uma string a um bitvector. 
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <cassert>
#include "bitvector.h"
#include <iostream>
using namespace std;

#if IS32BIT
#define mask32(i) mask[(i)]
#else IS64BIT
#define mask64(i) mask[(i)]
#endif

uint32_t mask32[] = {
  0xFFFFFFFF,0x7FFFFFFF,0x3FFFFFFF,0x1FFFFFFF,
  0x0FFFFFFF,0x07FFFFFF,0x03FFFFFF,0x01FFFFFF,
  0x00FFFFFF,0x007FFFFF,0x003FFFFF,0x001FFFFF,
  0x000FFFFF,0x0007FFFF,0x0003FFFF,0x0001FFFF,
  0x0000FFFF,0x00007FFF,0x00003FFF,0x00001FFF,
  0x00000FFF,0x000007FF,0x000003FF,0x000001FF,
  0x000000FF,0x0000007F,0x0000003F,0x0000001F,
  0x0000000F,0x00000007,0x00000003,0x00000001,
  0x00000000
};

uint64_t mask64[] = {
  0xFFFFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFFF,0x1FFFFFFFFFFFFFFF,
  0x0FFFFFFFFFFFFFFF,0x07FFFFFFFFFFFFFF,0x03FFFFFFFFFFFFFF,0x01FFFFFFFFFFFFFF,
  0x00FFFFFFFFFFFFFF,0x007FFFFFFFFFFFFF,0x003FFFFFFFFFFFFF,0x001FFFFFFFFFFFFF,
  0x000FFFFFFFFFFFFF,0x0007FFFFFFFFFFFF,0x0003FFFFFFFFFFFF,0x0001FFFFFFFFFFFF,
  0x0000FFFFFFFFFFFF,0x00007FFFFFFFFFFF,0x00003FFFFFFFFFFF,0x00001FFFFFFFFFFF,
  0x00000FFFFFFFFFFF,0x000007FFFFFFFFFF,0x000003FFFFFFFFFF,0x000001FFFFFFFFFF,
  0x000000FFFFFFFFFF,0x0000007FFFFFFFFF,0x0000003FFFFFFFFF,0x0000001FFFFFFFFF,
  0x0000000FFFFFFFFF,0x00000007FFFFFFFF,0x00000003FFFFFFFF,0x00000001FFFFFFFF,
  0x00000000FFFFFFFF,0x000000007FFFFFFF,0x000000003FFFFFFF,0x000000001FFFFFFF,
  0x000000000FFFFFFF,0x0000000007FFFFFF,0x0000000003FFFFFF,0x0000000001FFFFFF,
  0x0000000000FFFFFF,0x00000000007FFFFF,0x00000000003FFFFF,0x00000000001FFFFF,
  0x00000000000FFFFF,0x000000000007FFFF,0x000000000003FFFF,0x000000000001FFFF,
  0x000000000000FFFF,0x0000000000007FFF,0x0000000000003FFF,0x0000000000001FFF,
  0x0000000000000FFF,0x00000000000007FF,0x00000000000003FF,0x00000000000001FF,
  0x00000000000000FF,0x000000000000007F,0x000000000000003F,0x000000000000001F,
  0x000000000000000F,0x0000000000000007,0x0000000000000003,0x0000000000000001,
  0x0000000000000000
};

/**
   A new, empty bitsequence.

   \param capacity The initial capacity (number of bitVector).
   \param growth_ratio 
**/

// Initializes a bitvector instance 
bitVector::bitVector(unsigned long capacity, float growth_ratio) {

    assert(sizeof(TYPE) * 8 == NBITS);

    cap = (capacity == 0 ? 0 : (capacity+(NBITS-1))/NBITS);
    len = 0;
    ratio = growth_ratio;

    A = (unsigned long*) calloc(cap,sizeof(unsigned long));
    if (!A)
        throw new bad_alloc();
}

// Frees the memory dinamically alocated to store the bitvector
bitVector::~bitVector() {
    free(A);
}

// TODO: change this funtion to be a method of 'bitVector', does it really need to be a method?

// allocates new space for the bitvector, returns 1 if sucess
int bitVector::grow(unsigned long ncap) {
    TYPE* AA = (TYPE*) realloc(A, ncap * sizeof(TYPE));
    if (!AA)
        throw new bad_alloc();
    for (TYPE i = cap; i < ncap; i++) 
        AA[i] = 0;
    A = AA;
    cap = ncap;
    return 1;
}

/**
   \brief Set A[i] to 1.

   If i > |A| - 1 then the behavior is undefined.
**/
void bitVector::set1(unsigned long i) {

    A[i / NBITS] |= ((TYPE)1 << (NBITS - 1)) >> (i % NBITS);
}

/**
   \brief Set A[i] to 0.

   If i > |A| - 1 then the behavior is undefined.
**/
void bitVector::set0(unsigned long i) {

    A[i / NBITS] &= ~(((TYPE)1 << (NBITS - 1)) >> (i % NBITS));
}

/**
   \brief Return 0 if A[i] == 0 or 1 if A[i] == 1.

   If i > |A|-1 then the behavior is undefined.
**/
int bitVector::access(unsigned long i) {

    return (A[i / NBITS] & (((TYPE)1 << (NBITS - 1)) >> (i%NBITS))) ? 1 : 0;
}

/**
   Add 0 to the end of A and increase |A| by one.
**/
void bitVector::append0() {

    if (len == NBITS * cap)
        grow(cap * ratio);
    len++;
}

/**
   Add 1 to the end of A and increase |A| by one.
**/
void bitVector::append1() {

    if (len == NBITS*cap)
        grow(cap * ratio);

    set1(len++);
}

unsigned long bitVector::getLength() { return len; }
unsigned long bitVector::getCap() { return cap; }

// Criar um utils?? colocar na endian????
unsigned long bitVector::ceil(unsigned long ul) { return (ul + NBITS - 1) / NBITS; }

/**
    Coloca uma sequencia predefinida de bits ao final do bitvector.
**/
void bitVector::extend(bitVector* B) {
    while(!(this->ceil(len + B->getLength()) <= cap))
        grow(cap * ratio);   

    TYPE bitsSobrando = len % NBITS;
    TYPE bitsFaltando = NBITS - bitsSobrando;
    TYPE cur = len/ NBITS;

    for (TYPE i = 0; i < B->ceil(B->getLength()); i++) {
         if (bitsSobrando == 0) {
            this->A[cur] = B->A[i];
            cur++;
        } else {
            this->A[cur] |= B->A[i] >> bitsSobrando;
            cur++;
            this->A[cur] |= B->A[i] << bitsFaltando;
        }
    }
    this->len += B->getLength();
}

/**
   \brief Print the bit array on the screen.
**/
void bitVector::print() {
    printf("len: %ld, cap: %ld, ratio: %f\n",len,cap,ratio);
    for (TYPE i=0; i< len; i++) {
        printf("%d",bitVector::access(i));
    }
    printf("\n\n");
}
