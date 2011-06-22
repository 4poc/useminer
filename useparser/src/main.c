#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "yenc.h"
#include "zlib.h"
#include "overview.h"
#include "binary.h"
#include "parser.h"

static bool abort_fread = false; 

void sigint_handler(int dummy);

int main(int argc, const char* argv[])
{
    int i;

    /* file reading by chunks into heap buffer */
    const char *fname;
    FILE *fd = NULL;
    char fchunk[FILE_CHUNK_SIZE];
    char *fbuffer = NULL, *fbuffer_realloc = NULL;
    size_t tmp=0, fsize=0, fbuffer_total=0, fbuffer_used=0; 

    /* multiline xzver responses are yEnc encoded, for decoding: */
    char *end_of_message = NULL, *yenc_decode_buffer = NULL;
    size_t yenc_decode_size=0;

    /* decoded buffer is zlib inflated, resulting ASCII plaintext that is 
     * read line-by-line */
    char *plain = NULL, *line = NULL;
    size_t plain_size=0;

    /* to measure the speed of reading the file */
    uint64_t time_start, count_chunks=0;
    uint16_t est_min, est_sec;

    INFO("useparser v" VERSION " (" __DATE__ " " __TIME__ ")\n");
    INFO(" ---------------------------------- \n");

    /* work with UTC/GMT everywhere */
    setenv("TZ", "UTC", 1);
    tzset();

    if(argc != 2) {
        ERROR("you need to specify the <FILE> parameter\n\n" \
                "Usage: %s <FILE>\n\nFILE can be a filename or - " \
                "to read from standard input instead.\n", argv[0]);
        return -1;
    }
    fname = argv[1];

    if((fd = fopen(fname, "r")) == NULL) {
        ERROR("unable to open '%s' (errno:%d)!\n", fname, errno);
        return -1;
    }

    fsize = fdsize(fd);
    if(fsize == -1) {
        ERROR("unable to determine filesize (errno:%d)!\n", errno);
        return -1;
    }
    INFO("opened file %s (size: %zu B | %0.2f MiB)\n", 
            fname, fsize, (fsize / 1024.0 / 1024.0));

    /* sigint (C-c) aborts file read */
    signal(SIGINT, sigint_handler);

    parser_startup();

    time_start = mstime();
    while(feof(fd) == 0 && !abort_fread) {
        if(fread(&fchunk, sizeof(char), FILE_CHUNK_SIZE, fd) != FILE_CHUNK_SIZE 
                && ferror(fd) != 0) {
            ERROR("fread error occured\n");
            break;
        }
        count_chunks++;

        /* grow buffer to store chunk */
        if(fbuffer == NULL) {
            fbuffer = malloc(FILE_CHUNK_SIZE);
            fbuffer_total = FILE_CHUNK_SIZE;
            DEBUG("allocate ybuf memory (%d bytes)\n", FILE_CHUNK_SIZE);
        }
        else if(fbuffer_total - fbuffer_used < FILE_CHUNK_SIZE) {
            tmp = fbuffer_total 
                - (fbuffer_total - fbuffer_used) /* ignore the unused bytes */ 
                + FILE_CHUNK_SIZE /* the chunk that gets copied */
                + 1; /* reserve one byte for null-byte terminator */ 
            fbuffer_realloc = realloc(fbuffer, tmp);
            if(fbuffer_realloc == NULL) {
                ERROR("unable to (re)allocate memory (%zu bytes)\n", tmp);
                break;
            }
            fbuffer = fbuffer_realloc;
            fbuffer_total = tmp;
        }

        /* append fread chunk to memory buffer ybuf */
        memcpy(&fbuffer[fbuffer_used], fchunk, FILE_CHUNK_SIZE);
        fbuffer_used += FILE_CHUNK_SIZE;
        if(fbuffer_total > fbuffer_used) { 
            /* terminate to make sure we don't search in unused memory 
             * and we can use strcpy... und alle so yeah! */ 
            fbuffer[fbuffer_used] = '\0';
        }

        /* process/consume completed multiline messages in buffer */
        while((end_of_message = strstr(fbuffer, "\r\n.\r\n")) != NULL) {
            *end_of_message = '\0';
            est_min = 0;
            est_sec = (fsize - count_chunks * FILE_CHUNK_SIZE) / 
                ((count_chunks * FILE_CHUNK_SIZE) /
                 ((mstime()-time_start)/1000.0));
            if(est_sec >= 60) {
                est_min = est_sec / 60;
                est_sec -= est_min * 60;
            }
            printf("\x1b[1Kdecode xover message.. " \
                   "(T:%0.2f MiB | S:%0.2f MiB/s [%02d:%02d min])\r",
                    (count_chunks * FILE_CHUNK_SIZE / 1024.0 / 1024.0),
                    (count_chunks * FILE_CHUNK_SIZE / 1024.0 / 1024.0) / 
                    ((mstime()-time_start)/1000.0),
                    est_min,
                    est_sec);

            /* decode yEnc encoded data */
            yenc_decode_size = yenc_decode(fbuffer, &yenc_decode_buffer);
            if(yenc_decode_size == -1) {
                ERROR("yenc decoding error\n");
                return -1;
            }

            /* inflate yEnd decoded data (to plaintext) */
            plain_size = zlib_inflate(yenc_decode_buffer, 
                                      yenc_decode_size, 
                                      &plain);

            if(plain_size == -1) {
                ERROR("zlib inflate error\n");
                return -1;
            }
            FREE(yenc_decode_buffer);

            /* read line-wise in plaintext buffer */
            line = plain;
            for(i = 0; i < plain_size-1; i++) {
                if(plain[i] == '\r' && plain[i+1] == '\n') {
                    plain[i] = '\0';

                    /* parse line and store new/updated multipart binary */
                    parser_process(line);

                    /* proceed with next line */
                    if(i+2 < plain_size) {
                        line = &plain[i+2];
                    }
                }
            } /* end line by line parsing */
            DEBUG("read %d lines from message chunk\n", i);
            FREE(plain);
            //exit(0);
            
            if(fbuffer - (end_of_message+5) == fbuffer_used) {
                /* do nothing (unlikely) */
            }
            else {
                strcpy(fbuffer, end_of_message+5);
                fbuffer_used = strlen(fbuffer);
                DEBUG("moved remaining memory " \
                        "(used:%zu, total:%zu size:%zu)\n", 
                        fbuffer_used, fbuffer_total,strlen(fbuffer));
            }
        }
    }
    printf("\n");

    parser_shutdown();

    FREE(fbuffer);

    fclose(fd);

    return 0;
}

void sigint_handler(int dummy)
{
    printf("\n\n >sigint signal caught< \n\n");
    abort_fread = true;
}

