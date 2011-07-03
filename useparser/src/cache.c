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
    MP_INT mp_hash, mp_size, mp_result;
    int result;

    /* init gmp values */
    mpz_init(&mp_hash);
    mpz_import(&mp_hash, 16, 1, sizeof(char), 0, 0, hash);
    mpz_init_set_ui(&mp_size, *config_integer("cache_table_size"));
    mpz_init(&mp_result);

    /* modulo reduction */
    mpz_mod(&mp_result, &mp_hash, &mp_size);
    result = mpz_get_ui(&mp_result);

    /* free gmp values */
    mpz_clear(&mp_hash);
    mpz_clear(&mp_size);
    mpz_clear(&mp_result);

    DEBUG("cache table index[%d]\n", result);

    return result;
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

