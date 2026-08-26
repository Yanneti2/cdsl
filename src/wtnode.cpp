#include "bitvector.h"
#include "wtnode.h"
#include <algorithm>
#include <cstdlib>
#include <map>
#include <algorithm>

using namespace std;

// lembrar de fazer a implicita dps
// mudar o vector para não utilizar ponteiros

std::map<char, bool> Hashing(string S) {
    std::map<char, bool> chars = {};
    for(uint32_t i = 0; i < S.size(); i++)
        if(chars.find(S[i]) == chars.end()) 
            chars.insert({S[i], true});
    return chars;
}

WaveletTreeNode::WaveletTreeNode(string S, WaveletTreeNode* dad) {
    // Parte para conseguir o alfabeto da str
    map<char, bool> alphabet = Hashing(S);
    std::string alpha = "";
    for (auto i = alphabet.begin(); i != alphabet.end(); i++) {
        alpha += i->first;                         
    }
    // se alfabeto for de tamanho 1 a recursão termina
    if (alpha.size() == 1){
        this->l = NULL;
        this->r = NULL;
        this->d = dad;
        return;
    }

    this->d = dad;


    uint32_t len = alpha.size() - 1;
    // len = alpha.size() - 1; precisa guardar o len?
    sort(alpha.begin(), alpha.end());
    bitVector *vector = new bitVector((unsigned long) ((S.size() + NBITS - 1)/NBITS), 2);
    uint32_t mid = len / 2;
    string LSS, RSS;

    for (uint32_t i = 0; i < S.size(); i++) {
        if (alpha[mid] >= S[i]) { // aq da p comparar só as posições n?
            vector->append0();
            LSS += S[i];
        }
        else {
            vector->append1();
            RSS += S[i];
        }
    }

    this->freq = vector;

    //analisar quando usar jacobson select e naive select // perguntar p B q isso

    // vector->JacobsonRank_build();
    // vector->build_select0();
    // vector->build_select1();

    if(LSS.size()) {
        this->l = new WaveletTreeNode(LSS, this);
    }
    if(RSS.size()) {
        this->r = new WaveletTreeNode(RSS, this);
    }
}

// void WaveletTreeNode::teste(){
//     if(l){
//         l->teste();
//     }
//     if(r){
//         r->teste();
//     }
// }

// WaveletTreeNode::~WaveletTreeNode() {
//     if (this->l != NULL) {
//         this->l->~WaveletTreeNode();
//         delete this->l;
//     }
//     if(this->r != NULL) {
//         this->r->~WaveletTreeNode();
//         delete this->r;
//     }
//     delete this->d;
//     this->freq->~bitVector();
// }

// // da p fazer com .dot do graphviz // perguntar q isso
// void WaveletTreeNode::print() {
// 	queue<WaveletTreeNode*> q;
// 	q.push(this);
// 	while(!q.empty()){
// 		WaveletTreeNode* cur = q.front();
// 		q.pop();
// 		if(cur->freq)cur->freq->print();
// 		cout<<cur->alpha<<endl<<endl;
// 		if(cur->l)q.push(cur->l);
// 		if(cur->r)q.push(cur->r);
// 	}
// }

// char WaveletTreeNode::access(unsigned long long i){ // n entendi isso tb
//     if(alpha.size() == 1){
//         return alpha[0];
//     }
//     if((*freq)[i] == 0){
//         auto j = freq->rank0(i);
//         return l->access(j);
//     }
//     else{
//         auto j = freq->rank1(i);
//         return r->access(j);
//     }
// }

// // seg fault, alpha should be only on root

// unsigned long long WaveletTreeNode::select_c(char c,  unsigned long long j){
//     unsigned long long a = 0;
// 	unsigned long long b = len;
//     if(a==b) return j;
//     unsigned long long aux = floor((a+b)/2);
//     if(c <= alpha[aux]){
//         j = l->select_c(c,j);
//         return freq->naive_select0(j);
//     }
//     else{
//         j = r->select_c(c,j);
//         return freq->naive_select1(j);
//     }
// }
// //abcdefimnoprtx
// unsigned long long WaveletTreeNode::rank_c(char c, unsigned long long i){
// 	WaveletTreeNode* root = this;
// 	unsigned long long a = 0;
// 	unsigned long long b = len-1;
// 	while(a!=b){
// 		if(c <= alpha[floor((a+b)/2)]){
// 			i = freq->naive_rank0(i);
// 			root = root->l;
// 			b = floor((a+b)/2);
// 		}else{
// 			i = freq->naive_rank1(i);
// 			root = root->r;
// 			a = floor((a+b)/2) + 1;
// 		}
// 	}	
// 	return i;
// }
