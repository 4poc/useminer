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
#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h> /* includes <stdint.h> */
#include <string.h>
#include <sys/stat.h>

#include <gmp.h>


#include "md5.h"

#define VERSION         "0.0.1"
/* enable log level error, debug info: */
#define ENABLE_ERROR
// #define ENABLE_DEBUG
#define ENABLE_INFO
/* ideally the chunk size should be roughly the same as the 
 * received message chunks (the range of articles of XZVER */
#define FILE_CHUNK_SIZE 1024 * 1024 * 1  /* fread chunks */
/* #define YENC_CHECK_SIZE             yenc decoding tests for correct size */
#define YENC_CHECK_CRC              /* yenc decoding tests for correct CRC */


#define FREE(ptr)       if(ptr != NULL) {   \
                            free(ptr);      \
                            ptr = NULL;     \
                        } else {}

#define ARRAY_LEN(a)    ( sizeof(a) / sizeof(a[0]) )

#define LOG_MESSAGE(type, ...) do { \
            printf("%s:%d: " type ": ", __FILE__, __LINE__); \
            printf( __VA_ARGS__); \
        } while(0)

#ifdef ENABLE_ERROR
    #define ERROR(...)      LOG_MESSAGE("error", __VA_ARGS__)
#else
    #define ERROR(...)
#endif

#ifdef ENABLE_DEBUG
    #define DEBUG(...)      LOG_MESSAGE("debug", __VA_ARGS__)
#else
    #define DEBUG(...)
#endif

#ifdef ENABLE_INFO
    #define INFO(...)      LOG_MESSAGE("info", __VA_ARGS__)
#else
    #define INFO(...)
#endif

/* returns true if the file/directory exits */
bool statok(char *path);
/* determine filesize via file descriptor, uses fseek/ftell/fseek */
uint64_t fdsize(FILE* fd);
/* return current unix epoch in milliseconds */
// uint64_t mstime();
uint64_t gettime();
struct s_format_time {
    char str[9]; /* store a time string in the form: 00:00:00 */
};
struct s_format_time formattime(uint64_t time);
/* generate md5 hash for data */
void md5(char *data, size_t data_size, char **hash);
struct s_hex_md5 {
    char str[16 * 2 + 1];
};
struct s_hex_md5 md5hex(char *hash);

int md5mod(char *hash, int size);

struct s_hsize {
    char str[15];
};
struct s_hsize hsize(uint64_t size);

/* slice partial string between begin and end,
 * Returns pointer to the first occurence of begin.
 * The string is moved to one byte after the end.
 * The first byte of found end is null-byte terminated.
 * Returns NULL if either begin or end couldn't be found.  */
char *slice_string(char **string, const char *begin, const char *end);
/* copy string to newly allocated memory */
char *copy_string(char *str);
/* join two strings into newly allocated memory */
char *join_string(char *s1, char *s2);
/* move str pointer after beginning whitespace and nullbyte terminate
 * last whitespace(s) */
char *trim_string(char *str);
/* allocate new memory and copy for integer */
int *copy_integer(int integer);

#endif /* _COMMON_H */

