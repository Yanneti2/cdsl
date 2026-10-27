#include "wtInterface.hpp"
#include "bitvectorj.hpp"

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

#ifndef WTimplicit
#define WTimplicit

class WaveletTreeImplicit : public WTinterface {
    private:
        vector<BitVectorJ*> V;
        string alpha;

        size_t _select(uint64_t c, size_t i, size_t it, size_t a, size_t b) const;
    
    public:
        WaveletTreeImplicit(string S);

        uint64_t access(size_t i) const override;
        size_t rank(uint64_t c, size_t i) const override;
        size_t select(uint64_t c, size_t i) const override;

        void print();

        ~WaveletTreeImplicit();
};

#endif