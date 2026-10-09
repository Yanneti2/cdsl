#include "bitvector.hpp"
#include "implicitGraph.hpp"
#include "wt.hpp"
#include "uvector.hpp"
#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace std;
//construtor q ja recebe um grafo construido e transforma em comprimido
//add arestas para o construtor

//todo:rename graph
ImplicitGraph::ImplicitGraph(BitVectorJ &Bv, UVector &neighbours) : B(Bv), N(neighbours) {
    B.init();
    n_vert = B.rank0(B.size());
    wt = WaveletTreeSuccint(N);

    if(n_vert != N.size){
        throw invalid_argument("Bitvector e UVector possuem número de arestas diferentes");
    }

    B.build_select0();
    B.build_select1();
}

ImplicitGraph::~ImplicitGraph(){
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
bool ImplicitGraph::adj(size_t v, size_t u){
    size_t suc_v = B.select1(B.rank1(v + 1) - 1); 
    size_t pos_v = B.select1(v);
    size_t r1 = wt.rankc(u, v);
    size_t r2 = wt.rankc(u, suc_v);
    return r2 - r1 == 1;
}


/**
 * pega a pos de v no bitvector
 * 
 */
size_t ImplicitGraph::outdegree(size_t v){
    size_t pos_v = B.select1(v);
    size_t suc_v = B.select1(B.rank1(pos_v)) - pos_v - 1;
    return suc_v;
}


/**
 * retorna vetor de vizinhos de saída de um nó
 */
size_t ImplicitGraph::outneigh(size_t v, size_t j){
    return N[B.rank0(B.select1(v)) + j];
}

/**
 * j-ésimo inneighbour
 */
size_t ImplicitGraph::rneigh(size_t v, size_t j){
    return B.select0(wt.selectc(v,j)) - wt.selectc(v, j);
}

size_t ImplicitGraph::indegree(size_t v){
    return wt.rankc(v, B.rank0(B.size()));
}

size_t ImplicitGraph::degree(size_t v){
    return outdegree(v) - indegree(v);
}