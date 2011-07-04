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

