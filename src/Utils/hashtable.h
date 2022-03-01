#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "utils.h"

typedef struct data {
    char *key;
    int   val;
} ht_data;

typedef struct hashtable {
    ht_data **table;
    uint      size;
} Hashtable;

/**
 * Initializes a hash table of given size
 */
int ht_init(Hashtable *ht, uint sz);

/**
 * Print the hash table
 */
void ht_print(Hashtable ht);

/**
 * Tries to insert d into the hashtable
 * Returns -1 if not successful
 * Returns indx if successful
 */
int ht_insert(Hashtable *ht, char *key, int val);

/**
 * Checks if the data is in the hashtable
 * Returns -1 if data is not in hashtable
 * Returns the kay if data is in
 */
int ht_lookup(Hashtable ht, char *key);

#endif
