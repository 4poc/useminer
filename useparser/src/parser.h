#ifndef _PARSER_H
#define _PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "common.h"

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
 * Linear Linked List for storing the newsgroups a
 * multipart binary is posted in.
 */
struct newsgroup_s {
    char *name;
    struct newsgroup_s *next; 
};
typedef struct newsgroup_s newsgroup_t;

/**
 * Goto the last (uninitialized) newsgroup list element,
 * allocate new newsgroup element with a name copy.
 */
void new_newsgroup(char *name, newsgroup_t *newsgroup);

/**
 * Free all memory of the provided newsgroup and all of
 * its childs.
 */
void free_newsgroup(newsgroup_t *newsgroup);

/**
 * Search for a specified name in the provided newsgroup list.
 */
bool search_newsgroup(newsgroup_t *newsgroup, char *name);

/**
 * Parse the Xref: header into newsgroup_t structure
 * 
 * / ([a-zA-Z0-9\.]+):[0-9]+/g
 */
newsgroup_t *parse_xref(char *xref);

typedef struct {
    char *message_id;
    uint32_t bytes;
} binary_part_t;

binary_part_t *new_binary_part(char *message_id, uint32_t bytes);

void free_binary_part(binary_part_t *part);

typedef struct {
    char *subject;
    char *from;
    uint64_t date;

    newsgroup_t *newsgroups;

    uint16_t parts_total; /* parsed from the (num/total) of the subject */
    binary_part_t **parts; /* array of pointers to part_t structs */
} binary_t;

/**
 * Allocate new structures for binary_t
 */
binary_t *new_binary(overview_t overview, uint16_t num, uint16_t total, char *newsgroup);

void free_binary(binary_t *binary);



/**
 * Date parsing stuff is messy
 * http://kitenet.net/~joey/blog/entry/date_formats_of_a_decade_of_usenet/
 */
static const char *usenet_date_format[] = {
    "%d %b %y %T",       
    "%a, %d %b %y %T",   
    "%a, %d-%b-%y %T",   
    "%d %b %y %T",       
    "%d %b %y %H:%M",    
    "%a, %d %b %y %H:%M",
    "%a, %d %b %Y %T",   
    "%a, %d %b %Y %H:%M",
    "%d %b %Y %T",       
    "%d-%b-%y %H:%M",    
    "%d %b %y, %T",      
    "%d %b %Y %H:%M",    
    "%a, %d %b T  %T"   
};
uint64_t parse_date(char *date);

#endif

