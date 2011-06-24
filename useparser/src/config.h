#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef union {
    char *string;
    int *integer;
} config_value_t;

/* linked list with configuration pairs (key/value) */
struct config_pair {
    char *key;
    config_value_t *value;
    struct config_pair *next;
};
typedef struct config_pair config_pair_t;



bool config_load(const char *config_file);
void config_unload();

void new_config_pair(config_pair_t **config, config_pair_t *pair);
void free_config_pair(config_pair_t **config);

config_value_t *new_config_value_string(char *string);
config_value_t *new_config_value_integer(int integer);

char *config_string(const char *key);
int *config_integer(const char *key);

#endif /* _CONFIG_H */

