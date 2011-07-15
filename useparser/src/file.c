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
#include "file.h"

void newsgroup_insert(
        struct s_newsgroup **newsgroup_list,
        char *name)
{
    while(*newsgroup_list) { /* skip to the end */
        newsgroup_list = &((*newsgroup_list)->next);
    }

    /* create new newsgroup */
    *newsgroup_list = malloc(sizeof(struct s_newsgroup));
    if(!*newsgroup_list) {
        ERROR("unable to allocate memory for s_newsgroup\n");
        return;
    }
    (*newsgroup_list)->name = copy_string(name);
    (*newsgroup_list)->next = NULL;
}

void newsgroup_free(struct s_newsgroup *newsgroup_list)
{
    struct s_newsgroup *newsgroup;
    DEBUG("free newsgroup list\n");
    while(newsgroup_list) {
        newsgroup = newsgroup_list;
        newsgroup_list = newsgroup_list->next;
        FREE(newsgroup->name);
        FREE(newsgroup);
    }
}

bool newsgroup_search(struct s_newsgroup *newsgroup_list, char *name)
{
    while(newsgroup_list) {
        if(strcmp(newsgroup_list->name, name) == 0) {
            return true;
        }
        newsgroup_list = newsgroup_list->next;
    }
    return false;
}

struct s_segment *segment_new(char *message_id, int bytes)
{
    struct s_segment *segment;
    segment = malloc(sizeof(struct s_segment));
    if(!segment) {
        ERROR("unable to allocate memory file segment");
        return NULL;
    }
    segment->message_id = copy_string(message_id);
    segment->bytes = bytes;
    return segment;
}

void segment_free(struct s_segment *segment)
{
    DEBUG("free segment\n");
    if(segment && segment->message_id) {
        FREE(segment->message_id);
        FREE(segment);
    }
}

struct s_file *file_new(
        char *subject, 
        char *from, 
        uint64_t date, 
        struct s_newsgroup *newsgroups, 
        uint16_t total)
{
    struct s_file *file;

    DEBUG("new file object\n");
    
    file = malloc(sizeof(struct s_file));
    if(!file) {
        ERROR("unable to allocate memory for file");
        return NULL;
    }

    file->subject = copy_string(subject);
    file->from = copy_string(from);
    file->date = date;
    file->newsgroups = newsgroups;

    file->completed = 0;
    file->total = total;
    /* allocate for all segments */
    file->segments = malloc(sizeof(struct s_segment *) * total);
    if(!file->segments) {
        ERROR("unable to allocate memory for file segments");
        return NULL;
    }
    memset(file->segments, 0, sizeof(struct s_segment *) * total);

    return file;
}

void file_free(struct s_file *file)
{
    DEBUG("free file object memory [%p]\n", file);
    if(!file) {
        return;
    }

    FREE(file->subject);
    FREE(file->from);

    newsgroup_free(file->newsgroups);

    for(int i = 0; i < file->total; i++) {
        if(file->segments[i]) {
            segment_free(file->segments[i]);
        }
    }
    DEBUG("free file segment array\n");
    FREE(file->segments);
    DEBUG("free file itself\n");
    FREE(file);
}

void file_insert_segment(
        struct s_file *file, 
        uint16_t num, 
        struct s_segment *segment)
{
    DEBUG("file insert segment #%d/%d\n", num, file->total);
    if(num > file->total) {
        ERROR("invalid file segment, number > total!\n");
        return;
    }

    if(file->segments[num-1]) {
        DEBUG("warning: overwrite file segment #%d/%d (subject: %s)\n", 
                num, file->total, file->subject);
        segment_free(file->segments[num-1]);
    }

    file->segments[num-1] = segment;
    file->completed++;
}

