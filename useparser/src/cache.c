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
#include "cache.h"

bool cache_table_init()
{
    size_t table_size = *config_integer("cache_table_size") *
       sizeof(struct s_cache_slot *); 

    INFO("init memory cache (table size: %d bytes)\n", table_size);

    cache_table = malloc(table_size);
    if(!cache_table) {
        ERROR("unable to allocate cache table!\n");
        return false;
    }
    memset(cache_table, 0, table_size);

    cache_stat_slots = 0;

    return true;
}

void cache_table_free()
{
    INFO("free cache memory\n");
    struct s_cache_slot *slot, *next_slot;
    for(int i = 0; i < *config_integer("cache_table_size"); i++) {
        next_slot = cache_table[i];
        while(next_slot) {
            slot = next_slot;
            next_slot = slot->next;
            FREE(slot->file);
            FREE(slot);
        }
    }
    FREE(cache_table);
}

int cache_table_index(char *hash)
{
    return md5mod(hash, *config_integer("cache_table_size"));
}

void cache_table_insert(int index, char *hash, struct s_file *file)
{
    DEBUG("cache memory insert file [#%d]\n", index);

    cache_stat_slots++;

    /* create a new slot */
    struct s_cache_slot *slot;
    slot = malloc(sizeof(struct s_cache_slot));
    if(!slot) {
        ERROR("error allocating memory for cache slot!\n");
        return;
    }

    /* set slot contents for provided hash and file */
    memcpy(&(slot->hash), hash, 16);
    slot->file = file;
    slot->next = NULL;

    /* append slot at the cache table row specified by index */
    struct s_cache_slot **prev_slot = &(cache_table[index]);
    while(*prev_slot) {
        prev_slot = &((*prev_slot)->next);
    }
    *prev_slot = slot;
}

void cache_table_remove(int index, char *hash)
{
    struct s_cache_slot *slot = cache_table[index];

    cache_stat_slots--;

    DEBUG("cache table remove [#%d]\n", index);

    if(!slot) {
        return;
    }
                                              
    if(!(slot->next)) { /* just the one slot in this table row */
        FREE(slot);
        cache_table[index] = NULL;
        return;
    }

    /* check if the first slot in row is the searched one */
    if(memcmp(slot->hash, hash, 16) == 0) {
        cache_table[index] = slot->next; /* detach */
        FREE(slot);
        return;
    }

    struct s_cache_slot *prev_slot = slot;
    slot = slot->next; /* we've already searched this slot */
    while(slot) {
        if(memcmp(slot->hash, hash, 16) == 0) {
            prev_slot->next = slot->next; /* detach this slot */ 
            FREE(slot);
            break;
        }
        
        prev_slot = slot;
        slot = slot->next;
    }
}

struct s_file *cache_table_search(int index, char *hash)
{
    struct s_cache_slot *slot = cache_table[index];

    while(slot) {
        if(memcmp(slot->hash, hash, 16) == 0) {
            return slot->file;
        }
        slot = slot->next;
    }

    return NULL;
}

bool complete_init()
{
    size_t table_size = *config_integer("complete_size") *
       sizeof(struct s_complete_slot *); 

    INFO("init memory for complete table (%s)\n", hsize(table_size).str);

    complete_table = malloc(table_size);
    if(!complete_table) {
        ERROR("unable to allocate complete table!\n");
        return false;
    }
    memset(complete_table, 0, table_size);

    FILE *fd = fopen(config_string("complete_file"), "r");
    if(fd) {
        INFO("read complete table from file: %s (%s)\n",
                config_string("complete_file"),
                hsize(fdsize(fd)).str);

        char hash[16];
        int index;
        while(feof(fd) == 0) {
            if((fread(&hash, sizeof(char), 16, fd) != 16) && ferror(fd) != 0) {
                ERROR("complete file corrupted?\n");
                return false;
            }
            index = complete_index(hash);
            if(complete_search(index, hash)) {
                ERROR("duplicate hashes in complete file!\n");
            }
            else {
                complete_insert(index, hash);
            }
        }

        fclose(fd);
    }                               

    return true;
}

void complete_free()
{
    INFO("write complete table to: %s\n", config_string("complete_file"));

    int written = 0;
    struct s_complete_slot *slot, *next_slot;
    FILE *fd = fopen(config_string("complete_file"), "w");
    if(!fd) {
        ERROR("unable to write to complete file.\n");
        return;
    }
    for(int i = 0; i < *config_integer("complete_size"); i++) {
        next_slot = complete_table[i];
        while(next_slot) {
            slot = next_slot;
            next_slot = slot->next;

            fwrite(slot->hash, 1, 16, fd);
            written++;

            FREE(slot);
        }
    }
    fclose(fd);
    FREE(complete_table);
    INFO("written %d hashes to complete table file\n", written);
}

int complete_index(char *hash)
{
    return md5mod(hash, *config_integer("complete_size"));
}

bool complete_search(int index, char *hash)
{
    struct s_complete_slot *slot = complete_table[index];

    while(slot) {
        if(memcmp(slot->hash, hash, 16) == 0) {
            return true;
        }
        slot = slot->next;
    }

    return false;
}

void complete_insert(int index, char *hash)
{
    DEBUG("complete table insert hash %s\n", md5hex(hash).str);

    /* create a new slot */
    struct s_complete_slot *slot;
    slot = malloc(sizeof(struct s_complete_slot));
    if(!slot) {
        ERROR("error allocating memory for complete slot!\n");
        return;
    }

    /* set slot hash */
    memcpy(&(slot->hash), hash, 16);
    slot->next = NULL;

    /* append slot at the complete table row specified by index */
    struct s_complete_slot **prev_slot = &(complete_table[index]);
    while(*prev_slot) {
        prev_slot = &((*prev_slot)->next);
    }
    *prev_slot = slot;
}

