
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "hashtable.h"

uint ht_hash(char *key, uint modulus) {
    uint hash_value = 0;
    // compute hash of string key using a polynomial rolling hash
    const uint p    = 53;
    const uint m    = 1e9 + 9;
    ulong      ppow = 1;
    for (size_t i = 0; i < strlen(key); i++) {
        hash_value = (hash_value + (key[i] - 'a' + 1) * ppow) % m;
        ppow       = (ppow * p) % m; // TODO precompute
    }

    return hash_value % modulus;
}

int ht_init(Hashtable *ht, uint sz) {
    ht->size  = sz;
    ht->table = calloc(sz, sizeof(ht_data *));

    return ht->table != NULL;
}

int ht_insert(Hashtable *ht, char *key, int val) {
    uint hash = ht_hash(key, ht->size);

    ht_data *d = malloc(sizeof *d);
    d->key     = key;
    d->val     = val;

    // linear probe
    uint indx       = hash;
    uint num_probed = 0;
    while (ht->table[indx] != NULL) {
        indx++;
        indx %= ht->size;
        num_probed++;
        if (num_probed == ht->size) {
            return -1;
        }
    }

    ht->table[indx] = d;
    return indx;
}

int ht_lookup(Hashtable ht, char *key) {
    uint indx       = ht_hash(key, ht.size);
    uint num_probed = 0;
    while (ht.table[indx] != NULL && strcmp(ht.table[indx]->key, key)) {
        indx++;
        indx %= ht.size;
        num_probed++;
        if (num_probed == ht.size)
            return -1;
    }

    if (ht.table[indx] == NULL)
        return -1;

    return ht.table[indx]->val;
}

void ht_print(Hashtable ht) {
    for (uint i = 0; i < ht.size; i++) {
        if (ht.table[i] == NULL)
            printf("empty\n");
        else
            printf("%s --> %d\n", ht.table[i]->key, ht.table[i]->val);
    }
}
