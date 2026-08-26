/**
   \file uarray.h

   \brief An array of unsigned integers with 1 to 64 bits.

   An array of unsigned integers having from 1 to 64 bits each.
   Of course with 8, 16, 32 or 64 bits you shouldn't use this.

   \internal guilherme p. telles, 2016.
**/


#include "abbrevs.h"

#ifndef UARRAYH
#define UARRAYH


/**
   \brief The array.
*/
struct uarray {

  u64* V;
  u64 n;
  u8 b;

};

typedef struct uarray uarray;


uarray* ua_alloc(u64 size, u8 bits);
uarray* ua_allocz(u64 size, u8 nbits);
void ua_free(uarray* A);

void ua_put(uarray* A, u64 pos, u64 value);
u64 ua_get(uarray* A, u64 pos);

#endif
