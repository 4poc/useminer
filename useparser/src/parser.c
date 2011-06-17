#include "parser.h"

overview_t parse_overview(char *line)
{
    char *p = line, *parts[9];
    int i;
    overview_t overview;
    overview.p = line;

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
    overview.article_num = parts[0];
    overview.subject     = parts[1];
    overview.from        = parts[2];
    overview.date        = parts[3];
    overview.message_id  = parts[4];
    overview.ref         = parts[5];
    overview.bytes       = parts[6];
    overview.lines       = parts[7];
    overview.xref        = parts[8];

    return overview;
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

binary_t *new_binary(overview_t overview, uint16_t num, uint16_t total, char *newsgroup)
{
    binary_t *binary;

    binary = malloc(sizeof(binary_t));
    if(!binary) {
        ERROR("unable to allocate memory for binary_t");
        return NULL;
    }

    binary->subject = copy_string(overview.subject);
    binary->from = copy_string(overview.from);

    // parse date
    // ...

    binary->newsgroups = copy_string(newsgroup);
    /* parse xref for other newsgroups, and append */

    binary->parts_total = total;
    /* allocate for all parts */
    binary->parts = malloc(sizeof(binary_part_t*) * total);
    DEBUG("memory allocation for ->parts, %d\n", total * sizeof(binary_part_t*));
    if(!binary->parts) {
        ERROR("unable to allocate memory for binary_t.parts");
        return NULL;
    }
    memset(binary->parts, 0, sizeof(binary_part_t*) * total);
    if(num > 1 && num <= total) {
        binary->parts[num-1] = new_binary_part(overview.message_id, atoi(overview.bytes));
    }

    return binary;
}

void free_binary(binary_t *binary)
{
    if(binary == NULL) {
        return;
    }

    FREE(binary->subject);
    FREE(binary->from);
    FREE(binary->newsgroups);

    DEBUG("free binary->parts\n");
    for(int i=0; i<binary->parts_total;i++){
        if(binary->parts[i] != NULL) {
            printf("\n\nparts[%d]->message_id: %s\n\n", i, binary->parts[i]->message_id);
        }
        free_binary_part(binary->parts[i]);
    }
    FREE(binary->parts);

    FREE(binary);
}

binary_part_t *new_binary_part(char *message_id, uint32_t bytes)
{
    binary_part_t *binary_part;
    binary_part = malloc(sizeof(binary_part));
    if(!binary_part) {
        ERROR("unable to allocate memory for binary_part_t");
        return NULL;
    }
    binary_part->message_id = copy_string(message_id);
    binary_part->bytes = bytes;
    return binary_part;
}

void free_binary_part(binary_part_t *part)
{
    if(!part) {
        return;
    }
    DEBUG("free message id: %s\n", part->message_id);
    FREE(part->message_id);
    FREE(part);
}

