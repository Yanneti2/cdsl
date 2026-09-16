/*
  A sequence of bits.
*/


#ifndef BITSH
#define BITSH


class bits {

 private:
  unsigned long* A;
  unsigned long cap;  // The number of words of A.
  unsigned long len;  // The lenght of the bit sequence (logical). 
  float ratio;

 public:
  bits(unsigned long capacity, float growth_ratio);
  ~bits();

  unsigned long length();
 
  void set(unsigned long i);
  void reset(unsigned long i);
  int  test(unsigned long i);
  
  void put(bits* SRC, unsigned long k, unsigned long i);

  void append0();
  void append1();

  void append(unsigned long number, unsigned long k);
  

  void print();
};

#endif
