#include "../../include/binary_tree.h" 
#include <iostream>
#include <queue>
using namespace std;

// builds a new Tree initializating the root
BinaryTree::BinaryTree(){
	this->root = new Node('$',0.0);
	// null
}	

// frees the memory of the existing now old tree
BinaryTree::~BinaryTree(){
	if (!this->root) return;
	queue<Node*> q;
	q.push(root);
	while(!q.empty()){
		int nodes = q.size();
		for (int i = 0; i < nodes; i ++){
			Node* aux = q.front();
			q.pop();
			if (aux->left) q.push(aux->left);
			if (aux->right) q.push(aux->right);
			delete aux;
		}
	}
}
// Returns 0 if not empty and 1 if empty
bool BinaryTree::isEmpty(Node* root){ if (root) return 0; else return 1; } 

// Return the root of the given existen Tree
BinaryTree::Node* BinaryTree::getRoot(){ return this->root; }

// Create and returns a new node
BinaryTree::Node* BinaryTree::create_node(char data, double freq){
	return new Node(data,freq);	
}

// Prints the Tree in BFS order
void BinaryTree::print_tree(Node* root){
	if (!root) return;
	queue<Node*> q;
	q.push(root);
	while(q.size()){
		int nodes = q.size();
		for (int i = 0; i < nodes; i++){
			Node* aux = q.front(); 
			q.pop();
			std::cout << "[" << aux->data << "] " << std::endl;
			if (aux->left) q.push(aux->left);
			if (aux->right) q.push(aux->right);
		}
		std::cout << "\n";
	}
}

/* Transforms the existen tree into a queue of nodes in BFS
vector* v explicit_to_implicit(){
	
}
*/
