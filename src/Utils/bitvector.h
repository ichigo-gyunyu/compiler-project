
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

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
 * Performs a = a U b
 */
void bv_union(Bitvector a, const Bitvector b, uint max_size);

#endif
