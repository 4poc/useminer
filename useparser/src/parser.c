#include "parser.h"

raw_article_t raw_parse_line(char *line) 
{
    char *p = line, *parts[9];
    int i;
    raw_article_t raw;
    raw.p = line;

    for(i = 0; i < 9 && p != NULL; i++) {
        p = strchr(p, '\t');
        if(p != NULL) {
            *p = '\0'; p++;
        }
        parts[i] = line;
        line = p;
    }

    /* the order and presence of each field should be queried from the server
     * via show/list/view overview.fmt */
    raw.article_num = parts[0];
    raw.subject     = parts[1];
    raw.from        = parts[2];
    raw.date        = parts[3];
    raw.message_id  = parts[4];
    raw.ref         = parts[5];
    raw.bytes       = parts[6];
    raw.lines       = parts[7];
    raw.xref        = parts[8];

    return raw;
}

bool parse_subject(char *subject, uint16_t *num, uint16_t *total)
{
    char *tmp, *open, *close, *slash;

    if((!(open = strrchr(subject, '('))) ||
       (!(close = strrchr(subject, ')'))) || 
       (!(slash = strchr(open, '/'))) ||
       slash > close || 
       open > close) {
        return false;
    }

    /* convert to 16 bit integers */
    *slash = '\0';
    *num = strtol(open+1, &tmp, 10);
    if(*tmp) {
        *slash = '/'; /* restore if invalid number */ 
        return false;
    }
    *close = '\0';
    *total = strtol(slash+1, &tmp, 10);
    if(*tmp) {
        *close = ')'; 
        return false;
    }

    /* null-byte terminate the subject string before the (num/total) part */
    if(open[-1] == ' ') {
        open[-1] = '\0';
    }
    else {
        *open = '\0';
    }

    return true;
}

