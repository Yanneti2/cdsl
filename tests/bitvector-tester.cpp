#include "bitvector.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    //----------------------------------
    // Teste de operacoes Put & Slice  |
    //----------------------------------

    BitVector BV = BitVector();

    for (int j = 0; j < 8; j++)
    {
        if (j%2==1) BV.append1();
        else BV.append0();
    }
    BV.print();
    BitVector BV2 = BitVector();
    for (int j = 0; j < 8; j++) {
        BV2.append1();
    }
    std::cout << "BV2:" << std::endl;
    BV2.print();

    // Put does not exist anymore
    // BV.put(BV2, 4);
    // std::cout << "BV2 putted into [4] of BV:" << std::endl;
    BV.print();

<<<<<<< HEAD
    //----------------------------------
    // 		Constructors	       |
    //----------------------------------

    string s = "11010011101000";
    BitVector* string = new BitVector(s);
    assert(string.naive_rank1(s.size()) == 7);
    assert(string.naive_rank0(s.size()) == 7);

    BitVector* string2 = new BitVector(s);

    //---------------------------
    //		Operators	|
    //---------------------------

    assert(string==string2);
    assert(string[4]==string2[4]);
=======
    bitVector* a = new bitVector(1,2.0);
    for(int i = 0; i < 5; i++){
	    a->append0();
	    a->append1();
    } // 0101010101
    // Select1(5) = 9 Select0(4) = 6
    cout << "Naive Select1(5): " << a->naive_select1(5) << endl << "Naive Select0(4): " <<a->naive_select0(4) << endl << endl;
>>>>>>> 24-wavelet-trees-implementation
}
