#include <iostream>
#include "tree.h"

int main (void){
	Tree* t = new Tree();
	Tree::Node* root = t->getRoot();
	root->left = t->create_node();
	root->right = t->create_node();
	t->print_tree(root);	
	delete t;
	return 0;
}
