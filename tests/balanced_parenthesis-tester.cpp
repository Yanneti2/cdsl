/* These operations consider the exclusive rank and select operations previously implemented. That being said, the enclose operation returns the k+1 position of the 0 indexed bitvector position of the starting of a segment that contains that i'th position.
 * 
 * These behaviour makes so that the enclosing of tha last position of the BV return 1, not 0.
 *
 * I suggest a remodel in this operations, comment that was made before my interview in this file, so that it returns the exact 0-indexed position of the bitVector.
 */

#include "../include/bp.h"
#include "../include/bitvector.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    // Creating the BP bitvector
    unsigned long long size = 32;
    bitVector B = bitVector(); 

    char pattern[] = "((())()(())(()())())()((())())()";

    // using sizeof(pattern) -1 to ignore the \0 at end of pattern
    for (int i = 0; i < sizeof(pattern) - 1; i++)
    {
        if (pattern[i] == '(')
        {
            B.append1();
            continue;
        }
        B.append0();
    }

    //---------------------------
    //     Excess Operation     |
    //---------------------------

    // Hand-crafted excesses:
    //
    // 01232121
    // ((())()(
    // 
    // 23212323
    // ())(()()
    // 
    // 21210101
    // )())()((
    // 
    // 23212101
    // ())())()

    unsigned long long excesses[] = { 
        0, 1, 2, 3, 2, 1, 2, 1,
        2, 3, 2, 1, 2, 3, 2, 3,
        2, 1, 2, 1, 0, 1, 0, 1,
        2, 3, 2, 1, 2, 1, 0, 1
    };

    //---------------------------
    //     Is BP Operation     |
    //---------------------------

    // B1 == ()()() == 101010
    bitVector B1 = bitVector();
    for(int i = 0; i < 3; i++){
	    B1.append1();
	    B1.append0();
    }
    assert(is_bp(B1));

    // B2 = ((())) == 111000
    bitVector B2 = bitVector();
    for(int i = 0; i < 3; i++)B2.append1();
    for(int i = 0; i < 3; i++)B2.append0();
    assert(is_bp(B2));

    // B3 = (()()() == 1101010
    bitVector B3 = bitVector();
    B3.append1();
    for(int i = 0; i < 3; i++){
	    B3.append1();
	    B3.append0();
    }
    assert(!is_bp(B3));

    // B4 == ) == 0
    bitVector B4 = bitVector();
    B4.append0();
    assert(!is_bp(B4));

    // B5 = empty
    bitVector B5 = bitVector();
    assert(!is_bp(B5));

    // B6 == ())(() == 100110
    string arantes = "100110";
    bitVector B6 = bitVector(arantes);
    assert(!is_bp(B6));

    // B7 == )))))((((( == 0000011111
    string arantes2 = "0000011111";
    bitVector B7 = bitVector(arantes2);
    assert(!is_bp(B7));

    // B8 == ((()())((((()()()))())((()(())()())))()) 
    string a = "1110100111110101000100111011001010000100";
    bitVector B8 = bitVector(a);
    assert(is_bp(B8));

    // B9 == () == 10
    bitVector B9 = bitVector();
    B9.append1();
    B9.append0();
    assert(is_bp(B9));

    // B10 == ((( == 111
    bitVector B10 = bitVector();
    B10.append1();
    B10.append1();
    B10.append1();
    assert(!is_bp(B10));

    // Checking if our implementation matches the expected hand-crafted excess
    // values array
    for (unsigned long long i = 0; i < size; i++) 
    {
        assert(excesses[i] == excess(&B, i));
    }

    //---------------------------
    //    BWDSearch Operation    |
    //---------------------------

    // i=0: excess(0) = 0. As there is no possible j < i, should return 0.
    assert(backward_search(&B, 0, 1) == 0);

    // i=4: excess(4) = 2, target=1. j=3->3, j=2->2, j=1->1. Expected result=1
    assert(backward_search(&B, 4, -1) == 1);

    // i=9: excess(9) = 3, target=2. j=8->2. Expected result=8
    assert(backward_search(&B, 9, -1) == 8);

    // i=31: excess(31) = 1, target=0. j=30->2. Expected result=30
    assert(backward_search(&B, 31, -1) == 30);

    // i=20: excess=0, target=-1 (impossible) so expected result=0 (sentinel)
    assert(backward_search(&B, 20, -1) == 0);


    // i=16: excess=2, target=20 (impossible as excess(i) <= i) so expected result=0 (sentinel)
    assert(backward_search(&B, 16, 18) == 0);

    //---------------------------
    //    Enclose Operation    |
    //---------------------------

    // 1110100111110101000100111011001010000100 Original bitVector
    //
    // ((()())((((()()()))())((()(())()())))()) Parenthesis Analogy
    //
    // 0123456789012345678901234567890123456789 indexing
    //           1         2         3          dozen 
    //
    // encloses(23) == 8

    bitVector* BV = new bitVector();
    BV->append1();
    BV->append1();
    BV->append0();
    BV->append1();
    BV->append0();
    BV->append0();
    BV->append1();
    BV->append1();
    BV->append1();
    BV->append0();
    BV->append1();
    BV->append0();
    BV->append0();
    BV->append0();

    assert(enclose(BV,8) == 7);
    assert(enclose(BV,11) == 8);
    assert(enclose(BV,13) == 1);
    assert(enclose(BV,9) == 8);

    string navarro = "1110100111110101000100111011001010000100";
    bitVector* gonzalo = new bitVector(navarro);

    assert(enclose(gonzalo,23) == 8);
    
    return 0;
}
