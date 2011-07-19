/* useminer -- efficient usenet binary indexer
 * Copyright (C) 2011 Matthias -apoc- Hecker <apoc@sixserv.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include <sys/stat.h>
#include <sys/types.h>

#include <tpl.h>

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

int cache_stat_slots;
 
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

/* dump cache_table to disk */
void cache_table_dump();
/* restore cache_table to disk */
void cache_table_restore();

/* hashtable with md5 hashes for all completed files,
 * stored on disk for persistance */
struct s_complete_slot {
    char hash[16];
    struct s_complete_slot *next;
};
static struct s_complete_slot **complete_table;
/* init table, read complete file */
bool complete_init();
/* free table memory, write to complete file */
void complete_free();
int complete_index(char *hash);
bool complete_search(int index, char *hash);
void complete_insert(int index, char *hash);

#endif /* _CACHE_H */

