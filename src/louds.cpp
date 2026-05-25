#include "general_tree.h"
#include "bitvector.h"
#include "louds.hpp"


/* Level-Order Unary Degree Sequence (LOUDS) is a compact tree representation that traverses the
 * tree in BFS-order and, for each new node v with c children, appends 1^c 0's to a given 
 * bitVector B. These c + 1 bits are called description of a node v, and B[1,2n+1].
 */

//starts with '10' plus the previous description so that it prevents some border cases
//this functions populates a bitVector for a given explicit form tree previously initialized
// void build_louds(bitVector* B, Gtree::gNode* node){
// 	B->append1(); B->append0();
// 	if (!node) return;
// 	queue<Gtree::gNode*>q;
// 	q.push(node);
// 	while(q.size()){
// 		ULL nodes = q.size();
// 		for(int i=0;i<nodes;i++){
// 			Gtree::gNode* aux = q.front();	
// 			for(int j=0;j<aux->Children.size();j++){
// 				B->append1();		
// 				q.push(aux->Children[j]);
// 			}
// 			q.pop();
// 			B->append0();
// 		}
// 	}	
// }

/**
 *
 */
LOUDS::LOUDS(const char *s) {

}

/**
 *
 */
LOUDS::LOUDS(Gtree *T) {

}

/**
 *
 */
LOUDS::~LOUDS() {

}

/**
 *
 */
size_t LOUDS::root() {

}

/**
 *
 */
size_t LOUDS::fchild(size_t v) {

}

/**
 *
 */
size_t LOUDS::lchild(size_t v) {

}

/**
 *
 */
size_t LOUDS::child(size_t v, unsigned long long t) {

}

/**
 *
 */
unsigned long long LOUDS::children(size_t v) {

}

/**
 *
 */
unsigned long long LOUDS::childrank(size_t v) {

}

/**
 *
 */
size_t LOUDS::parent(size_t v) {

}

/**
 *
 */
size_t LOUDS::nsibling(size_t v) {

}

/**
 *
 */
size_t LOUDS::psibling(size_t v) {

}

/**
 *
 */
bool LOUDS::isleaf(size_t v) {

}

/**
 *
 */
unsigned long long LOUDS::nodemap(size_t v) {

}

/**
 *
 */
size_t LOUDS::nodeselect(unsigned long long i) {

}
