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

