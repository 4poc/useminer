#include "overview.h"

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

void new_newsgroup(newsgroup_t **newsgroup, char *name)
{
    //DEBUG("new_newsgroup(%p, %s)\n", *newsgroup, name);

    if(*newsgroup == NULL) {
        *newsgroup = malloc(sizeof(newsgroup_t));
        if(!*newsgroup) {
            ERROR("unable to allocate memory for newsgroup_t root.\n");
            return;
        }
        (*newsgroup)->name = copy_string(name);
        (*newsgroup)->next = NULL;
    }
    else {
        new_newsgroup(&((*newsgroup)->next), name);
    }
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

void print_newsgroup(newsgroup_t *newsgroup)
{
    if(!newsgroup) {
        return;
    }

    printf("%s ", newsgroup->name);
    print_newsgroup(newsgroup->next);
}

newsgroup_t *parse_xref(char *xref)
{
    newsgroup_t *newsgroup = NULL;
    char *p, *colon=NULL, *end, *token;

    end = xref + strlen(xref);
    token = xref + 7; /* first token after "Xref: " */

    /* split by whitespace */
    for(p = token; p <= end; p++) {
        if(*p == ' ' || p == end){
            *p = '\0';
            // token = the word without whitespace:
            if((colon = strchr(token, ':'))) {
                *colon = '\0';
                new_newsgroup(&newsgroup, token);
            }
            token = p+1;
        }
    }
    return newsgroup;
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

uint64_t parse_date(char *date)
{
    uint64_t unixtime;
    char *result, *num_result;
    char strf[21];
    bool valid = false;
    int i, j, timezone_hours;
    struct tm time;

    memset(&time, 0, sizeof(struct tm));

    /* test the most commonly used date formats in usenet */
    for(i=0; i<ARRAY_LEN(date_formats); i++) {
        result = strptime(date, date_formats[i], &time);

        /* this sucks! srsly */
        if(result && strlen(result) >= 3 && /* remaining chars, timezone? */
            result[0] == ' ') {
        
            if((result[1] == '+' || result[1] == '-') &&
                result[4] == '0' && result[5] == '0') { /* numeric timezone */

                timezone_hours = strtol(result+1, &num_result, 10);
                if(!*num_result && timezone_hours >= 100) { /* invalid hours */
                  time.tm_hour -= (timezone_hours / 100);
                }
                valid = true;
                break;

            }
            else {
                /* try to find a matching timezone name: */
                for(j=0; j<ARRAY_LEN(timezone_names); j++) {
                    if(strcmp(result+1, timezone_names[j].name) == 0) {
                        time.tm_hour -= timezone_names[j].offset + 
                                        timezone_names[j].dst;
                        valid = true;
                        break;
                    }
                }
            }
        }
        else if(result && result[0] == '\0') { /* assume the timezone is UTC?*/
            /*timezone is gmt? */
            valid = true;
            break;
        }
        else if(result) {
            // ERROR("format invalid, remaining: %s\n", result);
        }
    }
    if(valid) {
        /* convert to unix epoch */
        strftime(strf, sizeof(strf), "%s", &time);

        unixtime = strtol(strf, &num_result, 10);
        if(!*num_result) { /* valid unix timestamp */
            // DEBUG("date: %s (%02d:%02d unix: %lld)\n", date, time.tm_hour, 
            //         time.tm_min, unixtime);
            return unixtime;
        }
    }

    ERROR("unable to parse date: %s (remain:%s)\n", date, result);
    return 0;
}


