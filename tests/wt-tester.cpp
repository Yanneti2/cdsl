#include "bitvector.h"
#include "wt.h"

#include <iostream>

using namespace std;

int main (void){
	// CONSTRUCTION AND ACCESS TESTS:

	WaveletTree* wt = new WaveletTree("mississippi");

	cout << endl <<"Print Function: " << endl << endl;
	wt->print(); 
	cout << endl << "Acesso em 2: " << wt->access(2) // s
		 << endl << "Acesso em 4: " << wt->access(4) // i
		 << endl << "Acesso em 8: " << wt->access(8) // p
		 << endl << endl;

	// RANK TESTS:

	cout << "Rank(S,6): " << wt->rankc('s',6) << endl;	// 3
	cout << "Rank(I,6): " << wt->rankc('i',7) << endl;	// 2
	cout << "Rank(I,10): "<< wt->rankc('i',10) << endl; // 3
	cout << "Rank(I,11): "<< wt->rankc('i',11) << endl; // 4
	cout << endl;

	// SELECT TESTS:

	cout << "Select(S,3): " << wt->selectc('s', 3, wt->getRoot()) << endl;
	cout << "Select(I,4): " << wt->selectc('i', 4, wt->getRoot()) << endl;
	// cout << "Select(I,10); " << wt->selectc('i', 10, wt->getRoot()) << endl;

	WaveletTree* wt2 = new WaveletTree("mississippilllllllllllllllll");
	cout << "Select(L,0): " << wt2->selectc('l', 0, wt2->getRoot()) << endl;
	cout << "Select(L,1): " << wt2->selectc('l', 1, wt2->getRoot()) << endl;
	cout << "Select(S,4): " << wt2->selectc('s', 4, wt2->getRoot()) << endl;

	delete wt;

	return 0;
}
