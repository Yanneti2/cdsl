#include "bitvector.hpp"
#include "graph.h"

#include <iostream>
#include <cmath>

using namespace std;
//todo:rename graph
Graph::Graph(size_t vertices) {
    this->v = vertices;  
    this->element_size = ceil(log2(vertices));
    this->B = BitVectorJ(vertices);
    for(size_t i; i < B.size(); i++){
        B.set1(i);
    }
    // this->N = (unsigned long*) calloc(pow(v, 2), sizeof(unsigned long));
    this->N = BitVector(vertices*element_size);
    if(!N)
        throw new std::bad_alloc();
}


Graph::~Graph(){
    delete N;
    delete B;
}


//TODO: overload de put para inserir apenas um bit em B
void Graph::AddEdge(unsigned long v, unsigned long u){
    B->put(false, B->naive_select1(v+1));
    N->append(u, element_size);
}


/**
 * goal: verify if edge(v,u) exists
 * pega a posição de v no bitvetor B
 * r1: retorna a qtd de u no vetor até o nó v
 * r2: retorn a qtd de u no vetor até o nó sucessor de v
 * se r2-r1=1 -> true; else false
 * 
 * obs: analisar se vale a pena usar o jacabson
 */
bool Graph::adj(unsigned long v, unsigned long u){
    size_t suc_v = B->naive_select1(B->naive_rank1(v + 1) - 1); 
    size_t pos_v = B->naive_select1(v);
    size_t r1 = rankc(u, v, Graph->wt); // rankc parte de uma wt
    size_t r2 = rankc(u, suc_v, Graph->wt);
    return r2 - r1 == 1;
}


/**
 * pega a pos de v no bitvector
 * 
 */
size_t Graph::outdegree(unsigned long v){
    size_t pos_v = B->naive_select1(v);
    size_t suc_v = B->naive_select1(B->naive_rank1(pos_v)) - pos_v - 1;
    return suc_v;
}


/**
 * retorna vetor de vizinhos de saída de um nó
 */
Graph::outneigh(unsigned long v){
    size_t d = outdegree(v);
}

