#ifndef WTinterfacea
#define WTinterfacea

#include <cstddef>
#include <cstdint>

class WTinterface {
    public:
        virtual ~WTinterface() = default;

        virtual uint64_t access(size_t i) const = 0;
        virtual size_t rank(uint64_t c, size_t i) const = 0;
        virtual size_t select(uint64_t c, size_t i) const = 0;
};

#endif