#include "bitvector.h"
#include "iwt.hpp"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <map>
#include <algorithm>

IWaveletTree::IWaveletTree(std::string S, unsigned long long i) {
    NodeN = i;
    lSonI = 2*i + 1;
    rSonI = 2*i + 2;
}