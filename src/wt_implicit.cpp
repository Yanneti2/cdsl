#include "wt_implicit.hpp"

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>

using namespace std;

#define ULL unsigned long long

static constexpr size_t n = static_cast<size_t>(-1);

string get_alpha(string og)
{
    set<char> unique;
    string alphabet = "";

    for (char c : og) unique.insert(c);
    for (char c : unique) alphabet += c;

    sort(alphabet.begin(), alphabet.end());

    return alphabet;
}

WaveletTreeImplicit::WaveletTreeImplicit(string S) : V()
{
    if (S.size() <= 0) return;

    queue<string> aux;
    aux.push(S);

    alpha = get_alpha(S);

    while(!aux.empty())
    {
        string cur = aux.front();
        aux.pop();

        string alphabet = get_alpha(cur);
        size_t alphas = alphabet.size(); 

        if (alphabet.size() <= 1) continue;

        string LSS, RSS;
        size_t mid = alphas/2;
        char split = alphabet[mid];
        BitVectorJ* vector = new BitVectorJ();

        for (ULL i = 0; i < cur.size(); i++)
        {
            if (alphabet[(alphas-1)/2] >= cur[i]) {
                vector->append0();
                LSS += cur[i];
            }
            else {
                vector->append1();
                RSS += cur[i];
            }
        }
        vector->init(); // jacob
        vector->build_select0();
        vector->build_select1();
        
        V.push_back(vector);

        if(LSS.size()) aux.push(LSS);
        if(RSS.size()) aux.push(RSS);
    }
}

uint64_t WaveletTreeImplicit::access(size_t i) const
{
    size_t end = alpha.size() - 1;
    size_t cindex = i;
    size_t beg = 0;
    size_t it = 0;

    while (beg < end)
    {
        BitVectorJ* cbv = V[it];
        size_t mid = (beg + end)/2;

        if ((*cbv)[cindex] == 0)
        {
            cindex = cbv->rank0(cindex);
            it = 2*it + 1;
            end = mid;
        } 
        else{
            cindex = cbv->rank1(cindex);
            it = 2*it + 2;
            beg = mid + 1;
        }
    }
    return alpha[beg];
}


size_t WaveletTreeImplicit::rank(uint64_t c, size_t i) const
{
    size_t end = alpha.size();
    size_t cindex = i;
    size_t beg = 0;
    size_t it = 0;

    BitVectorJ* cbv = V[it];
    
    while(beg + 1 != end)
    {
        if (it >= V.size()) break;

        cbv = V[it];
        size_t mid = (beg + end)/2;

        // cout << endl << beg << " " << end << " " << mid << " " << cindex << " \n";

        if(c < alpha[mid])
        {
            cindex = cbv->rank0(cindex);
            it = 2*it + 1;
            end = mid;
        }
        else {
            cindex = cbv->rank1(cindex);
            it = 2*it + 2;
            beg = mid;
        }
    }
    return cindex;
}

size_t WaveletTreeImplicit::_select(uint64_t c, size_t i, size_t it, size_t a, size_t b) const
{
    if (a == b) return alpha[a] == c ? i : n;

    size_t mid = (a+b) / 2;

    if(c <= alpha[mid])
    {
        i = _select(c, i, 2*it + 1, a, mid);
        if (i == n) return n;
        return V[it]->select0(i);
    }
    else {
        i = _select(c, i, 2*it + 2, mid + 1, b);
        if (i == n) return n;
        return V[it]->select1(i);
    }
}

size_t WaveletTreeImplicit::select(uint64_t c, size_t i) const
{
    if (V.empty()) return n;
    if (alpha.empty()) return n;

    if(alpha.size() == 1)
    {
        if (alpha[0] != c) return n;
        return i;
    }

    return _select(c, i, 0, 0, alpha.size() - 1);
}

void WaveletTreeImplicit::print()
{
    for (auto bv : V){
		bv->print(false);
		cout << "\n";
	}
}

WaveletTreeImplicit::~WaveletTreeImplicit()
{
    if (V.empty()) return;

    for(auto bv : V) delete bv;
}
