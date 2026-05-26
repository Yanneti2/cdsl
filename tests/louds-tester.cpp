#include "general_tree.h"
#include "binary_tree.h"
#include "louds.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace std;

int main (int argc, char *argv[])
{
	//==========================
	//	Constructors	   |
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
	//l.print();
	delete t;

	string s = "10111011011000011010100111101110010000000";
	LOUDS l1 = LOUDS(s);
	//l1.print();

	Tree* t1 = new Tree();
	Tree::Node* root = t1->getRoot();	
	root->left = t1->create_node('a',1.0);
	root->right = t1->create_node('b',1.0);
	root->left->left = t1->create_node('c',1.0);
	root->right->right = t1->create_node('d',1.0);

	LOUDS l2 = LOUDS(t1);
	//l2.print();
	delete t1;

	//==========================
	//	 IS LOUDS ? 	   |
	//==========================
	
	assert(l1.is_louds(s));
	assert(l1.is_louds());

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
