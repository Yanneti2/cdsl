#include "general_tree.hpp"
#include "binary_tree.hpp"
#include "louds.hpp"
#include "general_tree.hpp"
// fiquei de gerar tree aleatoria, refazer last child e assert nas f 
// cria um vetor, colocar raiz no vetor e sortear alguma index do vetor pra colocar um filho no nó q tiver nessa posição e colocar esse filho no vetor
#include <iostream>
#include <cassert>
#include <string>

using namespace std;


Gtree *rand_tree(size_t n) {
    Gtree *GT = new Gtree();
    vector<Gtree::gNode *> nodes;
    nodes.push_back(GT->getRoot());

    for (size_t i = 0; i < n - 1; i++) {
        Gtree::gNode *node = nodes[rand() % nodes.size()];
        Gtree::gNode *new_node = GT->create_node();
        GT->add_node(node, new_node);
        nodes.push_back(new_node);
    }
    return GT;
}


int main (int argc, char *argv[])
{
	//==========================
	//		Constructors	   |
	//==========================
	
	Gtree* t = new Gtree();
	Gtree::gNode* n1 = t->getRoot();
	Gtree::gNode* n2 = t->create_node();
	Gtree::gNode* n3 = t->create_node();
	Gtree::gNode* n4 = t->create_node();
	n1->Children.push_back(n2);
	n1->Children.push_back(n3);
	n1->Children.push_back(n4);
	t->append_nnode(n2);
	t->append_nnode(n2);
	Gtree::gNode* n7 = t->create_node();
	Gtree::gNode* n8 = t->create_node();
	n3->Children.push_back(n7);
	n3->Children.push_back(n8);
	Gtree::gNode* n9 = t->create_node();
	Gtree::gNode* n11 = t->create_node();
	n7->Children.push_back(n9);
	t->append_nnode(n7);
	n8->Children.push_back(n11);
	Gtree::gNode* n12 = t->create_node();
	n9->Children.push_back(n12); t->append_nnode(n12);
	t->append_nnode(n12);
	t->append_nnode(n12);
	t->append_nnode(n11);
	Gtree::gNode* n14 = t->create_node();
	n11->Children.push_back(n14);
	t->append_nnode(n11);
	t->append_nnode(n11);
	t->append_nnode(n14);

	LOUDS l = LOUDS(t);
	l.print();
	delete t;
	//          12345678901234567890123456789012345678901
	string s = "10111011011000011010100111101110010000000";
	BitVector b = BitVector(s);
	LOUDS la = LOUDS(b);
	LOUDS l1 = LOUDS(s);
	l1.print();

	BinaryTree* t1 = new BinaryTree();
	BinaryTree::Node* root = t1->getRoot();	
	root->left = t1->create_node('a',1.0);
	root->right = t1->create_node('b',1.0);
	root->left->left = t1->create_node('c',1.0);
	root->right->right = t1->create_node('d',1.0);

	LOUDS l2 = LOUDS(t1);
	l2.print();
	delete t1;
	
	unsigned long long n = 1000;
	Gtree* tree = rand_tree(n);

	std::cout <<
	l1.fchild(2) <<
	l1.fchild(3) <<
	l1.fchild(7) <<
	l1.fchild(8) <<

	l1.lchild(2) <<
	l1.lchild(3) <<
	l1.lchild(7) <<
	l1.lchild(8) <<

	l1.child(1,3) <<
	l1.child(2,2) <<
	l1.child(2,1) <<
	l1.child(11,4) <<

	l1.children(1) <<
	l1.children(2) <<
	l1.children(8) <<
	l1.children(11) <<

	l1.childrank(2) << //1
	l1.childrank(3) <<
	l1.childrank(4) <<
	l1.childrank(5) << //2
	l1.childrank(6) <<
	l1.childrank(7) << //3
	l1.childrank(8) <<
	l1.childrank(9) << //7
	l1.childrank(10) <<
	l1.childrank(11) << //8
	l1.childrank(12) << //9
	l1.childrank(13) << //11
	l1.childrank(14) <<
	l1.childrank(15) <<
	l1.childrank(16) <<
	l1.childrank(17) << //12
	l1.childrank(18) <<
	l1.childrank(19) <<

	l1.nsibling(2) <<
	l1.nsibling(3) <<
	l1.nsibling(5) <<
	l1.nsibling(7) <<
	l1.nsibling(9) <<
	l1.nsibling(13) <<
	l1.nsibling(14) <<
	l1.nsibling(15) <<
	l1.nsibling(17) <<
	l1.nsibling(18) <<

	l1.psibling(3) <<
	l1.psibling(4) <<
	l1.psibling(6) <<
	l1.psibling(8) <<
	l1.psibling(10) <<
	l1.psibling(11) <<
	l1.psibling(12) <<
	l1.psibling(14) <<
	l1.psibling(15) <<
	l1.psibling(16) <<
	l1.psibling(18) <<

	l1.isleaf(2) <<
	l1.isleaf(3) <<
	l1.isleaf(4) <<
	l1.isleaf(11) <<
	l1.isleaf(17) <<

	l1.parent(2) << //1
	l1.parent(3) <<
	l1.parent(4) <<
	l1.parent(5) << //2
	l1.parent(6) <<
	l1.parent(7) << //3
	l1.parent(8) <<
	l1.parent(9) << //7
	l1.parent(10) <<
	l1.parent(11) << //8
	l1.parent(12) << //9
	l1.parent(13) << //11
	l1.parent(14) <<
	l1.parent(15) <<
	l1.parent(16) <<
	l1.parent(17) << //12
	l1.parent(18) <<
	l1.parent(19) <<

	l1.nodemap(10) <<
	l1.nodemap(20) <<
	l1.nodemap(30) <<
	l1.nodemap(41) <<
	l1.nodeselect(2) <<
	l1.nodeselect(8) <<
	l1.nodeselect(11) <<
	l1.nodeselect(19) <<
	std::endl;



	//==========================
	//		 IS LOUDS ? 	   |
	//==========================
	
	//assert(l1.is_louds(s));
	//assert(l1.is_louds());

	string s1 = "10";
	LOUDS lt1 = LOUDS(s1);
	assert(!lt1.is_louds(s1));
	assert(!lt1.is_louds());

	string s2 = "100";
	LOUDS lt2 = LOUDS(s2);
	assert(lt2.is_louds(s2));
	assert(lt2.is_louds());

	string s3 = "10100";
	LOUDS lt3 = LOUDS(s3);
	assert(lt3.is_louds(s3));
	assert(lt3.is_louds());

	string s4 = "1011000";
	LOUDS lt4 = LOUDS(s4);
	assert(lt4.is_louds(s4));
	assert(lt4.is_louds());

	string s5 = "1011000";	
	LOUDS lt5 = LOUDS(s5);
	assert(lt5.is_louds(s5));
	assert(lt5.is_louds());

	string s6 = "101010100";
	LOUDS lt6 = LOUDS(s6);
	assert(lt6.is_louds(s6));
	assert(lt6.is_louds());

	string s7 = "10100100";
	LOUDS lt7 = LOUDS(s7);
	assert(!lt7.is_louds(s7));
	assert(!lt7.is_louds());
}
