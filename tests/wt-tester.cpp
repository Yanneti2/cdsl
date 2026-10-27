#include "wt_implicit.hpp"
#include "bitvector.hpp"
#include "wt.hpp"

#include <iostream>

using namespace std;

int main (void){
	// CONSTRUCTION AND ACCESS TESTS:

	WaveletTree* wt = new WaveletTree("mississippi");

	cout << endl <<"Print Function: " << endl << endl;
	wt->print(); 
	cout << endl << "Acesso em 2: " << (char)wt->access(2) // s
		 << endl << "Acesso em 4: " << (char)wt->access(4) // i
		 << endl << "Acesso em 8: " << (char)wt->access(8) // p
		 << endl << endl;

	// RANK TESTS:

	cout << "Rank(S,6): " << wt->rank('s',6) << endl;	// 3
	cout << "Rank(I,6): " << wt->rank('i',7) << endl;	// 2
	cout << "Rank(I,10): "<< wt->rank('i',10) << endl; // 3
	cout << "Rank(I,11): "<< wt->rank('i',11) << endl; // 4
	cout << endl;

	// SELECT TESTS:

	cout << "Select(S,3): " << wt->select('s', 3) << endl;
	cout << "Select(I,4): " << wt->select('i', 4) << endl;
	// cout << "Select(I,10); " << wt->selectc('i', 10, wt->getRoot()) << endl;

	WaveletTree* wt2 = new WaveletTree("mississippilllllllllllllllll");
	cout << "Select(L,0): " << wt2->select('l', 0) << endl;
	cout << "Select(L,1): " << wt2->select('l', 1) << endl;
	cout << "Select(S,4): " << wt2->select('s', 4) << endl << endl;

	delete wt;

	cout << "mississippi\n\n";

	WaveletTreeImplicit* wti = new WaveletTreeImplicit("mississippi");
	wti->print();

	cout << endl << "Acesso em 2: " << (char)wti->access(2) // s
		 << endl << "Acesso em 4: " << (char)wti->access(4) // i
		 << endl << "Acesso em 8: " << (char)wti->access(8) // p
		 << endl << endl;

	cout << "Rank(S,6): " << wti->rank('s',6) << endl;  // 3
	cout << "Rank(I,6): " << wti->rank('i',7) << endl;  // 2
	cout << "Rank(I,10): "<< wti->rank('i',10) << endl; // 3
	cout << "Rank(I,11): "<< wti->rank('i',11) << endl; // 4
	cout << endl;

	cout << "Select(L,0): " << wti->select('l', 0) << endl;
	cout << "Select(L,1): " << wti->select('l', 1) << endl;
	cout << "Select(M,1): " << wti->select('m', 1) << endl;
	cout << "Select(I,4): " << wti->select('i', 4) << endl;
	cout << "Select(S,4): " << wti->select('s', 4) << endl << endl;

	delete wti;

	cout << "\nabracadabra\n\n";

	WaveletTreeImplicit* wtii = new WaveletTreeImplicit("abracadabra");
	wtii->print();
	delete wtii;

	return 0;
}
