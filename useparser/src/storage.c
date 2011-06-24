#include "storage.h"

binary_t *storage_mem = NULL;
int storage_used = 0;

void storage_init()
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


}

void storage_uninit()
{
    FREE(storage_mem);
}

void storage_new(char *hash, binary_t *binary)
{
    // storage_mem[storage_used++] = binary;

}

uint32_t hashtable_reduce(char *hash)
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

    DEBUG("mod reduced result: %d\n", hash_int32);

    return hash_int32;
}

