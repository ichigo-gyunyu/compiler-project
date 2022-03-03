
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "bitvector.h"

#define WORDSIZE 32
#define WS_BITS  5

#define MASK 0x1f // for easy modulo computation

uint bv_getsize(uint max) { return max / WORDSIZE + 1; }

int bv_init(Bitvector *bv, uint max) {
    *bv = calloc(bv_getsize(max), sizeof(int));
    return *bv != NULL;
}

void bv_set(const Bitvector bv, int pos) { bv[pos >> WS_BITS] |= (1 << (pos & MASK)); }

int bv_contains(const Bitvector bv, int pos) { return bv[pos >> WS_BITS] & (1 << (pos & MASK)); }

// TODO ensure a and b are of equal size
Bitvector bv_union(const Bitvector a, const Bitvector b, uint max) {
    Bitvector un = a;

    for (uint i = 0; i < max; i++) {
        un[i >> WS_BITS] |= ((1 << (i & MASK)) & b[i >> WS_BITS]);
    }
    return un;
}
