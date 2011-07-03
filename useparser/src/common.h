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
// #define ENABLE_DEBUG
#define ENABLE_INFO
#define FILE_CHUNK_SIZE 1024 * 16   /* fread chunks */
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

