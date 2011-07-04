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
};
/* append newsgroup to the end of newsgroups-list, copies name */
void newsgroup_insert(
        struct s_newsgroup **newsgroup_list,
        char *name);
/* free memory of the newsgroups-list */
void newsgroup_free(struct s_newsgroup *newsgroup_list);
/* search within newsgroups-list list for name */
bool newsgroup_search(struct s_newsgroup *newsgroup_list, char *name);


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
    struct s_newsgroup *newsgroups;

    uint16_t total;
    uint16_t completed;
    struct s_segment **segments;
};
/* create and return a new file */
struct s_file *file_new(
        char *subject, 
        char *from, 
        uint64_t date, 
        struct s_newsgroup *newsgroups, 
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

