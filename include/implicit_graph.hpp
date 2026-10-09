#include "bitvector.hpp"
#include "bitvectorj.hpp"
#include "wt.hpp"
#include "uvector.hpp"

class ImplicitGraph{
    private:
        UVector N;            //array of neighbours, trocar para uvect
        BitVectorJ B;           //vertices bitvector
        size_t n_vert;               //number of vertices
        WaveletTreeSuccint wt;

    public:
        ImplicitGraph(BitVectorJ &Bv, UVector &neighbours);
        ~ImplicitGraph(); //deve existir apesar de vazio
        bool adj(size_t v, size_t u);
        size_t outdegree(size_t v);
        size_t outneigh(size_t v, size_t j);
        size_t rneigh(size_t v, size_t j);
        size_t indegree(size_t v);
        size_t degree(size_t v);
};