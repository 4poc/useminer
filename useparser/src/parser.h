#ifndef _PARSER_H
#define _PARSER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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
} raw_article_t; 

/**
 * Splice line string by TAB, arrange each header part.
 * TODO: order according to overview.fmt
 */
raw_article_t raw_parse_line(char *line);

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

#endif

