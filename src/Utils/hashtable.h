
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

/**
 * Generic Hashtable ADT.
 *
 * (type unsafe!)
 *
 * Dynamic growth (doubles capacity when full).
 * Open addressing to resolve collisions.
 */

#define HT_START_SIZE 64

#include "utils.h"

// typedefs for some function pointers
typedef void (*ht_kcopy)(void *dest, const void *src);
typedef void (*ht_vcopy)(void *dest, const void *src);
typedef void (*ht_kdtr)(void *p);
typedef void (*ht_vdtr)(void *p);
typedef bool (*ht_kequal)(const void *k1, const void *k2);
typedef u_long (*ht_hash)(const void *key);

/**
 * Buckets hold key value pairs.
 */
typedef struct Bucket {
    void *key;
    void *val;
} Bucket;

/**
 * Hashtable implemented as an array of buckets.
 * Size dynamically grows.
 * Supports generic keys and values with optional
 * copy constructors and destructors.
 */
typedef struct Hashtable {
    Bucket  **table;     // array of bucket pointers
    uint      size;      // number of valid entries in the ht
    uint      capacity;  // current max number of buckets
    size_t    key_width; // size of key data type
    size_t    val_width; // size of value data type
    ht_kcopy  kcopy;     // copy constructor for the key
    ht_vcopy  vcopy;     // copy constructor for the value
    ht_kdtr   kdtr;      // destructor for the key type
    ht_vdtr   vdtr;      // destructor for the value type
    ht_kequal kequal;    // function to check equality of keys
    ht_hash   hash_fn;   // hash function
} Hashtable;

/**
 * Initialize a hashtable.
 * Requires the sizeof key and value data types.
 * Requires the hash function.
 * Optional constructors and destructors for key and values.
 * Recommended capacity is HT_START_SIZE.
 */
Hashtable *ht_init(const size_t key_width, const size_t val_width, const ht_hash hash_fn, const ht_kcopy kcopy,
                   const ht_vcopy vcopy, const ht_kdtr kdtr, const ht_vdtr vdtr, const ht_kequal kequal,
                   const uint capacity);

/**
 * Inserts (a copy of) key value pair into the hash table.
 * Pointer to hashtable pointer is required as it can be
 * modified when resizing.
 * No type checking on key and value is done.
 * Open addressing is used to resolve collisions.
 *
 * Returns true on successful insertion, false otherwise.
 */
bool ht_insert(Hashtable **ht, const void *key, const void *val);

/**
 * Perform a lookup operation with the given key.
 *
 * Returns pointer to value if key exists, NULL otherwise.
 */
void *ht_lookup(Hashtable *ht, const void *key);

/**
 * Cleans up all allocations.
 */
void ht_free(Hashtable *ht);

/**
 * Doubles the hashtable capacity.
 * Primarily used in ht_insert, but the functionality
 * is also exposed to the user.
 */
Hashtable *ht_grow(Hashtable *ht);

/**
 * Some standard hash functions.
 */
u_long ht_polyRollingHash(const char **key);
u_long ht_moduloHash(int key);

#endif
