
#include <cstdlib>
#include <cstdio>
#include <cinttypes>

#include "bitvector.h"
#include "bitvectorBS.h"

using namespace std;


int is_bigendian() {

  unsigned i = 1;

  if (*((unsigned char*)&i) == 1)
    return 0;
  else
    return 1;
}


int main(void) {

  for (unsigned s=0; s<=100; s++) {
    srand(s);
  
    for (uint64_t n=32; n<=10000+s; n++) {
  
      printf("%lu\n",n);

      bitvectorBS* S = new bitvectorBS(n);
      S->reset_all();
  
      bitvector* B = new bitvector(n);
      B->set0();
  
      for (uint64_t i=0; i<n; i++) {
	if (rand() % 2) {
	  S->set(i);
	  B->set1(i);
	}
	else {
	  S->reset(i);
	  B->set0(i);
	}
      }

      S->preprocess();
  
      for (uint64_t i=0; i<n; i++) {

	uint64_t bs = S->access(i);
	uint64_t raw = B->get(i);
    
	if (bs != raw) {
	  printf("access n=%lu i=%lu bs=%lu raw=%lu\n\n",n,i,bs,raw);
	  S->print();
	  printf("\n");
	  B->print();
	  exit(1);
	}


	bs = S->rank1(i);
	raw = B->rank1(i);
    
	if (bs != raw) {
	  printf("rank1 n=%lu i=%lu bs=%lu raw=%lu\n\n",n,i,bs,raw);
	  S->print();
	  printf("\n");
	  B->print();
	  exit(1);
	}

	bs = S->rank0(i);
	raw = B->rank0(i);
    
	if (bs != raw) {
	  printf("rank0 n=%lu i=%lu bs=%lu raw=%lu\n\n",n,i,bs,raw);
	  S->print();
	  printf("\n");
	  B->print();
	  exit(1);
	}


	bs = S->select1(i+1);
	raw = B->select1(i+1);
    
	if (bs != raw) {
	  printf("select1 n=%lu i=%lu bs=%lu raw=%lu\n\n",n,i+1,bs,raw);
	  S->print();
	  printf("\n");
	  B->print();
	  exit(1);
	}

	bs = S->select0(i+1);
	raw = B->select0(i+1);
    
	if (bs != raw) {
	  printf("select0 n=%lu i=%lu bs=%lu raw=%lu\n\n",n,i+1,bs,raw);
	  S->print();
	  printf("\n");
	  B->print();
	  exit(1);
	}
      }
    
      delete S;
      delete B;
    }
  }
  
  printf("ok\n");
  return 0;
} 
 
 
