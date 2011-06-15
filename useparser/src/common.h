#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

/* some definitions / compile-time configurations */

#define VERSION         "0.1"
#define DEBUG                           /* activates more or less useful debug information */
#define FILE_CHUNK_SIZE 1024 * 32       /* how many bytes, fread should read at once */
/* #define YENC_CHECK_SIZE                 yenc decoding tests for correct size */
#define YENC_CHECK_CRC                  /* yenc decoding tests for correct CRC */

#ifdef DEBUG
    #define LOG(...)    fprintf(stdout, __VA_ARGS__)
#else
    #define LOG(...)
#endif

#define FREE(ptr)       free(ptr); \
                        ptr = NULL;


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

#endif /* _COMMON_H */

