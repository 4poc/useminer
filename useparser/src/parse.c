/* useminer -- efficient usenet binary indexer
 * Copyright (C) 2011 Matthias -apoc- Hecker <apoc@sixserv.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "parse.h"

static char *hash = NULL;

bool parse_init()
{
    INFO("parse init\n");
    overview = malloc(sizeof(struct s_overview));
    if(!overview) {
        ERROR("unable to allocate overview\n");
        return false;
    }

    if(!cache_table_init()) {
        return false;
    }

    if(!complete_init()) {
        return false;
    }

    parse_stat_completed = 0;
    parse_stat_incomplete = 0;
    parse_stat_lines = 0;
    parse_stat_segments = 0;
    parse_stat_skip_completed = 0;

    return true;
}

void parse_uninit()
{
    INFO("parse uninit\n");
    complete_free();
    cache_table_free();

    FREE(hash);
    FREE(overview);
}

void parse_process(char *line)
{
    static char *hash_data = NULL;
    struct s_file *file;
    uint16_t num, total;
    int cache_index, complete_idx;

    parse_stat_lines++;

    /* splice line and write (global) static overview */
    parse_overview(line);

    /* parse subject line for multipart numbers (num and total) */
    if(!parse_header_subject(overview->subject, &num, &total)) {
        ERROR("article subject not valid multipart binary? (subject:%s)",
                overview->subject);
        return;
    }
    if(num > total) {
        ERROR("segment# larger than total, skip segment (%s)\n", 
                overview->subject);
        return;
    }

    DEBUG("process segment (#%d/%d) %s\n", num, total, overview->subject);

    /* calculate hash for subject and from headers,
     * (NOTE: subject is spliced from num and total by parse subject) */
    hash_data = join_string(overview->subject, overview->from);
    md5(hash_data, strlen(hash_data), &hash);
    FREE(hash_data);

    /* check hash with a list of already completed and "stored away"
     * files, this list can become very large */
    complete_idx = complete_index(hash);
    if(complete_search(complete_idx, hash)) {
        DEBUG("file hash found in complete table, skipping\n");
        parse_stat_skip_completed++;
        return;
    }

    cache_index = cache_table_index(hash);
    if((file = cache_table_search(cache_index, hash))) {
        /* file found in storage, add new segment */
        file_insert_segment(file, num, segment_new(
                    overview->message_id, 
                    atoi(overview->bytes)));
    }
    else { /* this segment is the first of this file */
        file = file_new(
                overview->subject,
                overview->from,
                parse_header_date(overview->date),
                parse_header_xref(overview->xref), /* newsgroup list */
                total);
        if(!file) {
            ERROR("error creating new file (subject:%s)\n", overview->subject);
            return;
        }
        /* insert this segment to the segment array */
        file_insert_segment(file, num, segment_new(
                    overview->message_id, 
                    atoi(overview->bytes)));
        DEBUG("new file created [%p]\n", file);
        cache_table_insert(cache_index, hash, file);
        parse_stat_incomplete++;
    }

    parse_stat_segments++;

    if(file->total == file->completed) {
        complete_insert(complete_idx, hash);
        file_free(file);
        cache_table_remove(cache_index, hash);
        parse_stat_completed++;
        parse_stat_incomplete--;
    }

    //exit(0);
}

bool parse_overview(char *line)
{
    char *p = line, *parts[9];
    int i;
    overview->p = line;

    for(i = 0; i < 9 && p != NULL; i++) {
        p = strchr(p, '\t');
        if(p != NULL) {
            *p = '\0'; p++;
        }
        parts[i] = line;
        line = p;
    }
    if(i != 9) {
        ERROR("wrong overview header field count(%d)\n", i);
        return false;
    }

    /* the order and presence of each field should be queried from the server
     * via show/list/view overview.fmt */
    overview->article_num = parts[0];
    overview->subject     = parts[1];
    overview->from        = parts[2];
    overview->date        = parts[3];
    overview->message_id  = parts[4];
    overview->ref         = parts[5];
    overview->bytes       = parts[6];
    overview->lines       = parts[7];
    overview->xref        = parts[8];

    return true;
}

bool parse_header_subject(char *subject, uint16_t *num, uint16_t *total)
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

    // DEBUG("subject:%s, {%d/%d}\n", subject, *num, *total);

    return true;
}

struct s_newsgroup *parse_header_xref(char *xref)
{
    struct s_newsgroup *newsgroup = NULL;
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
                newsgroup_insert(&newsgroup, token);
            }
            token = p+1;
        }
    }
    return newsgroup;
}

uint64_t parse_header_date(char *date)
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

