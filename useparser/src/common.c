
#include "common.h"

uint64_t fdsize(FILE* fd)
{
    uint64_t size;

    /* seek to end of file*/
    if(fseek(fd, 0, SEEK_END) != 0) {
        ERROR("unable to determine filesize, fseek error.\n");
        return -1;
    }

    /* current position == filesize */
    size = ftell(fd);

    /* rewind to start of file */
    rewind(fd);

    return size;
}

uint64_t mstime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000.0 + 0.5);
}

void md5(char *data, size_t data_size, char **hash)
{
    MD5_CTX context;

    if(!*hash) {
        *hash = malloc(16 * sizeof(char));
        DEBUG("allocate memory for md5 at %p\n", *hash);
    }
    if(!*hash) {
        ERROR("unable to allocate memory for gen_md5()\n");
        return;
    }

    MD5_Init(&context);
    MD5_Update(&context, data, data_size);
    MD5_Final((unsigned char*)(*hash), &context);
}

char *slice_string(char **string, const char *begin, const char *end)
{
    char *pbegin = NULL, *pend = NULL;
    pbegin = strstr(*string, begin);
    if(pbegin != NULL) { 
        /* continue the search after the found substring */
        pbegin += strlen(begin); 
        pend = strstr(pbegin, end);
        if(pend != NULL) {
            *string = pend + strlen(end);
            *pend = '\0'; /* null-byte terminate the slice */
            return pbegin;
        }
    }
    return NULL;
}


char *copy_string(char *str)
{
    char *str_copy;

    str_copy = malloc(strlen(str) + 1); /* ? +1 nullbyte?*/
    if(!str_copy) {
        ERROR("unable to allocate memory for string copy\n");
        return NULL;
    }

    strcpy(str_copy, str);

    return str_copy;
}

char *join_string(char *s1, char *s2)
{
    char *joined = NULL;
    size_t s1_size, s2_size;
   
    s1_size = strlen(s1);
    s2_size = strlen(s2);

    joined = malloc(s1_size + s2_size + 1); /* + null-byte */
    if(!joined) {
        ERROR("unable to allocate memory to join strings!\n");
        return NULL;
    }

    memcpy(joined, s1, s1_size);
    memcpy(joined + s1_size, s2, s2_size + 1);

    return joined;
}

char *trim_string(char *str)
{
    char *end;
    for(str=str; *str==' '; str++);
    while((end = strrchr(str, ' '))) {
        *end = '\0';
    }

    return str;
}

int *copy_integer(int integer)
{
    int *p;
    p = malloc(sizeof(int));
    *p = integer;
    return p;
}

