#include "cache.h"

bool cache_table_init()
{
    size_t table_size = *config_integer("cache_table_size") *
       sizeof(struct s_cache_slot *); 

    INFO("allocate cache table (size: %d bytes)\n", table_size);

    cache_table = malloc(table_size);
    if(!cache_table) {
        ERROR("unable to allocate cache table!\n");
        return false;
    }
    memset(cache_table, 0, table_size);

    return true;
}

void cache_table_free()
{
    struct s_cache_slot *slot;
    for(int i = 0; i = *config_integer("cache_table_size"); i++) {
        slot = cache_table[i];
        while(slot) {
            FREE(slot->file);
            FREE(slot);
            slot = slot->next;
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

    return result;
}

void cache_table_insert(int index, char *hash, struct s_file *file)
{
    /* create a new slot */
    struct s_cache_slot *slot;
    slot = malloc(sizeof(s_cache_slot));
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

    s_cache_slot *prev_slot = slot;
    slot = slot->next; /* we've already searched this slot */
    while(slot) {
        if(memcmp(slot->hash, hash, 16) == 0) {
            prev_slot->next = slot->next; /* detach this slot */ 
            FREE(slot->file);
            FREE(slot);
            break;
        }
        
        prev_slot = slot;
        slot = slot->next;
    }
}

struct s_file *cache_table_search(int index, char *hash)
{
    s_cache_slot *slot = cache_table[index];

    while(slot) {
        if(memcmp(slot->hash, hash, 16) == 0) {
            return slot->file;
        }
        slot = slot->next;
    }

    return NULL;
}

