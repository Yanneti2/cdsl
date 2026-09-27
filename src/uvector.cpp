#include "uvector.hpp"
#include "iostream"

using namespace std;

UVector::UVector(unsigned word_size) : size(word_size) {}
UVector::UVector(unsigned word_size, size_t size) : size(word_size), B(size * word_size, 0) {}

long long UVector::operator[](size_t i) {
    return B.accessWord(i, size);
}

void UVector::push_back(long long x) {
    unsigned long long b = 0x8000000000000000 >> (8 * sizeof(long long) - size);
    for (; b; b >>= 1) {
        if (b & x)
            B.append1();
        else 
            B.append0();
    }
}

void UVector::set(size_t i, long long x) {
    unsigned long long b = 0x8000000000000000 >> (8 * sizeof(x) - size);
    i *= size;
    for (; b; b >>= 1) {
        if (b & x)
            B.set1(i);
        else 
            B.set0(i);
        i++;
    }
}
