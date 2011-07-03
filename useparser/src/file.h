#ifndef _FILE_H
#define _FILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "common.h"

/* linear list of newsgroups */
struct s_newsgroup {
    char *name;
    struct s_newsgroup *next;
}
/* append newsgroup to the end of newsgroups-list, copies name */
void newsgroup_insert(
        struct s_newsgroup **newsgroups,
        char *name);
/* free memory of the newsgroups-list */
void newsgroup_free(struct s_newsgroup *newsgroups);
/* search within newsgroups-list list for name */
bool newsgroup_search(struct s_newsgroup *newsgroups, char *name);


/* file segment (a single usenet article/message) */
struct s_segment {
    char *message_id;
    int bytes;
};
/* create new segment with copied message id and return */
struct s_segment *segment_new(char *message_id, int bytes);
/* free segment memory */
void segment_free(struct s_segment *segment);


/* file consists of multiple segments/ usenet articles/messages */
struct s_file {
    char *subject;
    char *from;
    uint64_t date;
    s_newsgroup *newsgroups;

    uint16_t total;
    uint16_t completed;
    struct s_segment **segments;
};
/* create and return a new file */
struct s_file *file_new(
        char *subject, 
        char *from, 
        uint64_t date, 
        s_newsgroup *newsgroups, 
        uint16_t total);
/* free memory of file, including newsgroup and segments lists */
void file_free(struct s_file *file);
/* insert provided segment within file, overwrites any existing
 * segment */ 
void file_insert_segment(
        struct s_file *file, 
        uint16_t num, 
        struct s_segment *segment);

#endif /* _FILE_H */

