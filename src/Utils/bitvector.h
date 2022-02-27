#ifndef BITVECTOR_H
#define BITVECTOR_H

#include "utils.h"

typedef int *bitvector;

/**
 * Initializes a bitvector
 * with capacity given by max
 */
int bv_init(bitvector *bv, uint max);

/**
 * Returns capacity of the bitvector
 * specified by max
 */
uint bv_getsize(uint max);

/**
 * Sets the bit at position
 * in the given bitvector
 */
void bv_set(const bitvector bv, int pos);

/**
 * Checkif the bit at pos
 * is set in the given bitvector
 */
int bv_contains(const bitvector bv, int pos);

/**
 * Returns the union of 2 bitvectors
 */
bitvector bv_union(const bitvector a, const bitvector b, uint max_size);

#endif
