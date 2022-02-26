#ifndef BITVECTOR_H
#define BITVECTOR_H

#include "utils.h"

typedef int *bitvector;

/**
 * Initializes a bitvector
 * with capacity given by max
 */
int bv_init(bitvector *bv, int max);

/**
 * Sets the bit at position
 * in the given bitvector
 */
void bv_set(bitvector bv, int pos);

/**
 * Checkif the bit at pos
 * is set in the given bitvector
 */
int bv_contains(bitvector bv, int pos);

#endif
