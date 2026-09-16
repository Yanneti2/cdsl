#include "binary_tree.hpp" 
#include "general_tree.hpp"
#include <queue>
#include <vector>
using namespace std;

// builds a new Tree initializating the root
Gtree::Gtree(){
	this->root = new gNode();
	//this->root->marked = false;
}	

// frees the memory of the existing now old tree
Gtree::~Gtree(){
	if (!this->root) return;
	queue<gNode*> q;
	q.push(root);
	while(!q.empty()){
		int nodes = q.size();
		for (int i = 0; i < nodes; i ++){
			gNode* aux = q.front();
			q.pop();
			if (aux->Children.size() > 0){
				unsigned long int tmp = aux->Children.size();
				for(unsigned long int i = 0; i < tmp; i++){ q.push(aux->Children[i]); }	
			}
			delete aux;
		}
	}
}
// Returns 0 if not empty and 1 if empty
bool Gtree::isEmpty(gNode* root){ if (root) return 0; else return 1; } 

// returns true or false if node marked
//bool Gtree::getMarked(){ return this->root->marked; }

// Return the root of the given existen Tree
Gtree::gNode* Gtree::getRoot(){ return this->root; }

// Create and returns a new node
Gtree::gNode* Gtree::create_node(){
	gNode* nnode = new gNode();
	//nnode->marked = false;
	return nnode;
}

// Appends a new node into the children vector of a given root
void Gtree::append_nnode(gNode* root){
	Gtree::gNode* nnode = Gtree::create_node();
	root->Children.push_back(nnode);
}

// Adds a dummy node into a "into" gNode, both existent
void Gtree::add_node(gNode* into, gNode* dummy){
	into->Children.push_back(dummy);
}

// Prints the Tree in BFS order
//void Tree::print_tree(Node* root){
//	if (!root) return;
//	queue<Node*> q;
//	q.push(root);
//	while(q.size()){
//		int nodes = q.size();
//		for (int i = 0; i < nodes; i++){
//			Node* aux = q.front(); 
//			q.pop();
//			std::cout << "[] ";
//			if (aux->left) q.push(aux->left);
//			if (aux->right) q.push(aux->right);
//		}
//		std::cout << "\n";
//	}
//}

/* Transforms the existen tree into a queue of nodes in BFS
vector* v explicit_to_implicit(){
	
}
*/
