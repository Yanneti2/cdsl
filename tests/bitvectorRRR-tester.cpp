
#include <iostream>
//using std::cout;

using namespace std;

//#include <cstddef>
//#include <cstring>
//#include <cinttypes>

#include "bitvectorRRR.h"



int main(void) {


  int n = 120;
  bitvectorRRR* B = new bitvectorRRR(n);
  
  //B->set_random();
  B->set1();
  
  B->print();
  printf("\n");
  
  B->preprocess();
  
  B->print();
  printf("\n");
  
  printf("rank(%d)=%lu\n",15,B->rank1(15));
  printf("rank(%d)=%lu\n",16,B->rank1(16));
  printf("rank(%d)=%lu\n",17,B->rank1(17));
  printf("rank(%d)=%lu\n",18,B->rank1(18));
  printf("rank(%d)=%lu\n",19,B->rank1(19));
  printf("rank(%d)=%lu\n",20,B->rank1(20));
  
  
  delete B;

  //for (int j=32; j<=100000; j++) {
  //
  //      printf("%d\n",j);
  //
  //bitvectorRRR* B = new bitvectorRRR(j);
  //B->set_random();
  //B->preprocess();
  //
  //for (int i=0; i<j; i++) {
  //  
  //  uint64_t rrr = B->rank1(i);
  //  uint64_t raw = B->rank1_raw(i);
  //
  //  if (rrr != raw) {
  //    printf("n=%d i=%d rrr=%lu raw=%lu\n",j,i,rrr,raw);
  //    B->print();
  //    exit(1);
  //  }
  //}
  //
  //delete B;
  //}
  //
  //printf("ok\n");
  
}
