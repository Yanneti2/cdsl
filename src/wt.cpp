#include "bitvector.h"
#include "wt.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <map>

std::map<char, bool> HashingWT(string S) {
    std::map<char, bool> chars = {};
    for(uint32_t i = 0; i < S.size(); i++)
        if(chars.find(S[i]) == chars.end()) 
            chars.insert({S[i], true});
    return chars;
}

bitVector* get_left_right_ss(string S, string* LSS, string* RSS){
    map<char, bool> alphabet = HashingWT(S);
    std::string alpha = "";
    for (auto i = alphabet.begin(); i != alphabet.end(); i++) {
        alpha += i->first;                         
    }
    uint32_t len = alpha.size() - 1;
    sort(alpha.begin(), alpha.end());
    bitVector *vector = new bitVector((unsigned long) ((S.size() + NBITS - 1)/NBITS), 2);
    uint32_t mid = len / 2;

    for (uint32_t i = 0; i < S.size(); i++) {
        if (alpha[mid] >= S[i]) {
            vector->append0();
            LSS->push_back(S[i]);
        }
        else {
            vector->append1();
            RSS->push_back(S[i]);
        }
    }
    return vector;
}

wtNode* build(string S, wtNode* dad){
    wtNode* cur = new wtNode{.d=dad, .l=nullptr, .r=nullptr, .freq=nullptr};

    map<char, bool> alphabet = HashingWT(S);
    std::string alpha = "";
    for (auto i = alphabet.begin(); i != alphabet.end(); i++) {
        alpha += i->first;                         
    }
    
    if (alpha.length() == 1) {
        return cur;
    }

    string LSS, RSS;
    bitVector* B = get_left_right_ss(S,&LSS,&RSS);

    cur->freq = B;

    if(LSS.size()) {
        cur->l = build(LSS, cur);
    }
    if(RSS.size()) {
        cur->r = build(RSS, cur);
    }
    return cur;
}

WaveletTree::WaveletTree(string S, wtNode* dad) {
    map<char, bool> alphabet = HashingWT(S);
    std::string alpha = "";
    for (auto i = alphabet.begin(); i != alphabet.end(); i++) {
        alpha += i->first;                         
    }

    this->alpha = alpha;
    this->root = dad;

    if (alpha.size() == 1){
        dad->l = nullptr;
        dad->r = nullptr;
        dad->d = nullptr;
        return;
    }

    string LSS,RSS;
    bitVector* B = get_left_right_ss(S, &LSS, &RSS);

    dad->freq = B;
    dad->r = build(RSS, dad);
    dad->l = build(LSS, dad);

    return;
}

// WaveletTree::WaveletTree(string S, WaveletTree* r) {
//     this->root = r;
//     map<char, bool> alphabet = HashingWT(S);
//     for (auto i = alphabet.begin(); i != alphabet.end(); i++) {
//         this->alpha += i->first;                         
//     }
//     sort(alpha.begin(), alpha.end());
// }

char WaveletTree::access(unsigned long long i) {
    wtNode* CurN = this->root;
    unsigned long long end = this->alpha.size() - 1;
    unsigned long long beg = 0;
    unsigned long long CurI = i;

    while (end != beg) {
        if((*CurN->freq)[CurI] == 0){
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
    wtNode* CurN = this->root;
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

unsigned long long WaveletTree::selectc(char c,  unsigned long long i, wtNode* node){
    unsigned long long beg = 0;
    unsigned long long end = node->freq->size();
    unsigned long long CharI;

    if(beg == end)
        return i;
        
    unsigned long long mid = (beg + end)/2;
    if(c <= alpha[mid]) {
        i = this->selectc(c, i, node->l);
        return node->freq->naive_select0(i);
    }
    else {
        i = this->selectc(c, i, node->r);
        return node->freq->naive_select1(i);
    }
}

void WaveletTree::print() {
	queue<wtNode*> q;
	q.push(this->root);
    cout<< this->alpha <<endl <<endl;
	while(!q.empty()){
		wtNode* cur = q.front();
		q.pop();
		if(cur->freq)cur->freq->print();
		if(cur->l)q.push(cur->l);
		if(cur->r)q.push(cur->r);
	}
}

// lembrar de fazer o destrutor de uma wt com queue