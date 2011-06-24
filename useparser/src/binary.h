#ifndef _BINARY_H
#define _BINARY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "overview.h"

/* part of an multipart binary message, there is no need to
 * store anything more */
typedef struct {
    char *message_id;
    uint32_t bytes;
} binary_part_t;
/* allocate new binary part, copy message id */
binary_part_t *new_binary_part(char *message_id, uint32_t bytes);
/* free memory of binary part, free message_id */
void free_binary_part(binary_part_t *part);

/* multipart binary "container" */
typedef struct {
    char *subject;
    char *from;
    uint64_t date;
    newsgroup_t *newsgroups;

    uint16_t parts_total; /* parsed from the (num/total) of the subject */
    uint16_t parts_completed;
    binary_part_t **parts; /* array of pointers to part_t structs */
} binary_t;
/* allocate new binary, parse overview information (subject must already
 * been parsed, copies memory of all strings */
binary_t *new_binary(overview_t overview, uint16_t num, uint16_t total);
void insert_binary_part(binary_t *binary, overview_t overview, uint16_t num);
/* free memory of binary container */
void free_binary(binary_t *binary);
bool complete_binary(binary_t *binary);

#endif /* _BINARY_H */

