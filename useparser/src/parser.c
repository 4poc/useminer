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
    binary->date = parse_date(overview.date);

    /* parse xref for other newsgroups, and append */
    if(overview.xref) {
        binary->newsgroups = parse_xref(overview.xref);
    }
    if(!search_newsgroup(binary->newsgroups, newsgroup)) {
        new_newsgroup(newsgroup, binary->newsgroups);
    }

    binary->parts_total = total;
    /* allocate for all parts */
    binary->parts = malloc(sizeof(binary_part_t*) * total);
    //DEBUG("memory allocation for ->parts, %d\n", total * sizeof(binary_part_t*));
    if(!binary->parts) {
        ERROR("unable to allocate memory for binary_t.parts");
        return NULL;
    }
    memset(binary->parts, 0, sizeof(binary_part_t*) * total);
    if(num > 1 && num <= total) { /* this would be an invalid article otherwise */
        binary->parts[num-1] = new_binary_part(overview.message_id, atoi(overview.bytes));
    }
    // else { free binary; return NULL; }

    return binary;
}

void free_binary(binary_t *binary)
{
    if(binary == NULL) {
        return;
    }

    FREE(binary->subject);
    FREE(binary->from);

    free_newsgroup(binary->newsgroups);

    for(int i=0; i<binary->parts_total;i++){
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
    FREE(part->message_id);
    FREE(part);
}

void new_newsgroup(char *name, newsgroup_t *newsgroup)
{
    if(!newsgroup) {
        newsgroup = malloc(sizeof(newsgroup_t));
        if(!newsgroup) {
            ERROR("unable to allocate memory for newsgroup_t root.\n");
            return;
        }
        newsgroup->name = copy_string(name);
        newsgroup->next = NULL;
        return;
    }
    new_newsgroup(name, newsgroup->next);
}

void free_newsgroup(newsgroup_t *newsgroup)
{
    if(!newsgroup) {
        return;
    }
    if(newsgroup->next) {
        free_newsgroup(newsgroup->next);
    }

    FREE(newsgroup->name);
    FREE(newsgroup);
}

bool search_newsgroup(newsgroup_t *newsgroup, char *name)
{
    /* search for newsgroup within list */
    if(!newsgroup) {
        return false;
    }
    else if(strcmp(newsgroup->name, name) == 0) {
        return true;
    }
    else {
        return search_newsgroup(newsgroup->next, name);
    }
}

newsgroup_t *parse_xref(char *xref)
{
    newsgroup_t *newsgroup = NULL;
    char *p, *colon=NULL, *end, *token;
    p = xref;
    end = p + strlen(xref); // points to null-byte
    p+=7; /* + "Xref: " */
    token = p;

    /* split by whitespace */
    for(p; p < end; p++) {
        if(*p == ' '){
            *p = '\0';
            // token = the word without whitespace:
            if((colon = strchr(token, ':'))) {
                *colon = '\0';
                new_newsgroup(token, newsgroup);
            }
            token = p+1;
        }
    }

    return newsgroup;
}

uint64_t parse_date(char *date)
{
    uint8_t timezone_hours=0;
    uint64_t unixtime;
    char *result, *num_result;
    char strf[21];
    bool valid = false;
    int i;
    struct tm time;

    memset(&time, 0, sizeof(struct tm));

    for(i=0; i<ARRAY_LEN(date_formats); i++) {
        result = strptime(date, date_formats[i], &time);

        /* this sucks! srsly */
        if(result && strlen(result) >= 3 && /* remaining (unable to parse) timezone */
            result[0] == ' ') {
        
            if(result[1] == '+' || result[1] == '-') { /* numeric timezone */

                timezone_hours = strtol(result+2, &num_result, 10);
                if(*num_result || timezone_hours < 100) { /* invalid hours */
                }
                else {
                  if(result[1] == '+') {
                      time.tm_hour += (timezone_hours / 100);
                  }
                  else {
                      time.tm_hour -= (timezone_hours / 100);
                  }
                }
                valid = true;
                break;

            }
            else { /* TODO: do this the proper(?) way.. */
                if(strcmp(result+1, "GMT") == 0) {
                    /* do nothing, +/- 0 offset */
                    valid=true;
                    break;
                }
            }
        }
        else if(result && result[0] == '\0') { /* assume the timezone is UTC?*/
            /*timezone is gmt? */
            valid = true;
            break;
        }
        else if(result) {
            ERROR("format invalid, remaining: %s\n", result);
        }
    }
    if(valid) {
        /* convert to unix epoch */
        strftime(&strf, sizeof(strf), "%s", &time);

        unixtime = strtol(strf, &num_result, 10);
        if(!*num_result) { /* valid unix timestamp */
            return unixtime;
        }
    }

    ERROR("unable to parse date: %s (remain:%s)\n", date, result);
    return 0;
}

