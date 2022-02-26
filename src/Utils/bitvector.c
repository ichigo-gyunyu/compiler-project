#include "bitvector.h"

#define WORDSIZE 32
#define WS_BITS  5

#define MASK 0x1f // for easy modulo computation

int bv_init(bitvector *bv, int max) {
    *bv = calloc(max / WORDSIZE + 1, sizeof(int));
    return *bv != NULL;
}

void bv_set(bitvector bv, int pos) {
    bv[pos >> WS_BITS] |= (1 << (pos & MASK));
}

int bv_contains(bitvector bv, int pos) {
    return bv[pos >> WS_BITS] & (1 << (pos & MASK));
}
