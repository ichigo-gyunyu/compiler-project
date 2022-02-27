#include "bitvector.h"

#define WORDSIZE 32
#define WS_BITS  5

#define MASK 0x1f // for easy modulo computation

uint bv_getsize(uint max) { return max / WORDSIZE + 1; }

int bv_init(bitvector *bv, uint max) {
    *bv = calloc(bv_getsize(max), sizeof(int));
    return *bv != NULL;
}

void bv_set(const bitvector bv, int pos) { bv[pos >> WS_BITS] |= (1 << (pos & MASK)); }

int bv_contains(const bitvector bv, int pos) { return bv[pos >> WS_BITS] & (1 << (pos & MASK)); }

// TODO ensure a and b are of equal size
bitvector bv_union(const bitvector a, const bitvector b, uint max) {
    bitvector un = a;

    for (uint i = 0; i < max; i++) {
        un[i >> WS_BITS] |= ((1 << (i & MASK)) & b[i >> WS_BITS]);
    }
    return un;
}
