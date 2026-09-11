#include "bitvector.h"
#include "graph.h"
#include <iostream>
#include <cmath>


Graph::Graph(size_t vertices) {
    this->v = vertices;  
    this->element_size = ceil(log2(vertices));
    this->B = new BitVector(vertices);
    for(size_t i; i < B->size(); i++){
        B->set1(i);
    }
    // this->N = (unsigned long*) calloc(pow(v, 2), sizeof(unsigned long));
    this->N = new BitVector(vertices*element_size);
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

unsigned long Graph::adj(unsigned long v, unsigned long u){
    
}



