#ifndef _FILE_H
#define _FILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "common.h"

struct s_newsgroup {
    char *name;
    struct s_newsgroup *next;
}

/* append newsgroup */
void newsgroup_insert(
        struct s_newsgroup **newsgroups,
        char *name);
void newsgroup_free(struct s_newsgroup *newsgroups);
/* search within newsgroups linked list for name newsgroup,
 * return true if found */
bool newsgroup_search(struct s_newsgroup *newsgroups, char *name);

struct s_segment {
    char *message_id;
    int bytes;
};

/* create new segment with copied message id and return */
struct s_segment *segment_new(char *message_id, int bytes);

/* free segment memory */
void segment_free(struct s_segment *segment);

struct s_file {
    char *subject;
    char *from;
    uint64_t date;
    s_newsgroup *newsgroups;

    uint16_t total;
    uint16_t completed;
    struct s_segment **segments;
};

struct s_file *file_new(
        char *subject, 
        char *from, 
        uint64_t date, 
        s_newsgroup *newsgroups, 
        char *message_id, 
        int bytes, 
        uint16_t num, 
        uint16_t total);

void file_free(struct s_file *file);

void file_insert_segment(
        struct s_file *file, 
        uint16_t num, 
        struct s_segment *segment);


#endif /* _FILE_H */

