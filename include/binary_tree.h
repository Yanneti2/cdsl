#ifndef BINARY_TREEH
#define BINARY_TREEH

class BinaryTree {
	public:
		struct Node {
			Node* left;
			Node* right;
			char data;
			double frequency;

			Node(char d, double f) : data(d), frequency(f), left(nullptr), right(nullptr) {}	
		};

		struct compareNodes {
		    bool operator()(Node* a, Node* b) {
			return (a->frequency > b->frequency);
		    }
		};

		BinaryTree();
		~BinaryTree();
		Node* create_node(char data, double freq);
		void print_tree(Node* root);
		Node* getRoot();
		bool getMarked();
		bool isEmpty(Node* root);

	private:
		Node* root;
};

#endif
