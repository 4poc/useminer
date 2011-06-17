
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

char *pslice(char **string, const char *begin, const char *end)
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

unsigned char *gen_md5(unsigned char *data, size_t data_size)
{
    unsigned char *hash;
    MD5_CTX context;

    hash = malloc(16 * sizeof(unsigned char));
    if(!hash) {
        ERROR("unable to allocate memory for gen_md5()\n");
        return NULL;
    }

    MD5_Init(&context);
    MD5_Update(&context, data, data_size);
    MD5_Final(&hash[0], &context);

    return hash;
}

