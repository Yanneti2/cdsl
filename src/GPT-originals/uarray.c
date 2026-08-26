/*
  Guilherme P. Telles, 2016.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "../../include/GPT-originals/uarray.h"



u64 mask[] = {
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

#define mask(i) mask[(i)]

// #define mask(i) ((i)<64?(~0ULL)>>(i):0ULL)


// Fiz um teste pequeno para avaliar a diferenca de desempenho entre armazenar e
// computar a mascara.

// O experimento:
// Para n entre 100799 e 101149, para bits entre 1 e 64, gera-se uma permutacao
// I[0..n-1] e um array A[0..n-1] de números aleatórios e insere-se todos os n
// números em um uarray e depois recupera-se cada um, na ordem dada por I.
// Média de 10 execuções.

// armazenar: 0m55.746s
// calcular:  0m59.773s


/**
   \brief Allocate a new uarray.

   \param size The number of elements of the array.
   \param bits The number of bits of each element, in the range [1,64].

   \returns On success it returns a pointer to a new uarray.
   On failure, it returns null and errno remains as set by malloc().
**/
uarray* ua_alloc(u64 size, u8 bits) {

  uarray* A = malloc(sizeof(uarray));
  if (!A) return 0;

  A->V = malloc((size*bits/64+1)*sizeof(u64));
  if (!A->V) {
    int e = errno;
    free(A);
    errno = e;
    return 0;
  }

  A->n = size;
  A->b = bits;

  return A;
}



/**
   \brief Allocate a new uarray with elements initialized to 0.

   \param size The number of elements of the array.
   \param nbits The number of bits of each element, in the range [1,64].

   \returns On success it returns a pointer to a new uarray.  On failure, it
   returns null and errno remains as set by malloc().
**/
uarray* ua_allocz(u64 size, u8 bits) {

  uarray* A = malloc(sizeof(uarray));
  if (!A) return 0;

  A->V = calloc(size*bits/64+1,sizeof(u64));
  if (!A->V) {
    int e = errno;
    free(A);
    errno = e;
    return 0;
  }

  A->n = size;
  A->b = bits;

  return A;
}



/**
   \brief Release a uarray.
**/
void ua_free(uarray* A) {

  if (A) {
    free(A->V);
    free(A);
  }
}



/**
   \brief Assign value to A[pos].

   \param value The value to store in the array.
   If value has more bits than those specified at A's creation then
   only the least significant will be preserved.
**/
void ua_put(uarray* A, u64 pos, u64 value) {

  u64 fword = pos*A->b;
  u64 sword = fword+A->b-1;
  u8 fbit = fword;
  u8 lbit = sword;

  fword /= 64;
  sword /= 64;
  fbit %= 64;
  lbit %= 64;

  //char b[65];
  //u64toa(mask(fbit),b);
  //printf("%d %s\n",(int)fbit,b);
  //u64toa(mask(lbit+1),b);
  //printf("%d %s\n",(int)lbit+1,b);

  if (sword == fword) {
    A->V[fword] &= ~(mask(fbit)) | mask(lbit+1);
    A->V[fword] |= value << (64-lbit-1);
  }
  else {
    A->V[fword] &= ~mask(fbit);
    A->V[fword] |= value >> (lbit+1);

    A->V[sword] &= mask(lbit+1);
    A->V[sword] |= value << (64-lbit-1);
  }
}



/**
   \brief Return A[pos].
**/
u64 ua_get(uarray* A, u64 pos) {

  u64 fword = pos*A->b;
  u64 sword = fword+A->b-1;
  u8 fbit = fword;
  u8 lbit = sword;

  fword /= 64;
  sword /= 64;
  fbit %= 64;
  lbit %= 64;

  if (sword == fword) {
    return (A->V[fword] & (mask(fbit) & ~mask(lbit+1))) >> (64-lbit-1);
  }
  else {
    u64 l = (A->V[fword] & mask(fbit)) << (lbit+1);
    u64 r = (A->V[sword] & ~mask(lbit+1)) >> (64-lbit-1);
    return l|r;
  }
}



/*
char* u64toa(u64 a, char* buffer) {

  buffer += 64;
  *buffer-- = 0;

  int i;
  for (i = 63; i >= 0; i--) {
    *buffer-- = (a & 1) + '0';
    a >>= 1;
  }

  return buffer+1;
}



void print(uarray* UA) {

  int i,j;
  char *buff = calloc(29,sizeof(char));

  printf("n %lu bits %u max value %lu\n",
         (unsigned long) UA->n, (int) UA->b,(unsigned long)(((u64)1)<<UA->b)-1);

  //for (i=0; i<UA->n; i++)
  //  printf("%lu ",(u64)ua_get(i,UA));
  //printf("\n");

  for (j=0; j<(UA->n*UA->b)/64+1; j++) {

    u8* p = (u8*) &(UA->V[j]);
    p += 7;

    for (i=0; i<8; i++) {
      u8toa(*p,buff);
      printf("%s ",buff);
      p--;
    }
  }

  printf("\n");

  for (i=0; i<UA->n*UA->b; i++) {
    printf("%d",(i/10)%10);

    if (i && (i+1)%8 == 0)
      printf(" ");

  }
  printf("\n");

  for (i=0; i<UA->n*UA->b; i++) {
    printf("%d",i%10);

    if (i && (i+1)%8 == 0)
      printf(" ");
  }
  printf("\n");

  free(buff);
}
*/
