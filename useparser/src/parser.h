#ifndef _PARSER_H
#define _PARSER_H

#include <stdlib.h>
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

#endif

