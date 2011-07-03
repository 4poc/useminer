
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

uint64_t gettime()
{
    return time(NULL);
}

struct s_format_time formattime(uint64_t time)
{
    struct s_format_time fmt;
    uint8_t hour=0, min=0, sec=0;

    if(time >= 60 * 60) { /* hour */
        hour = time / (60 * 60);
        time -= hour * (60 * 60);
    }
    if(time >= 60) {
        min = time / 60;
        time -= min * 60;
    }
    sec = time;


    sprintf(&fmt.str, "%02d:%02d:%02d", hour, min, sec);

    return fmt;
}

void md5(char *data, size_t data_size, char **hash)
{
    MD5_CTX context;

    if(!*hash) {
        *hash = malloc(16 * sizeof(char)); //  + 1);
        DEBUG("allocate memory for md5 at %p\n", *hash);
        // *hash[16] = '\0';
    }
    if(!*hash) {
        ERROR("unable to allocate memory for gen_md5()\n");
        return;
    }

    MD5_Init(&context);
    MD5_Update(&context, data, data_size);
    MD5_Final((unsigned char*)(*hash), &context);
}

struct s_hex_md5 md5hex(char *hash)
{
    struct s_hex_md5 hex;
    int i;

    for(i = 0; i < 16; i++) {
        sprintf(hex.str + i*2, "%02x", (unsigned char)hash[i]);
    }

    return hex;
}

struct s_hsize hsize(uint64_t size)
{
    struct s_hsize hsize;

    if(size >= 1024.0*1024*1024) {
        sprintf(&hsize.str, "%0.3f GiB", (size / (1024.0*1024.0*1024.0)));
    }
    else if(size >= 1024*1024) {
        sprintf(&hsize.str, "%0.3f MiB", (size / (1024.0*1024.0)));
    }
    else if(size >= 1024) {
        sprintf(&hsize.str, "%0.3f KiB", (size / 1024.0));
    }
    else if(size < 1024) {
        sprintf(&hsize.str, "%d");
    }

    return hsize;
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

