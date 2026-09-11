#include "bitvector.h"

class Graph{
    private:
        BitVector* N; //array of neighbours
        BitVector* B; //vertices bitvector
        unsigned long v; //number of vertices
        size_t element_size;

    public:
        Graph(size_t vertices);
        ~Graph();
        void AddEdge(unsigned long v, unsigned long u);
        unsigned long adj(unsigned long v, unsigned long u);
};