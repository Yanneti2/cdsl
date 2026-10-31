#include "uvector.hpp"
#include "iostream"

using namespace std;

UVector::UVector() : word_size(16) {}
UVector::UVector(unsigned word_size) : word_size(word_size) {}
UVector::UVector(unsigned word_size, size_t size) : word_size(word_size), B(size * word_size, 0) {}

long long UVector::operator[](size_t i) const {
    return B.accessWord(i, word_size);
}

void UVector::push_back(long long x) {
    B.append(x, word_size);
}

void UVector::set(size_t i, long long x) {
    B.set(x, word_size, i * word_size);
}

size_t UVector::size() const {
    return B.size() / word_size;
}