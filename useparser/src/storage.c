#include "storage.h"

binary_t *storage_mem = NULL;
int storage_used = 0;

int test_collisions = 0;
int test_rows = 0;

bool storage_init()
{
    /*
    INFO("initialize storage (memory storage: %d binaries/%d bytes)\n",
            *config_integer("storage_memory"), 
            *config_integer("storage_memory") * sizeof(binary_t));
    storage_mem = malloc(*config_integer("storage_memory") * sizeof(binary_t));
    if(!storage_mem) {
        ERROR("unable to allocate memory storage!\n");
    }
    */

    size_t hashtable_size = *config_integer("storage_memory_hashtable") *
       sizeof(hashtable_row_t *); 

    INFO("allocate hashtable (size: %d bytes)\n", hashtable_size);

    hashtable = malloc(hashtable_size);
    if(!hashtable) {
        ERROR("unable to allocate memory for hashtable!\n");
        return false;
    }
    memset(hashtable, 0, hashtable_size);

    return true;
}

void storage_uninit()
{
    FREE(hashtable);
}

void storage_new(char *hash, binary_t *binary)
{
    uint32_t index = hashtable_index(hash);
    if(!hashtable_search(index, hash)) {
        DEBUG("store new: %s\n", binary->subject);
        hashtable_new(&hashtable[index], hash, binary);
        DEBUG("test rows: %d collisions: %d\n", test_rows, test_collisions);
    }
    else {
//        DEBUG("skip existing: %s\n", binary->subject);
    }


    // storage_mem[storage_used++] = binary;
    //
    //    md5print(hash);
    //    printf("(reduced hash: %d)\n\n", hashtable_index(hash));
}

uint32_t hashtable_index(char *hash)
{
    MP_INT hash_int128, mod_reduce, result;
    uint32_t hash_int32;

    mpz_init(&result);
    mpz_init(&hash_int128);
    mpz_import(&hash_int128, 16, 1, sizeof(char), 0, 0, hash);

    mpz_init_set_ui(&mod_reduce, *config_integer("storage_memory_hashtable"));

    /* modulo reduction */
    mpz_mod(&result, &hash_int128, &mod_reduce);
    hash_int32 = mpz_get_ui(&result);

    mpz_clear(&mod_reduce);
    mpz_clear(&hash_int128);
    mpz_clear(&result);

    // DEBUG("mod reduced result: %d\n", hash_int32);

    return hash_int32;
}

void hashtable_new(hashtable_row_t **row, char *hash, binary_t *binary)
{
    // printf("hashtable_new(%p)\n", *row);
    if(!*row) {
        *row = malloc(sizeof(hashtable_row_t));
        if(!*row) {
            ERROR("error allocating memory for hashtable row!\n");
            return;
        }
        memcpy(&((*row)->hash), hash, 16);
        (*row)->binary = binary;
        (*row)->next = NULL;
        test_rows++;
    }
    else {
        hashtable_new(&((*row)->next), hash, binary);
        test_collisions++;
    }
}

hashtable_row_t *hashtable_search(uint32_t index, char *hash)
{
    hashtable_row_t *row = hashtable[index];

    while(row) {
        if(memcmp(row->hash, hash, 16) == 0) {
            return row;
        }

        row = row->next;
    }

    return NULL;
}

