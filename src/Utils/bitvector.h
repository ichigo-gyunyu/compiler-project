#ifndef BITVECTOR_H
#define BITVECTOR_H

#include "utils.h"

typedef int *Bitvector;

/**
 * Initializes a bitvector
 * with capacity given by max
 */
int bv_init(Bitvector *bv, uint max);

/**
 * Returns capacity of the bitvector
 * specified by max
 */
uint bv_getsize(uint max);

/**
 * Sets the bit at position
 * in the given bitvector
 */
void bv_set(const Bitvector bv, int pos);

/**
 * Checkif the bit at pos
 * is set in the given bitvector
 */
int bv_contains(const Bitvector bv, int pos);

/**
 * Returns the union of 2 bitvectors
 */
Bitvector bv_union(const Bitvector a, const Bitvector b, uint max_size);

#endif
