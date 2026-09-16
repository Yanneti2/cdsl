/**
   \file endian.h
   \brief Endianness conversions.

   Original by Guilherme P. Telles, Jun 2014.
   Edited in March 2026.
**/

#ifndef ENDIANH
#define ENDIANH

#include <stdint.h>

// returns 0 if your system is little-endian and 1 if bigendian
int is_bigendian();

// transforms a 32 bit integer from little to big-endian and vice versa
int32_t reverse32(int32_t x);

// transforms a 64 bits integer from little to big-endian and vice versa
int64_t reverse64(int64_t x);

// transforms a  64 bit decimal (double) from little to big-endian and vice versa
double reversedouble(double x);

// transforms a 32 bit decimal (float) from big to little-endian and vice versa
float reversefloat(float x);

#endif