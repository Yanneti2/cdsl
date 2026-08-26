
#include <vector>

using namespace std;

#ifndef GENERAL_TREE_H
#define GENERAL_TREE_H

class Gtree{
	public:
		typedef struct gNode{
			//bool marked;
			vector<gNode*> Children;
		}gNode;

		Gtree();
		~Gtree();
		
		gNode* create_node();
		void append_nnode(gNode* root);
		void add_node(gNode* into, gNode* dummy);

		void print_tree(gNode* root);

		gNode* getRoot();
		//bool getMarked();
		vector<gNode*> getChildren();

		bool isEmpty(gNode* root);

	private:
		gNode* root;
};

#endif
