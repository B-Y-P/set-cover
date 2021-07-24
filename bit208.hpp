#pragma once
#include <stdint.h>

typedef uint64_t type;
typedef uint16_t stype;

class bit208{
public:
    inline constexpr bit208() noexcept = default;
    bit208(const type &t) : a(t) {}
    inline bit208(const type &w, const type &x, const type &y, const stype &z) : a(w), b(x), c(y), d(z) {}
    inline bit208(const bit208  &bit) : a(bit.a), b(bit.b), c(bit.c), d(bit.d) {}

    bit208  operator~ () { return bit208(~a,~b,~c,~d); }
    bool    operator!=(const bit208 &bit) const { return a!=bit.a || b!=bit.b || c!=bit.c || d!=bit.d; }
    bit208& operator&=(const bit208 &bit) { a&=bit.a; b&=bit.b; c&=bit.c; d&=bit.d; return *this; }

    inline bit208& operator= (const bit208 &bit) {  a=bit.a; b=bit.b; c=bit.c; d=bit.d; return *this;  }
    inline bit208& operator|=(const bit208 &bit) { a|=bit.a; b|=bit.b; c|=bit.c; d|=bit.d; return *this; }
    inline bit208  operator| (const bit208 &bit) const { return bit208((a|bit.a),(b|bit.b),(c|bit.c),(d|bit.d)); }
    inline bool    operator==(const bit208 &bit) const { return a==bit.a && b==bit.b && c==bit.c && d==bit.d; }

public:
    inline unsigned int count() const { return (__builtin_popcountll(a) + __builtin_popcountll(b) + __builtin_popcountll(c) + __builtin_popcount(d)); }
    inline unsigned int CountComb(const bit208 &bit) const { return (__builtin_popcountll(a|bit.a) + __builtin_popcountll(b|bit.b) + __builtin_popcountll(c|bit.c) + __builtin_popcount(d|bit.d)); }
    inline bool Contributes(const bit208 &bit) const { return (a!=(a|bit.a) || b!=(b|bit.b) || c!=(c|bit.c) || d!=(d|bit.d)); }

    bit208& reset(){ a=b=c=0;d=0; return *this; }
    bit208& set(unsigned short val){
        if(val>=0   && val<64) { a |= (type(1)<<val); return *this; }
        if(val>=64  && val<128){ b |= (type(1)<<val); return *this; }
        if(val>=128 && val<192){ c |= (type(1)<<val); return *this; }
        if(val>=192 && val<208){ d |= (type(1)<<val); return *this; }
        return *this;
    }

protected:
    type  a=0,b=0,c=0;
    stype d=0;
};
