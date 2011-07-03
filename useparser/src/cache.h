/* Incomplete files (with missing segments) are cached in memory 
 * and disk, in memory their organized as a hashtable with the 
 * subject/from hash as keys. When the memory consumption gets to
 * large or the useparser is quiting, cache entries are moved to 
 * disk. Completed files are removed from the cache.  */
#ifndef _CACHE_H
#define _CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <gmp.h>

#include "common.h"
#include "config.h"
#include "file.h"

/* simple hashtable implementation for incomplete memory cache */
struct s_cache_slot {
    char hash[16]; /* binary md5 digest */
    struct s_file *file;
    struct s_cache_slot *next; /* may used for table collisions */
}; /* this should be considered to be an internal structure */

/* global memory table, initialized by cache_table_init() */
static struct s_cache_slot **cache_table;

/* allocate memory cache hashtable */
bool cache_table_init();
/* free cache_table and including file ptrs */
void cache_table_free();

/* calculate hashtable index for md5 hash, you need to
 * precalculate this index for the following functions */
int cache_table_index(char *hash);

/* insert file ptr to a specific location */
void cache_table_insert(int index, char *hash, struct s_file *file);
/* remove a cache slot from the cache table */
void cache_table_remove(int index, char *hash);
/* search cached file entry within the cache table */
struct s_file *cache_table_search(int index, char *hash);

#endif /* _CACHE_H */

