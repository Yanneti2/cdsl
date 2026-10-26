#include "bitvector.hpp"

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
    BV.print(false);
    BitVector BV2 = BitVector();
    for (int j = 0; j < 8; j++) {
        BV2.append1();
    }
    std::cout << "BV2:" << std::endl;
    BV2.print(false);

    // Put does not exist anymore
    // BV.put(BV2, 4);
    // std::cout << "BV2 putted into [4] of BV:" << std::endl;
    BV.print(false);

    //----------------------------------
    // 		Constructors	       |
    //----------------------------------

    string s = "11010011101000";
    BitVector* stringg = new BitVector(s);
    assert(stringg->naive_rank1(s.size()) == 7);
    assert(stringg->naive_rank0(s.size()) == 7);

    BitVector* string2 = new BitVector(s);

    //---------------------------
    //	       Operators    	|
    //---------------------------

    // assert(stringg==string2);
    // assert(stringg[4]==string2[4]);

    BitVector B(1000, 1);

    B.set(0b1001, 4, 13);
    B.set(0b1001, 4, 18);
    B.set(0b1001, 4, 62);

    B.print(false);

    //---------------------------
    //	       Extend       	|
    //---------------------------

    cout << endl << "extend start" << endl;

    string test = "100101";
    BitVector* testbv = new BitVector(test);
    BitVector nbv("001");
    nbv.print(false);
    cout << "\nbefore end\n";
    nbv.extend(testbv);
    nbv.print(false);

    cout << "\n\n";
}
