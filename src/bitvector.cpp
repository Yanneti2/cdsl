/*
  Original by G.P. Telles, 2013-2016.
  Edited in March 2026.
*/

/*
    TODO:
    Uma funcao para appendar um long e/ou uma string a um bitvector.
*/

#include <stack>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <cassert>
#include "../include/bitvector.h"
#include "../include/jacobsonrank.h"

#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;

#ifndef bitMask
#ifdef IS32BIT
#define bitMask(i) bitMask32[(i)]
#else
#define bitMask(i) bitMask64[(i)]
#endif
#endif

uint32_t bitMask32[] = {
    0xFFFFFFFF,0x7FFFFFFF,0x3FFFFFFF,0x1FFFFFFF,
    0x0FFFFFFF,0x07FFFFFF,0x03FFFFFF,0x01FFFFFF,
    0x00FFFFFF,0x007FFFFF,0x003FFFFF,0x001FFFFF,
    0x000FFFFF,0x0007FFFF,0x0003FFFF,0x0001FFFF,
    0x0000FFFF,0x00007FFF,0x00003FFF,0x00001FFF,
    0x00000FFF,0x000007FF,0x000003FF,0x000001FF,
    0x000000FF,0x0000007F,0x0000003F,0x0000001F,
    0x0000000F,0x00000007,0x00000003,0x00000001,
    0x00000000
};

uint64_t bitMask64[] = {
    0xFFFFFFFFFFFFFFFF,0x7FFFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFFF,0x1FFFFFFFFFFFFFFF,
    0x0FFFFFFFFFFFFFFF,0x07FFFFFFFFFFFFFF,0x03FFFFFFFFFFFFFF,0x01FFFFFFFFFFFFFF,
    0x00FFFFFFFFFFFFFF,0x007FFFFFFFFFFFFF,0x003FFFFFFFFFFFFF,0x001FFFFFFFFFFFFF,
    0x000FFFFFFFFFFFFF,0x0007FFFFFFFFFFFF,0x0003FFFFFFFFFFFF,0x0001FFFFFFFFFFFF,
    0x0000FFFFFFFFFFFF,0x00007FFFFFFFFFFF,0x00003FFFFFFFFFFF,0x00001FFFFFFFFFFF,
    0x00000FFFFFFFFFFF,0x000007FFFFFFFFFF,0x000003FFFFFFFFFF,0x000001FFFFFFFFFF,
    0x000000FFFFFFFFFF,0x0000007FFFFFFFFF,0x0000003FFFFFFFFF,0x0000001FFFFFFFFF,
    0x0000000FFFFFFFFF,0x00000007FFFFFFFF,0x00000003FFFFFFFF,0x00000001FFFFFFFF,
    0x00000000FFFFFFFF,0x000000007FFFFFFF,0x000000003FFFFFFF,0x000000001FFFFFFF,
    0x000000000FFFFFFF,0x0000000007FFFFFF,0x0000000003FFFFFF,0x0000000001FFFFFF,
    0x0000000000FFFFFF,0x00000000007FFFFF,0x00000000003FFFFF,0x00000000001FFFFF,
    0x00000000000FFFFF,0x000000000007FFFF,0x000000000003FFFF,0x000000000001FFFF,
    0x000000000000FFFF,0x0000000000007FFF,0x0000000000003FFF,0x0000000000001FFF,
    0x0000000000000FFF,0x00000000000007FF,0x00000000000003FF,0x00000000000001FF,
    0x00000000000000FF,0x000000000000007F,0x000000000000003F,0x000000000000001F,
    0x000000000000000F,0x0000000000000007,0x0000000000000003,0x0000000000000001,
    0x0000000000000000
};

/**
   A new, empty bitsequence.

   \param capacity The initial capacity (number of bitVector).
   \param growth_ratio
**/

// Initializes a bitvector instance
bitVector::bitVector() {

    assert(sizeof(TYPE) * 8 == NBITS);

    _cap = 1;
    _size = 0;

    A = (TYPE*) calloc(_cap, sizeof(TYPE));

    if (!A)
        throw new bad_alloc();
}

bitVector::bitVector(string s) {
	
    assert(sizeof(TYPE) * 8 == NBITS);

    _cap = (s.size() + NBITS -1) / NBITS;
    _cap = (_cap == 0) ? 1 : _cap;
    _size = s.size();

    A = (TYPE*) calloc(_cap, sizeof(TYPE));

    if(!A) throw new bad_alloc();

    for(unsigned long i = 0; i < s.size();i++){
	    if(s[i]=='0')this->set0(i);
	    else if(s[i]=='1')this->set1(i);
    }
}

bitVector::bitVector(size_t size) {
    
    assert(sizeof(TYPE) * 8 == NBITS);

    _cap = (size + NBITS - 1) / NBITS;
    _cap = (_cap == 0) ? 1 : _cap;
    _size = 0;

    A = (TYPE*) calloc(_cap, sizeof(TYPE));

    if (!A)
        throw new bad_alloc();
}

bitVector::bitVector(size_t size, int init) {

    assert(sizeof(TYPE) * 8 == NBITS);

    _cap = (size + NBITS - 1) / NBITS;
    _cap = (_cap == 0) ? 1 : _cap;
    _size = size;

    A = (TYPE*) calloc(_cap, sizeof(TYPE));

    if (!A)
        throw new bad_alloc();

    if (init == 0) return;

    for (unsigned long i = 0; i < _cap - 1; i++) {
        A[i] = -1;
    }
    A[_cap - 1] = ~bitMask(_size % NBITS);
}

bitVector::bitVector(size_t size, bool (*fn)(size_t)) {
    assert(sizeof(TYPE) * 8 == NBITS);

    _cap = (size + NBITS - 1) / NBITS;
    _cap = (_cap == 0) ? 1 : _cap;
    _size = size;

    A = (TYPE*) calloc(_cap, sizeof(TYPE));

    if (!A)
        throw new bad_alloc();
    
    for (unsigned long i = 0; i < _size; i++) {
        if (fn(i))
            this->set1(i);
    }
}
// Frees the memory dinamically alocated to store the bitvector
bitVector::~bitVector() {
    free(A);
}

bitVector &bitVector::operator=(const bitVector &B) {
    if (this == &B) {
        return *this;
    }

    TYPE *new_A = (TYPE *) realloc(A, B.cap() * sizeof(TYPE));
    if (!new_A)
        throw bad_alloc();

    for (size_t i = 0; i < B.cap(); i++) 
        new_A[i] = B.A[i];

    this->A = new_A;
    this->_cap = B.cap();
    this->_size = B.size();
    return *this;
}

// TODO: change this funtion to be a method of 'bitVector', does it really need to be a method?

// allocates new space for the bitvector, returns 1 if sucess
void bitVector::grow(size_t ncap) {
    if (ncap <= _cap)
        return;

    TYPE* AA = (TYPE*) realloc(A, ncap * sizeof(TYPE));
    if (!AA)
        throw bad_alloc();

    for (size_t i = _cap; i < ncap; i++) 
        AA[i] = 0;

    this->A = AA;
    this->_cap = ncap;
}

/**
   \brief Set A[i] to 1.

   If i > |A| - 1 then the behavior is undefined.
**/
void bitVector::set1(size_t i) {
    A[i / NBITS] |= (bitMask(i % NBITS) ^ bitMask(i % NBITS + 1));
}

/**
   \brief Set A[i] to 0.

   If i > |A| - 1 then the behavior is undefined.
**/
void bitVector::set0(size_t i) {
    A[i / NBITS] &= ~(bitMask(i % NBITS) ^ bitMask(i % NBITS + 1));
}

/**
   \brief Return 0 if A[i] == 0 or 1 if A[i] == 1.

   If i > |A|-1 then the behavior is undefined.
**/
int bitVector::operator[](size_t i) const {
    return (A[i / NBITS] & (bitMask(i % NBITS) ^ bitMask(i % NBITS + 1))) ? 1 : 0;
}

bool bitVector::operator==(bitVector B) const {
    size_t thisSize = this->size();
    size_t BSize = B.size();
    if (thisSize != BSize) return false;

    for (size_t i = 0; i < thisSize / NBITS; i++) {
        if (this->accessWord(i) != B.accessWord(i)) return false;
    }
    return true;
}

TYPE bitVector::accessWord(size_t i) const {
    return A[i];
}

TYPE bitVector::accessWord(size_t i, unsigned wordSize) const {
    unsigned long long start = i * wordSize;
    unsigned long long end = start + wordSize - 1;
    unsigned long long start_index = start / NBITS;
    unsigned long long end_index = end / NBITS;
    start %= NBITS;
    end %= NBITS;
    if (start_index == end_index) {
        return (A[start_index] & ~bitMask(end + 1)) << start;
    }
    return (A[start_index] << start) | ((A[end_index] & ~bitMask(end + 1)) >> (NBITS - start));
}

/**
   Add 0 to the end of A and increase |A| by one.
**/
void bitVector::append0() {

    if (_size == NBITS * _cap)
    {
        unsigned long new_cap = ceil(_cap * 1.5);
        if (new_cap <= _cap)
            new_cap = _cap + 1;
        grow(new_cap);
    }

    _size++;
}

/**
   Add 1 to the end of A and increase |A| by one.
**/
void bitVector::append1() {

    if (_size == NBITS * _cap)
    {
        unsigned long new_cap = ceil(_cap * 1.5);
        if (new_cap <= _cap)
            new_cap = _cap + 1;
        grow(new_cap);
    }

    set1(_size++);
}

size_t bitVector::size() const { return _size; }
size_t bitVector::cap() const { return _cap; }

/**
    Coloca uma sequencia predefinida de bits ao final do bitvector.
**/
void bitVector::extend(bitVector* B) {
    if (_cap == 0) _cap = 1;

    while(!((_size + B->size() + NBITS - 1) / NBITS <= _cap))
    {
        unsigned long new_cap = (unsigned long) ceil(_cap * 1.5);
        if (new_cap <= _cap)
            new_cap = _cap + 1;
        grow(new_cap);
    }

    short bitsSobrando = _size % NBITS;
    short bitsFaltando = NBITS - bitsSobrando;
    short cur = _size / NBITS;

    for (unsigned long long i = 0; i < (B->size() + NBITS - 1) / NBITS; i++) {
         if (bitsSobrando == 0) {
            this->A[cur] = B->A[i];
            cur++;
        } else {
            this->A[cur] |= B->A[i] >> bitsSobrando;
            cur++;
            this->A[cur] |= B->A[i] << bitsFaltando;
        }
    }
    this->_size += B->size();
}

/**
    Copy a part of original bitvector into another bitvector and returns the new one.
**/
bitVector* bitVector::slice(unsigned long i, unsigned long k) const {
    bitVector* Bnew = new bitVector();
    if (i + k > _size)
        throw std::out_of_range("slice out of bounds");
    for (unsigned long j = 0; j < k; j++) {
        if ((*this)[i + j])
            Bnew->append1();
        else
            Bnew->append0();
    }
    return Bnew;
}

/**
    Inserts a bit sequence in the i'th bitvector position.
**/
void bitVector::put(bitVector* B, unsigned long i) {
    bitVector* endOriginal = this->slice(i, this->size() - i);
    this->_size = i;
    this->extend(B);
    this->extend(endOriginal);
    delete endOriginal;
}

/**
   \brief Print the bit array on the screen.
**/
void bitVector::print() const {
    printf("size: %ld, cap: %ld\n", _size, _cap);
    for (size_t i = 0; i < _size; i++) {
        printf("%d", (*this)[i]);
    }
    printf("\n\n");
}

size_t bitVector::popcount(){
    size_t pop_count = 0; 
    for(size_t i = 0; i < _size; i++){
        pop_count += std::__popcount(accessWord(i));
    }
    return pop_count;
}

size_t bitVector::naive_rank1(size_t i){
    size_t pop_count = 0;
    size_t j;
    for(j = 0; j < i / NBITS; j++){
        pop_count += std::__popcount(accessWord(j));
    }
    pop_count += std::__popcount(accessWord(j) & ~bitMask(i % NBITS));
    return pop_count;
}

size_t bitVector::naive_rank0(size_t i) {
    return i - naive_rank1(i); 
}

size_t bitVector::select1(size_t i) {
    return rank->select1(this, i);
}

size_t bitVector::select0(size_t i){
    return rank->select0(this, i);
}
    
void bitVector::JacobsonRank_build() {
    if (rank)
        delete rank;

    rank = new JacobsonRank(this);
}

size_t bitVector::rank0(size_t i) {
    return rank->rank0(this, i);
}

size_t bitVector::rank1(size_t i) {
    return rank->rank1(this, i);
}

void bitVector::print_rank() {
    rank->print();
}

void bitVector::build_select0() {
    rank->build_select0(this);
}

void bitVector::build_select1() {
    rank->build_select1(this);
}

size_t bitVector::naive_select1(size_t i){
    size_t pop_count = 0;
    size_t j;
    for (j = 0; j < _size; j++) {
        if (pop_count == i) return j;
        pop_count += (*this)[j];
    }
    return -1;
}

size_t bitVector::naive_select0(size_t i) {
    size_t counter = 0;
    size_t j;
    for (j = 0; j < _size; j++) {
        if ((j - counter) == i) return j;
        counter += (*this)[j];
    }
    return -1;
}

void bitVector::serialize(const char *path) {
    
    FILE *arch = fopen(path, "w");
    string s;
    for (size_t i = 0; i < _size; i++) {
        if ((*this)[i])
            s += "1";
        else
            s += "0";
    }
    if (arch) {
        fprintf(arch, "%s", s.data());
        fclose(arch);
    }
}

bitVector* bitVector::deserialize(const char *path){
    ifstream file;
    file.open(path);
    if (!file.is_open()) {
        return NULL;
    }
    bitVector *bv = new bitVector();
    string s;
    getline(file, s);
    for (char c : s) {
        if (c == '1')
            bv->append1();
        else
            bv->append0();
    }
    return bv;
}

#undef bitMask
