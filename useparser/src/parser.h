#ifndef _PARSER_H
#define _PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"

struct newsgroup {
    char *name;
    struct newsgroup *next; 
};
typedef struct newsgroup newsgroup_t;

typedef struct {
    char *message_id;
    uint32_t bytes;
} binary_part_t;

typedef struct {
    char *subject;
    char *from;
    uint64_t date;

    newsgroup_t *newsgroups;

    uint16_t parts_total; /* parsed from the (num/total) of the subject */
    binary_part_t **parts; /* array of pointers to part_t structs */
} binary_t;

typedef struct {
    char *p; /* raw pointer to the start of the article header line,
                each part of the line is null-byte terminated */

    char *article_num;

    /* article header parts */
    char *subject;      /* Subject: */
    char *from;         /* From: */
    char *date;         /* Date: */
    char *message_id;   /* Message-ID: */
    char *ref;          /* References: */
    char *bytes;        /* Bytes: */
    char *lines;        /* Lines: */
    char *xref;         /* Xref:full */
} overview_t; 


/**
 * Parse the overview information from the provided line.
 * FIXME: dynamic order and presence of fields (server specific)
 */
overview_t parse_overview(char *line);

/**
 * Parse/splice subject line for yEnc multipart file number and total.
 *
 * This function searches for the following pattern: /[ ]?\((\d+)\/(\d+)\)$/ the
 * first and last match groups are stored within the num and total pointers.
 * The matched pattern is removed/truncated from the subject line.
 * Example:
 *
 * Subject: Stuff - [13/123] - "stuff.part013.rar" yEnc (12/31)
 *
 * Results in:
 * Arguments: *subject = "Stuff - [13/123] - "stuff.part013.rar" yEnc"
 *            *num = 12; *total = 31;
 * Returns: true
 */
bool parse_subject(char *subject, uint16_t *num, uint16_t *total);

/**
 * Allocate new structures for binary_t
 */
binary_t *new_binary(overview_t overview, uint16_t num, uint16_t total, char *newsgroup);
void free_binary(binary_t *binary);

binary_part_t *new_binary_part(char *message_id, uint32_t bytes);
void free_binary_part(binary_part_t *part);

void new_newsgroup(char *name, newsgroup_t *newsgroup);
void free_newsgroup(newsgroup_t *newsgroup);
bool search_newsgroup(newsgroup_t *newsgroup, char *name);
newsgroup_t *parse_xref(char *xref);

#endif

