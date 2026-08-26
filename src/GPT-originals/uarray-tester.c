#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>

#include "abbrevs.h"
#include "../include/GPT-originals/uarray.h"



char* u8toa(u8 a, char* buffer) {

  buffer += 8;
  *buffer-- = 0;

  int i;
  for (i = 7; i >= 0; i--) {
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



u64* rand_perm(u64 n) {

  u64* pi = (u64*) malloc(n*sizeof(u64));
  if (!pi)
    return 0;

  u64 i;
  pi[0] = 0;
  for (i=1; i<n; i++) {
    u64 r = random() % (i+1);
    if (r != i)
      pi[i] = pi[r];
    pi[r] = i;
  }

  return pi;
}



int main0() {

  uarray* UA = ua_allocz(15,5);
  print(UA);

  int i = 2;

  ua_put(UA,i,31);
  print(UA);

  printf("A[%d] = %d\n",(int) i,(int) ua_get(UA,i));
  print(UA);

  return 0;
}



// Para seed crescente, para n crescente, para bits entre 1 e 64 bits, gera uma
// permutacao de [0,n-1] e insere e recupera um aleatório em cada posicao do
// array na ordem da permutacao.

int main() {

  u32 n, bits, seed;
  u32 i, j;

  for (seed=13; seed<23; seed++) {
    srandom(seed);

    //printf("seed %"PRIu32"\n",seed);

    for (n=100799; n<=101149; n+=1) {

      u64* I = rand_perm(n);

      u64* A = (u64*) malloc(n*sizeof(u64));
      for (i=0; i<n; i++)
        A[i] = random();


      for (bits=1; bits<=64; bits++) {

        u64 base = 1;

        if (bits < 64)
          base <<= bits;
        else
          base = -1;

        uarray* UA = ua_allocz(n,bits);

        //printf("n %"PRIu64" bits %"PRIu8" base %"PRIu64"\n",UA->n,UA->b,base);

        for (i=0; i<n; i++) {
          ua_put(UA,I[i],A[I[i]] % base);
          //printf("  put %"PRIu64" at %"PRIu64"\n",A[I[i]]%base,I[i]);
        }

        for (i=0; i<n; i++) {
          if (A[I[i]] % base != ua_get(UA,I[i])) {
            printf("seed %d bits %d max %"PRIu64" n %u \n",seed,bits,base-1,n);
            printf("1 Expected %"PRIu64" got %"PRIu64" at %d\n",A[I[i]],ua_get(UA,I[i]),I[i]);
            print(UA);
            exit(0);
          }
        }

        ua_free(UA);
      }
      free(I);
      free(A);
    }
  }
  return 0;
}



// Para seeds variados, para entre 1 e 64 bits, para n variado, insere e testa valores aleatorios em posicoes aleatorias.
int main2() {

  u32 n,bits,seed;
  u32 i,j;

  for (seed=1; seed<10; seed++) {
    srandom(seed);

    printf("seed %"PRIu32"\n",seed);

    for (bits=1; bits<=63; bits++) {

      for (n=10; n<=100; n+=1) {

        uarray* UA = ua_allocz(n,bits);
        u64* A = calloc(n,sizeof(u64));

        u64 max = 1;
        max <<= bits;

        for (i=0; i<100*n; i++) {
          u32 p = random() % n;
          u64 k = random() % max;

          //printf("put %"PRIu64" at %"PRIu32"\n",k,p);

          A[p] = k;
          ua_put(UA,p,k);

          if (A[p] != ua_get(UA,p)) {
            printf("seed %d bits %d max %"PRIu64" n %u \n",seed,bits,max-1,n);
            printf("1 Expected %"PRIu64" got %"PRIu64" at %d\n",A[p],ua_get(UA,p),p);
            print(UA);
            exit(0);
          }
        }

        for (j=0; j<n; j++) {
          if (A[j] != ua_get(UA,j)) {
            printf("seed %d bits %d max %"PRIu64" n %u \n",seed,bits,max-1,n);
            printf("2 Expected %"PRIu64" got %"PRIu64" at %d\n",A[j],ua_get(UA,j),j);
            print(UA);
            exit(0);
          }
        }

        ua_free(UA);
        free(A);
      }
    }
  }
  return 0;
}
