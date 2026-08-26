#include "../include/general_tree.h"
#include "../include/binary_tree.h"
#include "../include/bitvector.h"
#include "../include/louds.h"
#include "../include/bp.h"
#include <iostream>
#include <vector>
#include <string>

// Fazer os assert para as funcoes (make)
// Test difference in speed using <vector> and bitVector();
int main (void){
	// size = 2N, N = |treeNodes|
	Tree* t1 = new Tree();
	Tree::Node* r1 = t1->getRoot();
	Tree::Node* aux = r1;
	for (int i = 0; i < 10; i++){
		aux->left = t1->create_node();
		aux = aux->left;
			
	}
	bitVector* B1 = new bitVector(64,2.0); //size = 22
	dfs_bt(B1,r1);
	B1->print();
	//vector<int> v;
	//dfs_bt2(v, r1);
	//std::cout << v.size() << "\n";
	//for (unsigned long int i = 0; i < v.size(); i++) { std::cout << v[i]; }
	//std::cout << "\n";

	Tree* t2 = new Tree();
	Tree::Node* r2 = t2->getRoot();
	r2->left = t2->create_node();
	r2->right = t2->create_node();
	Tree::Node* au = r2->left;
	au->left = t2->create_node();
	bitVector* B2 = new bitVector(64,2.0); //size = 8
	dfs_bt(B2,r2);
	B2->print();
	//vector<int> h;
	//dfs_bt2(h, r2);
	//std::cout << h.size() << "\n";
	//for (unsigned long int i = 0; i < h.size(); i++) { std::cout << h[i]; }
	//std::cout << "\n";	

	Tree* t3 = new Tree();
	Tree::Node* r3 = t3->getRoot();
	r3->left = t3->create_node();
	Tree::Node* a = r3->left;
	a->left = t3->create_node();
	a->right = t3->create_node();
	bitVector* B3 = new bitVector(64, 2.0); //size = 8
	dfs_bt(B3,r3);
	B3->print();
	//vector<int> t; dfs_bt2(t, r3);
	//std::cout << t.size() << "\n";
	//for (unsigned long int i = 0; i < t.size(); i++) { std::cout << t[i]; }
	//std::cout << "\n";

	//vector<int> j;
	//j.push_back(0);
	//j.push_back(1);
	//std::cout << j.size() << "\n";
	//for (unsigned long int i = 0; i < j.size(); i++){
	//	std::cout << i;
	//}
	//std::cout << "\n";
	
	Gtree* g1 = new Gtree();
	Gtree::gNode* rg1 = g1->getRoot();
	for (int i = 0; i < 3; i++){ g1->append_nnode(rg1); }
	bitVector* B4 = new bitVector(64,2.0);	
	dfs_gt(B4,rg1);
	B4->print();
	//vector<char> nn;
	//bp_build(nn,rg1);
	//for(int i=0; i<nn.size();i++)std::cout<<nn[i];
	//std::cout<<std::endl;

	Gtree* g2 = new Gtree();
	Gtree::gNode* rg2 = g2->getRoot();
	Gtree::gNode* dummy = g2->create_node();
	g2->append_nnode(rg2);
	g2->add_node(rg2,dummy);
	for (int i = 0; i < 3; i++)g2->append_nnode(dummy);
	g2->append_nnode(rg2);
	bitVector* B5 = new bitVector(64,2.0);
	dfs_gt(B5,rg2);
	B5->print();
	//vector<char> n;
	//bp_build(n,rg1);
	//for(int i=0; i<n.size();i++)std::cout<<n[i];
	//std::cout<<std::endl;

	Gtree* g3 = new Gtree();
	Gtree::gNode* rg3 = g3->getRoot();
	for (int i = 0; i < 4; i++){ g3->append_nnode(rg3); }
	bitVector* B6 = new bitVector(64,2.0);	
	dfs_gt(B6,rg3);
	B6->print();
	//vector<char> b;
	//bp_build(b,rg1);
	//for(int i=0; i<b.size();i++)std::cout<<b[i];
	//std::cout<<std::endl;


	Gtree* gg = new Gtree();
	Gtree::gNode* rgg = gg->getRoot();
	Gtree::gNode* dois = gg->create_node();
	Gtree::gNode* cinco = gg->create_node();
	gg->add_node(rgg,dois);
	gg->add_node(rgg,cinco);
	gg->append_nnode(rgg);
	gg->append_nnode(dois);
	gg->append_nnode(dois);
	Gtree::gNode* seis = gg->create_node();
	Gtree::gNode* treze = gg->create_node();
	gg->add_node(cinco,seis);
	gg->add_node(cinco,treze);
	Gtree::gNode* sete = gg->create_node();
	gg->add_node(seis,sete);
	gg->append_nnode(seis);
	Gtree::gNode* quatorze = gg->create_node();
	gg->add_node(treze,quatorze);
	Gtree::gNode* oito = gg->create_node();
	gg->add_node(sete,oito);
	for(int i=0;i<3;i++)gg->append_nnode(oito);
	gg->append_nnode(quatorze);
	Gtree::gNode* desesseis = gg->create_node();
	gg->add_node(quatorze,desesseis);
	gg->append_nnode(desesseis);
	gg->append_nnode(quatorze);
	gg->append_nnode(quatorze);
	vector<char> naoexiste;
	bp_build(naoexiste,rgg);
	for(int i=0;i<naoexiste.size();i++)std::cout << naoexiste[i];
	std::cout << "\n";
	bitVector* B7 = new bitVector(64,2.0);
	dfs_gt(B7, rgg);
	B7->print();
	//string str = "";
	//bp_build(str, rgg);
	
	//Gtree* teste = new Gtree();
	//Gtree::gNode* root = teste->getRoot();
	//teste->append_nnode(root);
	//teste->append_nnode(root);
	//teste->append_nnode(root);
	//Gtree::gNode* oi = teste->create_node();
	//teste->add_node(root,oi);
	//vector<char> vv;
	//bp_build(vv,root);
	//for(int i=0;i<vv.size();i++)std::cout<<vv[i];
	//std::cout << "\n";
	
	Gtree* loud = new Gtree();
	Gtree::gNode* one = loud->getRoot();
	Gtree::gNode* two = loud->create_node();
	Gtree::gNode* three = loud->create_node();
	loud->add_node(one,two);
	loud->add_node(one,three);
	loud->append_nnode(one);
	loud->append_nnode(two);
	loud->append_nnode(two);
	Gtree::gNode* seven = loud->create_node();
	Gtree::gNode* eight = loud->create_node();
	loud->add_node(three,seven);
	loud->add_node(three,eight);
	Gtree::gNode* nine = loud->create_node();
	Gtree::gNode* eleven = loud->create_node();
	loud->add_node(seven,nine);
	loud->append_nnode(seven);
	loud->add_node(eight,eleven);
	Gtree::gNode* twelve = loud->create_node();
	Gtree::gNode* fourteen = loud->create_node();
	loud->add_node(nine,twelve);
	loud->append_nnode(eleven);
	loud->add_node(eleven,fourteen);
	loud->append_nnode(eleven);
	loud->append_nnode(eleven);
	loud->append_nnode(twelve);
	loud->append_nnode(twelve);
	loud->append_nnode(twelve);
	loud->append_nnode(fourteen);
	bitVector* B8 = new bitVector(64,2.0);
	//build_louds(bitVector* B, Gtree::gNode* node);
	build_louds(B8,one);
	B8->print();

	Gtree* house = new Gtree();
	Gtree::gNode* raiz = house->getRoot();
	house->append_nnode(raiz);
	Gtree::gNode* lchild = house->create_node();
	Gtree::gNode* rchild = house->create_node();
	house->add_node(raiz,lchild);
	house->add_node(raiz,rchild);
	house->append_nnode(lchild);
	house->append_nnode(lchild);
	house->append_nnode(rchild);
	bitVector* B9 = new bitVector(1,2.0);
	build_louds(B9,raiz);
	B9->print();
	
	return 0;
}
