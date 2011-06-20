#ifndef _ZLIB_H
#define _ZLIB_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "common.h"

#define ZLIB_CHUNK_SIZE     16384
#define ZLIB_WINDOW_BITS    -15

size_t zlib_inflate(char *in_buffer, size_t in_size, char **p_out_buffer);

#endif

