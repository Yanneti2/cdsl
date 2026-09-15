#include "bitvector.h"
#include "wt.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <map>

#define ULL unsigned long long

std::map<char, bool> HashingWT(string S) {
    std::map<char, bool> chars = {};
    for(uint32_t i = 0; i < S.size(); i++)
        if(chars.find(S[i]) == chars.end()) 
            chars.insert({S[i], true});
    return chars;
}

string get_alphabet(map<char,bool> chars){
    string alphabet;
    for (auto i = chars.begin(); i != chars.end(); i++){
        alphabet += i->first;                         
    }
    return alphabet;
}

wtNode* WaveletTree::buildWT(string S, wtNode* dad)
{
    wtNode* cur_node = new wtNode();
    cur_node->dad = dad;
    cur_node->rchild = cur_node->lchild = nullptr;

    map<char,bool> chars = HashingWT(S);
    string alphabet = get_alphabet(chars);

    if (alphabet.size() <= 1) {
        cur_node->symbol = alphabet[0];   
        return cur_node;
    }

    ULL mid = (alphabet.size() - 1) / 2;
    sort(alphabet.begin(), alphabet.end());

    string LSS, RSS;
    bitVector vector = bitVector((unsigned long) ((S.size() + NBITS - 1)/NBITS), 2);

    for (ULL i = 0; i < S.size(); i++)
    {
        if (alphabet[mid] >= S[i]) {
            vector.append0();
            LSS += S[i];
        }
        else {
            vector.append1();
            RSS += S[i];
        }
    }
    cur_node->freq = vector;
    // vector.JacobsonRank_build();

    if(LSS.size()) cur_node->lchild = buildWT(LSS, cur_node);
    if(RSS.size()) cur_node->rchild = buildWT(RSS, cur_node);

    return cur_node;
}

WaveletTree::WaveletTree(string S){
    if (S.size() == 0) {
        this->root = nullptr;
        return;
    }
    map<char,bool> chars = HashingWT(S);
    this->alphabet = get_alphabet(chars);
    this->root = buildWT(S,nullptr);
}

wtNode* WaveletTree::getRoot(){
    return this->root;
}

char WaveletTree::access(ULL i) {
    wtNode* CurN = this->root;
    ULL end = this->alphabet.size() - 1;
    ULL beg = 0;
    ULL CurI = i;

    while (end != beg) {
        if((CurN->freq)[CurI] == 0){
            CurI = CurN->freq.naive_rank0(CurI);
            CurN = CurN->lchild;
            end = (beg + end)/2;
        }
        else {
            CurI = CurN->freq.naive_rank1(CurI);
            CurN = CurN->rchild;
            beg = (beg + end)/2 + 1;
        }
    }
    return CurN->symbol;
}

ULL WaveletTree::rankc(char c, ULL i) {
    wtNode* CurN = this->root;
    ULL CurI = i;
    ULL beg = 0;
    ULL end = this->alphabet.size() - 1;

    while(beg != end) {
        ULL mid = (beg + end)/2;
        if(c <= alphabet[mid]) {
            CurI = CurN->freq.naive_rank0(CurI);
            CurN = CurN->lchild;
            end = mid;
        }
        else {
            CurI = CurN->freq.naive_rank1(CurI);
            CurN = CurN->rchild;
            beg = mid + 1;
        }
    }
    return CurI;
}

ULL WaveletTree::selectc(char c,  ULL i, wtNode* node){
    ULL beg = 0;
    ULL end = this->alphabet.size();
    ULL CharI;

    if(beg == end)
        return i;
        
    ULL mid = (beg + end)/2;
    if(c <= alphabet[mid]) {
        i = this->selectc(c, i, node->lchild);
        return node->freq.naive_select0(i);
    }
    else {
        i = this->selectc(c, i, node->rchild);
        return node->freq.naive_select1(i);
    }
}

void WaveletTree::print() {
	queue<wtNode*> q;
	q.push(this->root);
    cout<< this->alphabet << endl << endl;
	while(!q.empty()){
		wtNode* cur = q.front();
		q.pop();
		// if(cur->freq.size() > 0){
        //     for (ULL i = 0; i < cur->freq.size(); i++){
        //         cout << (cur->freq)[i];
        //     }cout << endl;
        // }
        if(cur->symbol)cout << cur->symbol << endl;
        else cur->freq.print();
		if(cur->lchild)q.push(cur->lchild);
		if(cur->rchild)q.push(cur->rchild);
	}
}

WaveletTree::~WaveletTree(){
    if (!this->root)return;

    queue<wtNode*> q;
    q.push(this->root);

    while(!q.empty()){
        wtNode* cur_node = q.front();
        q.pop();
        if(cur_node->lchild) q.push(cur_node->lchild);
        if(cur_node->rchild) q.push(cur_node->rchild);
        delete cur_node;
    }
}