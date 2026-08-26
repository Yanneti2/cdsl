#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include "endian.h"

char* u8toa(uint8_t a, char* buffer) {

  buffer += 8;
  *buffer-- = 0;

  int i;
  for (i = 7; i >= 0; i--) {
    *buffer-- = (a & 1) + '0';
    a >>= 1;
  }

  return buffer+1;
}



int main(int argc, char** argv) {

  printf("bigendian %d\n",is_bigendian());

  uint32_t a = 0x01030507;
  char *buff = calloc(9,sizeof(char));

  printf("a = %08xx\n",a);

  uint8_t* q = (uint8_t*)  &a;

  int i;
  for (i=0; i<4; i++) {
    u8toa(*q++,buff);
    printf("%s ",buff);
  }

  a = reverse32(a);

printf("\n");
  printf("rev a = %08xx \n",a);

  q = (uint8_t*)  &a;

  for (i=0; i<4; i++) {
    u8toa(*q++,buff);
    printf("%s ",buff);
  }

printf("\n");

  return 0;
}
