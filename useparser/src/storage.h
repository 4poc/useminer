#ifndef _STORAGE_H
#define _STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <gmp.h>

#include "common.h"
#include "binary.h"
#include "config.h"

/* the hashtable maps the md5 hash (of subject + from headers) to pointers
 * of binary_t structures.
 * hashing is done by modulo reduction of the hash itself,
 * collisions within the hashtable are resolved by seperate chaining */
struct hashtable_row {
    char hash[16];
    binary_t *binary;
    struct hashtable_row *next; /* only used if calculated hash collides */
};
typedef struct hashtable_row hashtable_row_t;

hashtable_row_t **hashtable;

/* calculate the modulo reduction of hash, using the hashtable size.
 * this method is using the gmp arbitary precision library to calculate
 * the modulo remainder of the 128bit "integer" md5 */ 
uint32_t hashtable_index(char *hash);


void hashtable_new(hashtable_row_t **row, char *hash, binary_t *binary);
hashtable_row_t *hashtable_search(uint32_t index, char *hash);


bool storage_init();
void storage_uninit();

void storage_new(char *hash, binary_t *binary);


#endif /* _STORAGE_H */

