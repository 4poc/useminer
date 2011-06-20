#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "md5.h"

#define VERSION         "0.0.1"
/* enable log level error, debug info: */
#define ENABLE_ERROR
//#define ENABLE_DEBUG
#define ENABLE_INFO
#define FILE_CHUNK_SIZE 1024 * 32   /* fread chunks */
/* #define YENC_CHECK_SIZE             yenc decoding tests for correct size */
#define YENC_CHECK_CRC              /* yenc decoding tests for correct CRC */


#define FREE(ptr)       if(ptr != NULL) {   \
                            free(ptr);      \
                            ptr = NULL;     \
                        } else {}

#define ARRAY_LEN(a)    ( sizeof(a) / sizeof(a[0]) )

#ifdef ENABLE_ERROR
    #define ERROR(...)    fprintf(stderr, "[error] " __VA_ARGS__);
#else
    #define ERROR(...)
#endif

#ifdef ENABLE_DEBUG
    #define DEBUG(...)    printf("[debug] " __VA_ARGS__);
#else
    #define DEBUG(...)
#endif

#ifdef ENABLE_INFO
    #define INFO(...)    printf("[info] " __VA_ARGS__);
#else
    #define INFO(...)
#endif

/* determine filesize via file descriptor, uses fseek/ftell/fseek */
uint64_t fdsize(FILE* fd);
/* return current unix epoch in milliseconds */
uint64_t mstime();
/* generate md5 hash for data */
void md5(char *data, size_t data_size, char **hash);

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

#endif /* _COMMON_H */

