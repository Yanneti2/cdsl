#include "../include/bitvector.h"
// #include "utils.h"
#include <iostream>
#include <cstdlib> //rand() & 1
#include <string>

using namespace std;

int main(void){
    unsigned long long size = 10000;
    // printar vetor inicilizado
    auto B1 = bitVector();
    auto B2 = bitVector();
    std::cout << std::endl << std::endl;
    // vetor na forma 10000100001....
    for (unsigned long long i = 0; i < size; i++){
        if (i%4 == 0){
            B1.append1();
        } else {
            B1.append0();
        }
    }
    B1.print();
    for (unsigned long long i = 0; i<size; i++){
        if (i%3 ==0) {
            B2.append1();
        } else if (i%8 == 0) {
            B2.append0();
        }
    }
    B2.print();
    B1.extend(&B2);
    B1.print();

    auto B3 = bitVector();
    for (TYPE i = 0; i<size; i++){
        if (rand() & 1){
            B3.append1();
        } else {
            B3.append0();
        }
    }
    std::cout << "B3" << std::endl;
    B3.print();
    // Teste de operacoes Put & Slice:
    auto B4 = bitVector();
    for (TYPE i = 0; i<size; i++){
        B4.append1();
    }
    std::cout << "B4" << std::endl;
    B4.print();
    bitVector* B5 = B3.slice(60,64);
    std::cout << "B5, slice de B3" << std::endl;
    B5->print();
    bitVector* B6 = B4.slice(128,64);
    std::cout << "B6, slice de B4" << std::endl;
    B6->print();
    // TA CORROMPENDO COM O GROW == 1.5!!!!!
    // OLHAR FUNCAO GROW, ACHO Q EH O PISO!!

    // bitVector* B7 = new bitVector();
    // for (TYPE i = 0; i<size; i++){
    //     if (rand() & 1){
    //         B7->append1();
    //     } else {
    //         B7->append0();
    //     }
    // }

    // B7->print();
    // // for (TYPE i = 0; i < size; i++) {
    // //     std::cout << "Acess[" << i << "] = " << B7->access(i) << std::endl;
    // // }
    // bitVector* B8 = B7->slice(7, 64);
    // std::cout << "B8, slice de B7" << std::endl;
    // B8->print();
    // return 0;

    auto BV = bitVector();

    std::cout << "BV:" << std::endl;
    for (int j = 0; j < 8; j++) {
        if (rand() & 1) BV.append1();
        else BV.append0();
    }
    BV.print();
    bitVector* BV2 = new bitVector();
    for (int j = 0; j < 8; j++) {
        BV2->append1();
    }
    std::cout << "BV2:" << std::endl;
    BV2->print();
    BV.put(BV2, 4);
    std::cout << "BV2 putted into [4] of BV:" << std::endl;
    BV.print();

    string s = "11010011101000";
    bitVector* string = new bitVector(s);
    string->print();
}
