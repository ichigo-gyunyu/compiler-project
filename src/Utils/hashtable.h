// insert into table
// lookup
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "utils.h"

typedef struct data {
    char *key;
    int   val;
} data;

typedef data *hashtable;

/**
 * Initializes a hash table of given size
 */
int ht_init(uint sz);

/**
 * Print the hash table
 */
void ht_print(hashtable ht);

/**
 * Tries to insert d into the hashtable
 * Returns 0 if not successful
 * Returns 1 if successful
 */
int ht_insert(hashtable ht, data d);

/**
 * Checks if the data is in the hashtable
 * Returns 0 if data is not in hashtable
 * Returns 1 if data is in
 */
int ht_lookup(hashtable ht, data d);

#endif
