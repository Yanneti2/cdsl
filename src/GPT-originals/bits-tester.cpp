
#include <iostream>
//using std::cout;

using namespace std;

//#include <cstddef>
//#include <cstring>
//#include <cinttypes>

#include "GPT-originals/bits.h"


int is_bigendian() {

  unsigned i = 1;

  if (*((unsigned char*)&i) == 1)
    return 0;
  else
    return 1;
}



int* rand_perm(int n) {

  int* pi = (int*) malloc(n*sizeof(int));
  if (!pi)
    return 0;

  int i;
  pi[0] = 0;
  for (i=1; i<n; i++) {
    int r = rand() % (i+1);
    if (r != i)
      pi[i] = pi[r];
    pi[r] = i;
  }

  return pi;
}



void die(string s, int i, int j) {

  cout << s << " " << i << " " << j << "\n";
  exit(0);
}


void printA(int* A, int n) {

  int i;
  for (i=0; i<n; i++) {
    printf("%d",A[i]);
    //if (i%8 == 7) {
    //  printf(" ");
    //}
  }
  printf("\n");
}



int check(bits* B, int* A, int n) {

  for (int j=0; j<n; j++) {
    if (B->test(j)) {
      if (A[j] == 0) {
	printf("A[%d] and B[%d] differ.\n",j,j);
	return 0;
      }
    }
    else {
      if (A[j] == 1) {
	printf("A[%d] and B[%d] differ.\n",j,j);
	return 0;
      }
    }
  }
  return 1;
}












int mainz() {

  // testa set e reset sequencial de todos os bits, compara contra um array de ints.

  int i,j,n = 1023;

  bits* B = new bits(n,2.0);
  int* A = new int[n];

  for (i=0; i<n; i++) {

    B->set(i);
    A[i] = 1;

    //print(B);
    //printf("\n");

    for (j=0; j<n; j++) {
      if (B->test(j)) {
        if (A[j] == 0)
          die("A[%d] and B[%d] differ.\n",j,j);
      }
      else {
        if (A[j] == 1)
          die("A[%d] and B[%d] differ.\n",j,j);
      }
    }

    B->reset(i);
    A[i] = 0;

    //print(B);
    //printf("\n");

    for (j=0; j<n; j++) {
      cout << j << "\n";
      if (B->test(j)) {
        if (A[j] == 0)
          die("A[%d] and B[%d] differ.\n",j,j);
      }
      else {
        if (A[j] == 1)
          die("A[%d] and B[%d] differ.\n",j,j);
      }
    }
  }

  cout << "ok\n";

  return 0;
}






// int put() {
//
//   // Teste para put
//   // Em loop, escolhe k e i e compara o bitarray contra um vetor de ints.
//
//   int i,j;
//
//   int n = 63;
//   int plen = 20;
//
//   bitarray *A = alloc(n);
//
//   bitarray *P = alloc(plen);
//   set(0,P);
//   set(2,P);
//   set(3,P);
//   set(5,P);
//   set(6,P);
//   set(7,P);
//   set(9,P);
//   set(10,P);
//   set(11,P);
//   set(12,P);
//   set(14,P);
//   set(15,P);
//   set(16,P);
//   set(17,P);
//   set(18,P);
//
//   int* Ai = calloc(n,sizeof(int));
//
//   int* Pi = calloc(plen,sizeof(int));
//   Pi[0] = 1;
//   Pi[1] = 0;
//   Pi[2] = 1;
//   Pi[3] = 1;
//   Pi[4] = 0;
//   Pi[5] = 1;
//   Pi[6] = 1;
//   Pi[7] = 1;
//   Pi[8] = 0;
//   Pi[9] = 1;
//   Pi[10] = 1;
//   Pi[11] = 1;
//   Pi[12] = 1;
//   Pi[13] = 0;
//   Pi[14] = 1;
//   Pi[15] = 1;
//   Pi[16] = 1;
//   Pi[17] = 1;
//   Pi[18] = 1;
//   Pi[19] = 0;
//
//
//   while (1) {
//
//     int k = 1 + rand() % plen;
//     int p = rand() % (n-k);
//
//     printf("%d bits at %d\n",k,p);
//
//     put(k,P,p,A);
//
//     for (i=0; i<k; i++) {
//       Ai[p+i] = Pi[i];
//     }
//
//     for (j=0; j<n; j++) {
//       if (test(j,A) != Ai[j]) {
//         print(A);
//         printf("\n");
//
//         printA(Ai,n);
//         printf("\n");
//
//         dies("A[%d] and Ai[%d] differ.\n",j,j);
//       }
//     }
//   }
// }
//
//
//
//
//
//
















// int main_man(int argc, char** argv) {
//
//   int n = 35;
//   int i;
//
//   bitarray *S = alloc(n);
//
//   set(11,S);
//   set(13,S);
//   set(15,S);
//   set(17,S);
//
//   print(S);
//   printf("\n");
//
//
//   printf("sel %d = %d\n",0,select1(0,S));
//   printf("sel %d = %d\n",1,select1(1,S));
//   printf("sel %d = %d\n",2,select1(2,S));
//   printf("sel %d = %d\n",3,select1(3,S));
//   printf("sel %d = %d\n",4,select1(4,S));
//   printf("sel %d = %d\n",5,select1(5,S));
//
//   for (i=0; i<n; i++) {
//     printf("rank %d = %d\n",i,rank1(i,S));
//   }
//
//   return 0;
// }



// // Para valores crescentes de n, testa set, reset, rank e select em todas as
// // posições sequencial e aleatoriamente.
//
// int set_reset_rank_select() {
//
//   char* s = NULL;
//   size_t ssize = 0;
//
//   int n,i,j;
//
//   for (n=1; n<=2049; n++) {
//
//     printf("%d bits %d slots\n",n,(n-1)/64+1);
//
//     bitarray *B = alloc(n);
//     int* A = calloc(n,sizeof(int));
//
//     for (i=0; i<n; i++) {
//       B.set(i);
//       A[i] = 1;
//
//       //toa(B,n,&s,&ssize);
//       //printf("%zu %s\n",ssize,s);
//
//       for (j=0; j<n; j++)
//
//         if (test(B,j)) {
//           if (A[j] == 0)
//             dies("A[%d] and B[%d] differ.\n",j,j);
//
//           if (select1(B,rank1(B,j)) != j) {
//
//             printf("B\n");
//             print(B);
//             printf("\n");
//             printf("\n");
//
//             printf("A\n");
//             printA(A,n);
//             printf("\n");
//             printf("\n");
//
//             printf("rank1(B,%d) = %lu\n",j,rank1(B,j));
//             printf("select1(B,rank1(B,%d)) = %lu\n",j,select1(B,rank1(B,j)));
//
//             dies("\nselect1(rank1(%d,B),B) != %d",j,j);
//           }
//         }
//         else
//           if (A[j] == 1)
//             dies("A[%d] and B[%d] differ.\n",j,j);
//
//       for (j=1; j<=i+1; j++)
//         if (rank1(B,select1(B,j)) != j)
//           dies("rank1(select1(%d,B),B) != %d",j,j);
//
//       //for (j=0; j<n; j++)
//       //  printf(" rank %d %lu\n",j,rank1(j,B));
//       //
//       //for (j=1; j<n+1; j++)
//       //  printf(" select %d %ld\n",j,select(j,B));
//     }
//
//
//     for (i=0; i<n; i++) {
//       reset(B,i);
//       A[i] = 0;
//
//       //toa(B,n,&s,&ssize);
//       //printf("%zu %s\n",ssize,s);
//
//       for (j=0; j<n; j++)
//         if (test(B,j)) {
//           if (A[j] == 0)
//             dies("A[%d] and B[%d] differ.\n",j,j);
//           if (select1(B,rank1(B,j)) != j)
//             dies("a select1(rank1(%d,B),B) != %d",j,j);
//         }
//         else
//           if (A[j] == 1)
//             dies("A[%d] and B[%d] differ.\n",j,j);
//
//       for (j=1; j<=n-i-1; j++)
//         if (rank1(B,select1(B,j)) != j)
//           dies("rank1(select1(%d,B),B) != %d",j,j);
//
//       //for (j=0; j<n; j++)
//       //  printf(" rank %d %lu\n",j,rank1(j,B));
//       //
//       //for (j=1; j<n+1; j++)
//       //  printf(" select %d %ld\n",j,select1(j,B));
//     }
//
//
//     int* pi = rand_perm(n);
//
//     for (i=0; i<n; i++) {
//       set(B,pi[i]);
//       A[pi[i]] = 1;
//
//       //toa(B,n,&s,&ssize);
//       //printf("%zu %s\n",ssize,s);
//
//       for (j=0; j<n; j++)
//         if (test(B,j)) {
//           if (A[j] == 0)
//             dies("A[%d] and B[%d] differ.\n",j,j);
//           if (select1(B,rank1(B,j)) != j)
//             dies("select1(rank1(%d,B),B) != %d",j,j);
//         }
//         else
//           if (A[j] == 1)
//             dies("A[%d] and B[%d] differ.\n",j,j);
//
//       for (j=1; j<=i+1; j++)
//         if (rank1(B,select1(B,j)) != j)
//           dies("rank1(select1(%d,B),B) != %d",j,j);
//
//       //for (j=0; j<n; j++)
//       //  printf(" rank %d %lu\n",j,rank1(B,j));
//       //
//       //for (j=1; j<n+1; j++)
//       //  printf(" select %d %ld\n",j,select1(B,j));
//     }
//
//     free(pi);
//     free(B);
//   }
//   free(s);
//
//   return 0;
// }
//
//
//


void putA(int* A, int i, int* Q, int k) {
  
  for (int j=0; j<k; j++) {
    A[i+j] = Q[j];
  }
}



void put_single_word() {

  int n = 128;

  int A[128] = {0};
  bits* B = new bits(n,2.0);

  for (int i=0; i<128; i++) {
    B->set(i);
    A[i] = 1;
  }

  B->print();
  printA(A,n);

  
  int k = 20;
  int Q[] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
  bits* P = new bits(20,2.0);

  P->set(1);
  P->set(3);
  P->set(5);
  P->set(7);
  P->set(11);
  P->set(13);
  P->set(15);
  P->set(17);
  
  P->print();
  printA(Q,k);

  int i = 1;

  B->put(P,9,60);
  putA(A,60,Q,9);

  printf("oi\n");
  B->print();
  printA(A,n);

  
  
  if (!check(B,A,n)) {
    printf("mismatch\n");
    B->print();
    printA(A,n);
    exit(1);
  }
}





int put_all() {

  // Testa put de P em B, em todas as posições, comparando contra um vetor A.


  for (int n=1; n<=5000; n++) {

    printf("n %d\n",n);
    
    for (int m=1; m<=n; m++) {

      bits* A = new bits(n,2.0);
      
      int* B = (int*) malloc(n*sizeof(int));
      for (int i=0; i<n; i++)
	B[i] = 0;
 
      bits* P = new bits(m,2.0);
      int* Q = (int*) malloc(m*sizeof(int));

      for (int i=0; i<m; i++)
	Q[i] = 0;
    
      for (int i=0; i<m; i+=3) {
	P->set(i);
	Q[i] = 1;
      }

      //A->print();
      //printA(B,n);
      //
      //P->print();
      //printA(Q,m);


      int* pi = rand_perm(n);
   
      for (int i=0; i<n; i++) {

	int k = m>1 ? (rand() % (m-1))+1 : 1;

	if (pi[i]+k >= n)
	  continue;
    
	//printf("put at %d\n",pi[i]);
	//printf("%lu bits\n",k);
    
	A->put(P,k,pi[i]);
    
	for (int j=0; j<k; j++) {
	  B[pi[i]+j] = Q[j];
	}
    
	if (!check(A,B,n)) {
	  printf("put %d bits at %d\n",k,pi[i]);
	  A->print();
	  printA(B,n);
	  exit(1);
	}

	//printf("---------------\n");
	//A->print();
	//printA(B,n);
	//printf("\n----------------\n");
      }

      delete A;
      free(B);
      delete P;
      free(Q);
      free(pi);
    }
  }


  
  cout << "ok\n";
  
  
  return 0;
}



int put() {

  int n = 128;
  bits *A = new bits(n,2.0);

  for (int i=0; i<n; i++) {
    A->reset(i);
  }

  
  int m = 7;
  bits *P = new bits(m,2.0);

  for (int i=0; i<m; i+=1) {
    P->set(i);
  }

  A->print();
  printf("\n");
  
  P->print();
  printf("\n");

  A->put(P,5,61);
  
  A->print();
  printf("\n");
  
  return 0;
}




int append_number(void) {

  int n = 128;

  bits *D = new bits(n,2.0);
  D->print();
  printf("\n");

  D->append(1,1);
  
  return 0;
}





int main(int argc, char** argv) {

  srand(1);

  printf("is big %d\n",is_bigendian());
  
  //put();

  put_all();
  //set_reset_rank_select();
  //put_single_word();
  //append();

  //append_number();
  
  return 0;
}



