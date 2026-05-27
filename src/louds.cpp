#include "general_tree.h"
#include "binary_tree.h"
#include "bitvector.h"
#include "louds.hpp"
#include <iostream>
#include <string>
#include <queue>

using namespace std;

/* Level-Order Unary Degree Sequence (LOUDS) is a compact tree representation that traverses the
 * tree in BFS-order and, for each new node v with c children, appends 1^c 0's to a given 
 * bitVector B. These c + 1 bits are called description of a node v, and B[1,2n+1].
 */

/**
 * Assuming the given s string is a valid LOUD tree in binary
 */
LOUDS::LOUDS(string s){
	if(is_louds(s)){
		for(unsigned long i = 0; i < s.size(); i++){
			if(s[i]=='1')T.append1();
			else if(s[i]=='0')T.append0();
		}
	}
}

/**
 * Starts with '10' plus the previous description so that it prevents some border cases
 * This functions populates a bitVector for a given explicit form tree previously initialized
 */
LOUDS::LOUDS(Gtree* G){
 	T.append1(); T.append0();
	Gtree::gNode* node = G->getRoot();
 	if (!node) return;
 	queue<Gtree::gNode*>q;
 	q.push(node);
 	while(q.size()){
 		unsigned long long nodes = q.size();
 		for(int i=0;i<nodes;i++)
		{
 			Gtree::gNode* aux = q.front();	
 			for(int j=0;j<aux->Children.size();j++)
			{
 				T.append1();		
 				q.push(aux->Children[j]);
 			}
			q.pop();
 			T.append0();
 		}
 	}	
}

/*
 * Starts with '10' plus the previous description so that it prevents some border cases
 * This functions populates a bitVector for a given explicit form tree previously initialized
 */
LOUDS::LOUDS(Tree* BT){
 	T.append1(); T.append0();
	Tree::Node* node = BT->getRoot();
 	if (!node) return;
 	queue<Tree::Node*>q;
 	q.push(node);
 	while(q.size()){
 		unsigned long long nodes = q.size();
 		for(int i=0;i<nodes;i++)
		{
 			Tree::Node* aux = q.front();	
			if(aux->left){
				T.append1();
				q.push(aux->left);
			}
			if(aux->right){
				T.append1();
				q.push(aux->right);
			}
			q.pop();
 			T.append0();
 		}
 	}	
}

/*
 * Based on some basic previous commom knowledge, not 100% sure, filters roughly if a string follow the louds schema
 */
bool LOUDS::is_louds(string s){
	if(s.size() < 2 || (s.size() > 2 && s.size() % 2 == 0))return false;
	if(!(s[0] =='1' && s[1] =='0'))return false;
	bitVector dummy = bitVector(s);
	if(dummy.naive_rank0(dummy.size()) - dummy.naive_rank1(dummy.size()) != 1)return false;
	int counter = 0;
	for(unsigned long i = 2; i <= dummy.size(); i++){
		if(counter == -1)return false;
		counter = dummy.naive_rank1(i) - dummy.naive_rank0(i);
	}
	if(counter == -1)return true;
	return false;
}

/*
 * Based on some basic previous commom knowledge, not 100% sure, filters roughly if a string follow the louds schema
 */
bool LOUDS::is_louds(){
	if(T.size() < 2 || (T.size() > 2 && T.size() % 2 == 0))return false;
	if(!(T[0] == 1 && T[1] == 0))return false;	
	if(T.naive_rank0(T.size()) - T.naive_rank1(T.size()) != 1)return false;
	int counter = 0;
	for(unsigned long i = 2; i <= T.size(); i++){
		if(counter == -1)return false;
		counter = T.naive_rank1(i) - T.naive_rank0(i);
	}
	if(counter == -1)return true;
	return false;
}

/*
 * Prints the bitVector associated to the current LOUDS tree
 */
void LOUDS::print(){
	for(unsigned long long i = 0; i < T.size(); i++){
		cout << T[i];
	}	
	cout << endl;
}

/**
 *
 */
size_t LOUDS::succ0(size_t v) {
    return T.select0(T.rank0(v) + 1) - 1; 
}

/**
 *
 */
size_t LOUDS::succ1(size_t v) {
    return T.select1(T.rank1(v) + 1) - 1;
}

/**
 *
 */
size_t LOUDS::pred0(size_t v) {
    int a = T.naive_rank0(v);
    int b = T.naive_select0(a);
    cout << "\nPRED0 " << a << " " << b << "\n";
    return b;
}

/**
 *
 */
size_t LOUDS::pred1(size_t v) {
    return T.select1(T.rank1(v)) - 1;
}

/**
 *
 */
size_t LOUDS::root() {
    return 2;
}

/**
 *
 */
size_t LOUDS::fchild(size_t v) {
    if (!T[v]) return -1;
    return T.select0(T.rank1(v + 1));
}

/**
 *
 */
size_t LOUDS::lchild(size_t v) {
    return 17;

}

/**
 *
 */
size_t LOUDS::child(size_t v, unsigned long long t) {
    if (!T[v]) return -1;
    return T.select0(T.rank1(v + t));
}

/**
 *
 */
unsigned long long LOUDS::children(size_t v) {
    return succ0(v) - v;
}

/**
 *
 */
unsigned long long LOUDS::childrank(size_t v) {
    return v - pred0(v);
}

/**
 *
 */
size_t LOUDS::parent(size_t v) {
	if (v <= 2) return 0;
    unsigned long long NodeNum =  T.naive_rank0(v); // número do nó
    unsigned long long NodeBeg = T.naive_select1(NodeNum); // acha o 1 correspondente ao nó
	cout << "parent " << NodeNum << NodeBeg << "\n";
    return pred0(NodeBeg);
}

/**
 *
 */
size_t LOUDS::nsibling(size_t v) {
    return 17;
}

/**
 *
 */
size_t LOUDS::psibling(size_t v) {
    return 17;
}

/**
 *
 */
bool LOUDS::isleaf(size_t v) {
    if (v < 2) return false;
    return T[v] == 0;
}

/**
 *
 */
unsigned long long LOUDS::nodemap(size_t v) {
    return T.rank0(v) - 1;
}

unsigned long long LOUDS::size() {
    return T.size();
}

/**
 *
 */
size_t LOUDS::nodeselect(unsigned long long i) {
    return T.select0(i + 1);

}
