#ifndef PERMUTATIONS
#define PERMUTATIONS 


#include "uvector.hpp"
#include <vector>

// TODO: Implement compressed permutation using Huffman Encoding



/*! \class permutations
 *  \brief A class that represent index permutations.
 *
 *  A permutation P of [0,n-1] is a reordering of the values {0,1,...,n-1}.
 *  Its a bijection over a set of indices {0,1,...,n-1} to iteself. Therefore,
 *  Duplicated elements are not allowed nor missing elements. The values
 *  Must cover the entire domain.
 *
 */
class Permutation
{
public:
    Permutation (std::vector<int> permutation, size_t t); 
    int operator()(int i); // returns P(i), that is equivalent to apply the permutation once
    int power(int i, int n); // returns P^n(i), that is equivalent to apply the permutation n times.
    int inverse(int i); // Returns P^-1(i), that is the equivalent to P^{k-1}(i), where k is the size of cycle that i belongs to
    size_t size(); // Returns the size of permutation

private:
    // Internal representation of the permutation
    std::vector<size_t> permutation;

    // shortcuts[i] is 1 if position i has a shortcut.
    // The destination of each shortcut is stored in
    // shortcuts_endpoints in increasing order of shortcut position.
    BitVector shortcuts;
    BitVector cycles;
    std::vector<int> shortcuts_endpoints;

    std::vector<int> tau;  // cycles written one after another
    int tau_inverse(int i); // position j such that tau[j] == i
};


#endif
