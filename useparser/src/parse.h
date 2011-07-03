#ifndef _PARSE_H
#define _PARSE_H

#include "common.h"

#include "file.h"
#include "cache.h"

bool parse_init();
void parse_uninit();
void parse_process(char *line);

/* raw pointers to overview headers, points to the same memory area */
struct s_overview {
    char *p; /* start of line */
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
}; 
static struct s_overview *overview;
/* parse line into overview struct, FIXME: dynamic order (OVERVIEW.FMT) */
bool parse_overview(char *line);

/* parse subject line of a multipart binary for number and total.
 * search for something like: /[ ]?\((\d+)\/(\d+)\)$/
 * example subject: Stuff - [13/123] - "stuff.part013.rar" yEnc (12/31)
 * (num = 12, total 31) */
bool parse_header_subject(char *subject, uint16_t *num, uint16_t *total);

/* parse xref header for newsgroups, search for something like: 
 * / ([a-zA-Z0-9\.]+):[0-9]+/g */
struct s_newsgroup *parse_header_xref(char *xref);

/* usenet date format mess, took some of the date formats from here: 
 * http://kitenet.net/~joey/blog/entry/date_formats_of_a_decade_of_usenet/ */
static const char *date_formats[] = {
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
/* timezones and offsets, copied from the git project date.c, git is
 * licensed GPL */
static const struct {
    const char *name;
    int offset;
    int dst;
} timezone_names[] = {
    { "IDLW", -12, 0, },    /* International Date Line West */
    { "NT",   -11, 0, },    /* Nome */
    { "CAT",  -10, 0, },    /* Central Alaska */
    { "HST",  -10, 0, },    /* Hawaii Standard */
    { "HDT",  -10, 1, },    /* Hawaii Daylight */
    { "YST",   -9, 0, },    /* Yukon Standard */
    { "YDT",   -9, 1, },    /* Yukon Daylight */
    { "PST",   -8, 0, },    /* Pacific Standard */
    { "PDT",   -8, 1, },    /* Pacific Daylight */
    { "MST",   -7, 0, },    /* Mountain Standard */
    { "MDT",   -7, 1, },    /* Mountain Daylight */
    { "CST",   -6, 0, },    /* Central Standard */
    { "CDT",   -6, 1, },    /* Central Daylight */
    { "EST",   -5, 0, },    /* Eastern Standard */
    { "EDT",   -5, 1, },    /* Eastern Daylight */
    { "AST",   -3, 0, },    /* Atlantic Standard */
    { "ADT",   -3, 1, },    /* Atlantic Daylight */
    { "WAT",   -1, 0, },    /* West Africa */

    { "GMT",    0, 0, },    /* Greenwich Mean */
    { "UTC",    0, 0, },    /* Universal (Coordinated) */
    { "Z",      0, 0, },    /* Zulu, alias for UTC */

    { "WET",    0, 0, },    /* Western European */
    { "BST",    0, 1, },    /* British Summer */
    { "CET",   +1, 0, },    /* Central European */
    { "MET",   +1, 0, },    /* Middle European */
    { "MEWT",  +1, 0, },    /* Middle European Winter */
    { "MEST",  +1, 1, },    /* Middle European Summer */
    { "CEST",  +1, 1, },    /* Central European Summer */
    { "MESZ",  +1, 1, },    /* Middle European Summer */
    { "FWT",   +1, 0, },    /* French Winter */
    { "FST",   +1, 1, },    /* French Summer */
    { "EET",   +2, 0, },    /* Eastern Europe, USSR Zone 1 */
    { "EEST",  +2, 1, },    /* Eastern European Daylight */
    { "WAST",  +7, 0, },    /* West Australian Standard */
    { "WADT",  +7, 1, },    /* West Australian Daylight */
    { "CCT",   +8, 0, },    /* China Coast, USSR Zone 7 */
    { "JST",   +9, 0, },    /* Japan Standard, USSR Zone 8 */
    { "EAST", +10, 0, },    /* Eastern Australian Standard */
    { "EADT", +10, 1, },    /* Eastern Australian Daylight */
    { "GST",  +10, 0, },    /* Guam Standard, USSR Zone 9 */
    { "NZT",  +12, 0, },    /* New Zealand */
    { "NZST", +12, 0, },    /* New Zealand Standard */
    { "NZDT", +12, 1, },    /* New Zealand Daylight */
    { "IDLE", +12, 0, },    /* International Date Line East */
};
/* convert date header string to unix timestamp (UTC) */
uint64_t parse_header_date(char *date);

#endif /* _PARSE_H */
 
