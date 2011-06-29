#ifndef _CACHE_H
#define _CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <gmp.h>

#include "common.h"
#include "file.h"
#include "config.h"

/* Incomplete files (with missing segments) are cached in memory 
 * and disk, in memory their organized as a hashtable with the 
 * subject/from hash as keys. When the memory consumption gets to
 * large or the useparser is quiting, old incomplete cache entries 
 * are moved to disk.
 * Completed files are removed from memory and disk caches.
 */

/* hashtable slot stores the key */
struct s_cache_slot {
    char hash[16]; /* binary md5 digest */
    struct s_file *file;
    struct s_cache_slot *next; /* may used for table collisions */
};

struct s_cache_slot **cache_table;

void cache_table_init();
void cache_table_free();
void cache_table_insert();
void cache_table_remove();
struct s_cache_slot *cache_table_search();
int cache_table_index();


