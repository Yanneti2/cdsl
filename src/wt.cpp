#include "bitvector.h"
#include "wt.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <map>
#include <algorithm>

std::map<char, bool> Hashingwt(string S) {
    std::map<char, bool> chars = {};
    for(uint32_t i = 0; i < S.size(); i++)
        if(chars.find(S[i]) == chars.end()) 
            chars.insert({S[i], true});
    return chars;
}

WaveletTree::WaveletTree(std::string S, WaveletTreeNode* r) {
    this->root = r;
    map<char, bool> alphabet = Hashingwt(S);
    for (auto i = alphabet.begin(); i != alphabet.end(); i++) {
        this->alpha += i->first;                         
    }
    sort(alpha.begin(), alpha.end());
}

char WaveletTree::Acess(unsigned long long i) {
    WaveletTreeNode* CurN = this->root;
    unsigned long long end = CurN->freq->size();
    unsigned long long beg = 0;
    unsigned long long CurI = i;

    while (end != beg) {
        if(CurN->freq[CurI] == 0){
            CurI = CurN->freq->rank0(CurI);
            CurN = CurN->l;
            end = (beg + end)/2;
        }
        else {
            CurI = CurN->freq->rank1(CurI);
            CurN = CurN->r;
            beg = (beg + end)/2 + 1;
        }
    }
    return this->alpha[beg];
}

unsigned long long WaveletTree::rankc(char c, unsigned long long i) {
    WaveletTreeNode* CurN = this->root;
    unsigned long long CurI = i;
    unsigned long long beg = 0;
    unsigned long long end = this->root->freq->size();

    while(beg != end) {
        unsigned long long mid = (beg + end)/2;
        if(c <= alpha[mid]) {
            CurI = CurN->freq->rank0(CurI);
            CurN = CurN->l;
            end = mid;
        }
        else {
            CurI = CurN->freq->rank1(CurI);
            CurN = CurN->r;
            beg = mid + 1;
        }
    }
    return CurI;
}

unsigned long long WaveletTree::selectc(char c,  unsigned long long i, WaveletTreeNode* Node){
    unsigned long long beg = 0;
    unsigned long long end = this->root->freq->size();
    unsigned long long CharI;

    if(beg == end)
        return i;
        
    unsigned long long mid = (beg + end)/2;
    if(c <= alpha[mid]) {
        i = this->selectc(c, i, this->root->l);
        return this->root->freq->naive_select0(i);
    }
    else {
        i = this->selectc(c, i, this->root->r);
        return this->root->freq->naive_select1(i);
    }
}

void WaveletTree::print() {
	queue<WaveletTreeNode*> q;
	q.push(this->root);
    cout<< this->alpha <<endl <<endl;
	while(!q.empty()){
		WaveletTreeNode* cur = q.front();
		q.pop();
		if(cur->freq)cur->freq->print();
		if(cur->l)q.push(cur->l);
		if(cur->r)q.push(cur->r);
	}
}

