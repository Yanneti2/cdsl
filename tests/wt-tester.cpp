#include "bitvector.h"
#include "wt.h"

#include <iostream>

using namespace std;

int main (void){
	// CONSTRUCTION AND ACCESS TESTS:

	wtNode* root = new wtNode();
	WaveletTree* wt = new WaveletTree("mississippi", root);

	cout << endl <<"Print Function: " << endl << endl;
	wt->print(); 
	cout << endl << "Acesso em 2: " << wt->access(2) << endl << "Acesso em 4: " << wt->access(4) << endl << "Acesso em 8: " << wt->access(8) << endl << endl;

	// RANK TESTS:

	cout << "Rank(S,6): " << wt->rankc('s',6) << endl;
	cout << "Rank(I,6): " << wt->rankc('i',6) << endl << endl;

	// SELECT TESTS:

	cout << "Select(S,3): " << wt->selectc('s', 4, root) << endl;	
	cout << "Select(I,10); " << wt->selectc('i', 4, root) << endl << endl;

	// wt->~wt();

	return 0;
}
