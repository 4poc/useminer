#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "md5.h"

/* some definitions / compile-time configurations */
#define VERSION         "0.1"
/* logging/message stuff there are err, dbg and info messages that must be 
 * configured during compilation: */
#define ENABLE_ERROR
#define ENABLE_DEBUG
#define ENABLE_INFO
#define FILE_CHUNK_SIZE 1024 * 32       /* how many bytes, fread should read at once */
/* #define YENC_CHECK_SIZE                 yenc decoding tests for correct size */
#define YENC_CHECK_CRC                  /* yenc decoding tests for correct CRC */


/* some useful makros */

#define FREE(ptr)       if(ptr != NULL) {   \
                            free(ptr);      \
                            ptr = NULL;     \
                        }

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


/* and some helpful functions */

/**
 * Determines the filesize of the file point to by the file descriptor.
 *
 * Uses a fseek to the end of the file and ftell to determine the size of the file.
 */
uint64_t fdsize(FILE* fd);

/**
 * Returns current unix epoch time in milliseconds.
 */
uint64_t mstime();

/**
 * Slice partial string between the begin and end substrings.
 *
 * Returns pointer to the first occurence of begin.
 * The string is moved to one byte after the end.
 * The first byte of found end is null-byte terminated.
 * Returns NULL if either begin or end couldn't be found.
 */
char *pslice(char **string, const char *begin, const char *end);

/**
 */
// void gen_md5(unsigned char *data, size_t data_size, unsigned char *hash);

extern void gen_md5(unsigned char *data, size_t data_size, unsigned char **hash);

char *copy_string(char *str);

char *join_string(char *first, char *second);

#endif /* _COMMON_H */

