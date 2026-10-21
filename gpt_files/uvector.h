/**
   \file uvector.h
   \internal Guilherme P. Telles, 2016, jul 2024.
   
   \brief An array V of unsigned integers of fixed width between 1 and 64.
**/


#ifndef UVECTOR_H
#define UVECTOR_H

#include <stdint.h>



class uvector {

private:
  uint64_t* V; 
  uint64_t vsize; // The number of unsigned integers in V.
  uint8_t width;  // The number of bits of each one.

public:
  uvector(uint64_t size, int width);
  ~uvector();

  uint64_t size();

  void set0();
  void set1();
  
  void set(uint64_t i, uint64_t x);
  uint64_t get(uint64_t index);

  #if DEBUG
  void print();
  void print_bits();
  #endif
};

#endif
