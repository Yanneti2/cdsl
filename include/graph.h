#include "bitvector.hpp"
#include "bitvectorj.hpp"
#include "wt.h"

class Graph{
    private:
        BitVector N;           //array of neighbours
        BitVectorJ B;           //vertices bitvector
        unsigned long v;        //number of vertices
        size_t element_size;

    public:
        Graph(size_t vertices);
        ~Graph();
        void AddEdge(unsigned long v, unsigned long u);
        bool adj(unsigned long v, unsigned long u);
        size_t outdegree(unsigned long v);
};