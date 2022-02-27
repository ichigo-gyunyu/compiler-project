#include "hashtable.h"

uint ht_hash(char *key, uint modulus) {
    uint hash_value = 0;
    // compute hash of string key

    return hash_value;
}

int ht_init(hashtable *ht, uint sz) {
    ht->size  = sz;
    ht->table = calloc(sz, sizeof(ht_data *));

    return ht->table != NULL;
}

int ht_insert(hashtable *ht, char *key, int val) {
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
        if (num_probed == ht->size)
            return -1;
    }

    ht->table[indx] = d;
    return indx;
}

int ht_lookup(hashtable ht, char *key) {
    uint indx       = ht_hash(key, ht.size);
    uint num_probed = 0;
    while (ht.table[indx] != NULL && strcmp(ht.table[indx]->key, key)) {
        indx++;
        num_probed++;
        if (num_probed == ht.size)
            return -1;
    }

    if (ht.table[indx] == NULL)
        return -1;

    return ht.table[indx]->val;
}

void ht_print(hashtable ht) {
    for (uint i = 0; i < ht.size; i++) {
        if (ht.table[i] == NULL)
            printf("empty\n");
        else
            printf("%s --> %d\n", ht.table[i]->key, ht.table[i]->val);
    }
}
