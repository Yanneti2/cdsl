#include "uvector.hpp"
#include <iostream>

using namespace  std;

int main(void) {
    UVector V(17);
    V.push_back(13);
    V.push_back(16);
    V.push_back(19);
    V.push_back(22);
    V.push_back(24);
    V.push_back(25);
    V.push_back(26);

    cout << V[0] << endl;
    cout << V[1] << endl;
    cout << V[2] << endl;
    cout << V[3] << endl;
    cout << V[4] << endl;
    cout << V[5] << endl;
    cout << V[6] << endl;

    for (int i = 0; i < 64; i++) {
        cout << ((V[0] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;
    for (int i = 0; i < 64; i++) {
        cout << ((V[1] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;
    for (int i = 0; i < 64; i++) {
        cout << ((V[2] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;
    for (int i = 0; i < 64; i++) {
        cout << ((V[3] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;
    for (int i = 0; i < 64; i++) {
        cout << ((V[4] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;
    for (int i = 0; i < 64; i++) {
        cout << ((V[5] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;
    for (int i = 0; i < 64; i++) {
        cout << ((V[6] & (0x8000000000000000 >> i)) && 1);
    }
    cout << endl;

    for (int i = 0; i < 17 * 7; i++) {
        cout << V.B[i];
    }
    cout << endl;

    V.set(1, 10);
    cout << V[1] << endl;
    for (int i = 0; i < 17 * 7; i++) {
        cout << V.B[i];
    }
    cout << endl;

}

