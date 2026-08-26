#include "../include/huffman.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <queue>
#include <cmath>
#include <map>
using namespace std;

int main (void) {
	// WORST CASE ENTROPY TEST:
	
	// 111 110 101 011 100 010 001 000, sequence of n = 3 bits with num of encodings of same lenght = 8
	cout << worst_case_entropy(8.0) << endl;	
	// abraabracadabra => a,b,c,d,r with |U| = 15 elements, such that are 5 distinc chars  with diff Pr(u), to encode them all with encodings of same len we need at least:
	cout << worst_case_entropy(5.0) << " bits" << endl << "Ceil: " << ceil(worst_case_entropy(5.0)) << endl << endl;

	// COUT OF ALL THE STRING RESPECTIVES:
	cout << "T1: bracadabra" << endl << "T2: bananaabacate" << endl << "T3: abracadabra" << endl << "teste1: aaaaaaa" << endl << "teste2: a$b$$c$$$" << endl << "teste3: 10000*a + bcde" << endl << "teste4: 100*(all ascii printable chars)" << endl << "teste5: 1000000*y + x" << endl << "teste6: file with string of order 1000000 (1M chars)" << endl << endl;

	// PROBABILITIES TEST:

	// 1,3,3,3,4 => "a", "abc", "bca", "cab", "caba"	
	// esperado: b 0.2, r 0.2, a 0.4, c 0.1, d 0.1, 
	string str = "bracadabra";
	auto start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> rel = probabilities_frequency(str,true);	
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = end - start;
	cout << "T1 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp(rel);
	while (!temp.empty()){
		cout << temp.top()->data << " " << temp.top()->frequency << endl;
		temp.pop();
	}
	cout << endl;
	
	string s = "bananaabacate";
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> rel1 = probabilities_frequency(s,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "T2 probabiities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp1(rel1); 
	while (!temp1.empty())
	{
		cout << temp1.top()->data << " " << temp1.top()->frequency << endl;
		temp1.pop();
	}	
	cout << endl;

	string gonzalo = "abracadabra";
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> navarro = probabilities_frequency(gonzalo,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "T3 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp2(navarro); 
	while (!temp2.empty()){
		cout << temp2.top()->data << " " << temp2.top()->frequency << endl;
		temp2.pop();
	}
	cout << endl;

	string teste1 = "aaaaaaa"; 
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> teste11 = probabilities_frequency(teste1,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste1 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp3(teste11); 
	while (!temp3.empty()){
		cout << temp3.top()->data << " " << temp3.top()->frequency << endl;
		temp3.pop();
	}
	cout << endl;

	string teste2 = "a$b$$c$$$";
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> teste22 = probabilities_frequency(teste2,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste2 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp4(teste22); 
	while (!temp4.empty()){
		cout << temp4.top()->data << " " << temp4.top()->frequency << endl;
		temp4.pop();
	}
	cout << endl;

	string teste3 = string(10000, 'a') + string(1, 'b') + string(1, 'c') + string(1, 'd') + string(1, 'e');
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> teste33 = probabilities_frequency(teste3,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste3 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp5(teste33); 
	while (!temp5.empty()){
		cout << temp5.top()->data << " " << temp5.top()->frequency << endl;
		temp5.pop();
	}
	cout << endl;

	string teste4 = ""; 
	for(int c = 32; c <= 126; c++) teste4 += string(100, (char)c);
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> teste44 = probabilities_frequency(teste4,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste4 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp6(teste44); 
	while (!temp6.empty()){
		cout << temp6.top()->data << " " << temp6.top()->frequency << endl;
		temp6.pop();
	}
	cout << endl;

	string teste5 = string(1000000, 'y') + string(1, 'x');
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> teste55 = probabilities_frequency(teste5,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste5 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp7(teste55); 
	while (!temp7.empty()){
		cout << temp7.top()->data << " " << temp7.top()->frequency << endl;
		temp7.pop();
	}
	cout << endl;

	ifstream f("../tests/input_1M.txt");
	string teste6((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());	
	start = std::chrono::high_resolution_clock::now();
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> teste66 = probabilities_frequency(teste6,true);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste6 probabilities calculation time: " << ms_double << endl;
	priority_queue<BinaryTree::Node*, vector<BinaryTree::Node*>, BinaryTree::compareNodes> temp8(teste66); 
	while (!temp8.empty()){
		cout << temp8.top()->data << " " << temp8.top()->frequency << endl;
		temp8.pop();
	}
	cout << endl;

	// HUFFAN BUILD AND ENCODING TEST:

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root = build_huffman(rel);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout <<  "T1 build time: " << ms_double << endl;
	map<char,string> t;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root,t,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hhh : t){cout<<hhh.first<<" - "<<hhh.second<<endl; }
	cout << "T1 encoding time: " << ms_double << endl;
	cout<<endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root1 = build_huffman(rel1);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "T2 build time: "<< ms_double << endl;
	map<char,string> t1;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root1,t1,"");	
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& h : t1){ cout << h.first <<" - " << h.second << endl; }
	cout << "T2 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root2 = build_huffman(navarro);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "T3 build time: " << ms_double << endl;
	map<char,string> t2;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root2,t2,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t2) { cout << hh.first << " - " << hh.second << endl; }
	cout << "T3 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root3 = build_huffman(teste11);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste1 build time: " << ms_double << endl;
	map<char,string> t3;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root3,t3,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t3) { cout << hh.first << " - " << hh.second << endl; }
	cout << "teste1 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root4 = build_huffman(teste22);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste2 build time: " << ms_double << endl;
	map<char,string> t4;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root4,t4,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t4) { cout << hh.first << " - " << hh.second << endl; }
	cout << "teste2 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root5 = build_huffman(teste33);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste3 build time: " << ms_double << endl;
	map<char,string> t5;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root5,t5,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t5) { cout << hh.first << " - " << hh.second << endl; }
	cout << "teste3 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root6 = build_huffman(teste44);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste4 build time: " << ms_double << endl;
	map<char,string> t6;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root6,t6,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t6) { cout << hh.first << " - " << hh.second << endl; }
	cout << "teste4 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root7 = build_huffman(teste55);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste5 build time: " << ms_double << endl;
	map<char,string> t7;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root7,t7,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t7) { cout << hh.first << " - " << hh.second << endl; }
	cout << "teste5 encoding time: " << ms_double << endl;
	cout << endl;

	start = std::chrono::high_resolution_clock::now();
	BinaryTree::Node* root8 = build_huffman(teste66);
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	cout << "teste6 build time: " << ms_double << endl;
	map<char,string> t8;
	start = std::chrono::high_resolution_clock::now();
	huffman_coding(root8,t8,"");
	end = std::chrono::high_resolution_clock::now();
	ms_double = end - start;
	for(auto const& hh : t8) { cout << hh.first << " - " << hh.second << endl; }
	cout << "teste6 encoding time: " << ms_double << endl;
	cout << endl;

	// MINIMUM AND AVERAGE CODE LENGTH TEST:
	
	// navarro: probability = navarro(priority_queue)->frequency, code_lenght = t2->second. 
	vector<unsigned long int> vvv1;
	vector<double>vvv2;
	while(!rel.empty()){
		BinaryTree::Node* atual = rel.top();
		rel.pop();
		double freq = atual->frequency;
		unsigned long int size = t[atual->data].size();
		vvv1.push_back(size);
		vvv2.push_back(freq);
	}
	double setSize = vvv1.size();
	cout<<"T1 Average Code Length: "<<average_length_codes(vvv1,vvv2)<<endl<<"T1 Minimum Average Code Length: "<<minimum_average_code_length(vvv2)<<endl<<"T1 Worst Case Entropy: "<<worst_case_entropy(setSize)<<endl<<endl;

	vector<unsigned long int> vv1;
	vector<double>vv2;
	while(!rel1.empty()){
		BinaryTree::Node* atual = rel1.top();
		rel1.pop();
		double freq = atual->frequency;
		unsigned long int size = t1[atual->data].size();
		vv1.push_back(size);
		vv2.push_back(freq);
	}
	double setSizee = vv1.size();
	cout<<"T2 Average Code Length: "<<average_length_codes(vv1,vv2)<<endl<<"T2 Minimum Average Code Length: "<<minimum_average_code_length(vv2)<<endl<<"T2 Worst Case Entropy: "<<worst_case_entropy(setSizee)<<endl<<endl;

	vector<unsigned long int> v1;
	vector<double>v2;
	while(!navarro.empty()){
		BinaryTree::Node* atual = navarro.top();
		navarro.pop();
		double freq = atual->frequency;
		unsigned long int size = t2[atual->data].size();
		v1.push_back(size);
		v2.push_back(freq);
	}
	double setSizeee = v1.size();
	cout<<"T3 Average Code Length: "<<average_length_codes(v1,v2)<<endl<<"T3 Minimum Average Code Length: "<<minimum_average_code_length(v2)<<endl<<"T3 Worst Case Entropy: "<<worst_case_entropy(setSizeee)<<endl<<endl;

	vector<unsigned long int> v3;
	vector<double>v33;
	while(!teste11.empty()){
		BinaryTree::Node* atual = teste11.top();
		teste11.pop();
		double freq = atual->frequency;
		unsigned long int size = t3[atual->data].size();
		v3.push_back(size);
		v33.push_back(freq);
	}
	double setSize3 = v3.size();
	cout<<"teste1 Average Code Length: "<<average_length_codes(v3,v33)<<endl<<"teste1 Minimum Average Code Length: "<<minimum_average_code_length(v33)<<endl<<"teste1 Worst Case Entropy: "<<worst_case_entropy(setSize3)<<endl<<endl;

	vector<unsigned long int> v4;
	vector<double>v44;
	while(!teste22.empty()){
		BinaryTree::Node* atual = teste22.top();
		teste22.pop();
		double freq = atual->frequency;
		unsigned long int size = t4[atual->data].size();
		v4.push_back(size);
		v44.push_back(freq);
	}
	double setSize4 = v4.size();
	cout<<"teste2 Average Code Length: "<<average_length_codes(v4,v44)<<endl<<"teste2 Minimum Average Code Length: "<<minimum_average_code_length(v44)<<endl<<"teste2 Worst Case Entropy: "<<worst_case_entropy(setSize4)<<endl<<endl;

	vector<unsigned long int> v5;
	vector<double>v55;
	while(!teste33.empty()){
		BinaryTree::Node* atual = teste33.top();
		teste33.pop();
		double freq = atual->frequency;
		unsigned long int size = t5[atual->data].size();
		v5.push_back(size);
		v55.push_back(freq);
	}
	double setSize5 = v5.size();
	cout<<"teste3 Average Code Length: "<<average_length_codes(v5,v55)<<endl<<"teste3 Minimum Average Code Length: "<<minimum_average_code_length(v55)<<endl<<"teste3 Worst Case Entropy: "<<worst_case_entropy(setSize5)<<endl<<endl;

	vector<unsigned long int> v6;
	vector<double>v66;
	while(!teste44.empty()){
		BinaryTree::Node* atual = teste44.top();
		teste44.pop();
		double freq = atual->frequency;
		unsigned long int size = t6[atual->data].size();
		v6.push_back(size);
		v66.push_back(freq);
	}
	double setSize6 = v6.size();
	cout<<"teste4 Average Code Length: "<<average_length_codes(v6,v66)<<endl<<"teste4 Minimum Average Code Length: "<<minimum_average_code_length(v66)<<endl<<"teste4 Worst Case Entropy: "<<worst_case_entropy(setSize6)<<endl<<endl;

	vector<unsigned long int> v7;
	vector<double>v77;
	while(!teste55.empty()){
		BinaryTree::Node* atual = teste55.top();
		teste55.pop();
		double freq = atual->frequency;
		unsigned long int size = t7[atual->data].size();
		v7.push_back(size);
		v77.push_back(freq);
	}
	double setSize7 = v7.size();
	cout<<"teste5 Average Code Length: "<<average_length_codes(v7,v77)<<endl<<"teste5 Minimum Average Code Length: "<<minimum_average_code_length(v77)<<endl<<"teste5 Worst Case Entropy: "<<worst_case_entropy(setSize7)<<endl<<endl;

	vector<unsigned long int> v8;
	vector<double>v88;
	while(!teste66.empty()){
		BinaryTree::Node* atual = teste66.top();
		teste66.pop();
		double freq = atual->frequency;
		unsigned long int size = t8[atual->data].size();
		v8.push_back(size);
		v88.push_back(freq);
	}
	double setSize8 = v8.size();
	cout<<"teste6 Average Code Length: "<<average_length_codes(v8,v88)<<endl<<"teste6 Minimum Average Code Length: "<<minimum_average_code_length(v88)<<endl<<"teste6 Worst Case Entropy: "<<worst_case_entropy(setSize8)<<endl<<endl;

	return 0;
}
