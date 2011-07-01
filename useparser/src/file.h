#ifndef _FILE_H
#define _FILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "overview.h"

struct s_segment {
    char *message_id;
    int bytes;
};
segment_init()

struct s_file {
    char *subject;
    char *from;
    uint64_t date;
    s_newsgroup *newsgroups;

    uint16_t total;
    uint16_t completed;
    struct s_segment **segments;
};

#endif /* _FILE_H */

