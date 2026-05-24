// Guilherme P. Telles

#include "endian.h"
#include <stdint.h>

// returns 0 if your system is little-endian and 1 if bigendian
int is_bigendian() {
    int i = 1;
    if (((char*) &i)[0] == 1)
        return 0;
    else
        return 1;
}

// transforms a 32 bit integer from little to big-endian and vice versa
int32_t reverse32(int32_t x) {
    int8_t* px = (int8_t*) &x;
    int32_t y;
    int8_t* py = (int8_t*) &y;
    py[0] = px[3];
    py[1] = px[2];
    py[2] = px[1];
    py[3] = px[0];
    return y;
}

// transforms a 64 bits integer from little to big-endian and vice versa
int64_t reverse64(int64_t x) {
    int8_t* px = (int8_t*) &x;
    int64_t y;
    int8_t* py = (int8_t*) &y;
    py[0] = px[7];
    py[1] = px[6];
    py[2] = px[5];
    py[3] = px[4];
    py[4] = px[3];
    py[5] = px[2];
    py[6] = px[1];
    py[7] = px[0];
    return y;
}

// transforms a  64 bit decimal (double) from little to big-endian and vice versa
double reversedouble(double x) {
    char* px = (char*) &x;
    double y;
    char* py = (char*) &y;
    if (sizeof(double) == 8) {
        py[0] = px[7];
        py[1] = px[6];
        py[2] = px[5];
        py[3] = px[4];
        py[4] = px[3];
        py[5] = px[2];
        py[6] = px[1];
        py[7] = px[0];
    } else {
        py[0] = px[3];
        py[1] = px[2];
        py[2] = px[1];
        py[3] = px[0];
    }
    return y;
}

// transforms a 32 bit decimal (float) from big to little-endian and vice versa
float reversefloat(float x) {
    char* px = (char*) &x;
    float y;
    char* py = (char*) &y;
    py[0] = px[3];
    py[1] = px[2];
    py[2] = px[1];
    py[3] = px[0];
    return y;
}

// Source - https://stackoverflow.com/a/2602885
// Posted by sth, modified by community. See post 'Timeline' for change history
// Retrieved 2026-03-30, License - CC BY-SA 4.0

// char = 1 byte
unsigned char reversechar(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
