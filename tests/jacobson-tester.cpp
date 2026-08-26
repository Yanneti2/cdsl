#include "../include/jacobsonrank.h" 
#include "../include/bitvector.h"
#include <iostream>
#include <chrono>
#include <math.h>
#include <cstdlib>

int main(void){
    // for (long long unsigned size = 1000; size < 100000; size *= 10) {

    for (long long unsigned size = 10; size < 100; size *= 10) {
		auto B1 = bitVector();
		int ordem = log10(size);

		for(TYPE i = 0; i < size; i++){
		    if(rand() % 2 == 0){
			B1.append1();
		    }else{
			B1.append0();
		    }
		}
        B1.print();
        B1.JacobsonRank_build();
        B1.build_select0();
		cout << "Size bitvector: " << B1.size() << endl;
		for (int i = 1; i < size; i++)
		{   
            cout << flush;
		    unsigned long long foo = 1 + (rand() % (4 * size / 10));
		    // R1.select1(&B1, foo);
            // cout << "foo: " << foo << "\n";
            cout << "naive rank0(" << i << "): " << B1.naive_rank0(i) << "\n";
            cout << "naive rank1(" << i << "): " << B1.naive_rank1(i) << "\n";
            
		}
	}

}
