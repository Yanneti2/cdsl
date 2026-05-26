#include "endian.h"
#include <cstdio>
#include <iostream>
#include <bitset>
#include <stdint.h>
#include <string>
#include "bitvector.h"
using namespace std;

// printa um decimal em binario
void double_to_binary(double number, int double_size){
    uint8_t *array = (uint8_t*)(&number);
    for (int i = 0; i < double_size; ++i) {
    uint8_t byte = array[i];
    for (int j = 0; j < 8; ++j) {
        std::cout << (byte & 0x1);
        byte >>= 1;
    }
    }
}

int main (void){
    // bitset<n> name; 
    // n is the number of bits to allocate and name is the variable name associated

    // Source - https://stackoverflow.com/a/7349767
    // Posted by Jerry Coffin, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-03-30, License - CC BY-SA 4.0
    char a = 'a';
    std::bitset<8> x(a);
    std::cout << "char 'a' in binary: " << x << std::endl;
    
    short c = 15;
    std::bitset<16> y(c);
    std::cout << "short '15' in binary " << y << std::endl << std::endl;

    /////////////////////////////////////////////////////////////////////////////////////////////

    int system = 0;
    if (is_bigendian()){
        std::cout << "Your system is big-endian" << std::endl;
        system++;
    } else {
        std::cout << "Your system is little-endian" << std::endl;
    }
    std::cout << std::endl;
    // std::cout << "0 para small-endian, 1 para big-endian: " << is_bigendian() << '\n' << '\n';

    // Testing the reverse32 function for itegers
    int32_t num1 = 63;
    std::bitset<32> b(num1);
    std::cout << "Decimal: " << num1 << '\n';
    std::cout << "Binary: " << b << '\n' << '\n';
    int32_t num1_reversed = reverse32(num1);
    std::bitset<32> d(num1_reversed);
    std::cout << "Reverse Decimal: " << num1_reversed << '\n';
    std::cout << "Reverse num in Binary: " << d << '\n' << '\n';

    // Testing the reverse64 function for integers
    int64_t num2 = 54;
    std::bitset<64> e(num2);
    std::cout << num2 << " represented bit by bit: " << e << '\n';
    int64_t num2_reversed = reverse64(num2);
    std::bitset<64> f(num2_reversed);
    std::cout << num2_reversed << " as a bitvector: " << f << '\n' << '\n';

    // Testing the reversedouble for doubles
    double num3 = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825;
    std::cout << num3 << " represented bit by bit: " << std::endl;
    double_to_binary(num3, 64);
    std::cout << std::endl << std::endl;
    double num3_reversed = reversedouble(num3);
    std::cout << num3_reversed << " as a bitvector: " << std::endl; 
    double_to_binary(num3_reversed, 64);
    std::cout << std::endl << std::endl;  
    
    //Testing the reversefloat for float-like numbers
    float num4 = 5432.8234325567753543;
    if (system) { // se big
        std::cout << "Big-Endian: " << std::endl;
        system--;
    } else {
        std::cout << "Little-Endian: " << std::endl;
        system++;
    }
    std::cout << "Num as Decimal: " << num4 << '\n';
    std::cout << "Same num as binary: " << std::endl;
    double_to_binary(num4, 32);
    std::cout << std::endl << std::endl;
    float num4_reversed = reversefloat(num4);
    if (system) { std::cout << "Big-Endian: " << std::endl;}
    else { std::cout << "Little-Endian " << std::endl; }
    std::cout << "Reverse num as Decimal: " << num4_reversed << std::endl;
    std::cout << "Same num in Binary: " << std::endl;
    double_to_binary(num4_reversed, 32);

    // Test on the reversechar
    char abc = 'b';
    std::bitset<8> ch(abc);
    std::cout << std::endl << std::endl << "'b' in binary: " << ch << '\n';
    char abc_reverse = reversechar(abc);
    std::bitset<8> k(abc_reverse);
    std::cout << "Reverse 'b' in binary: " << k << '\n' << '\n';

    return 1;
}
