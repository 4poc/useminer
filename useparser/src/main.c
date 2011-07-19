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
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "config.h"
#include "yenc.h"
#include "zlib.h"
#include "parse.h"

/* overview, raw information within the overview database of the usenet server
 * file, a single file made of N segments
 * segment, yEnc encoded binary segment (a.k.a. part) of a single file,
 *          equals to a single usenet message/article.
 *
 * cache, stores incompleted files in memory and on disk
 */

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
    size_t tmp=0, fbuffer_total=0, fbuffer_used=0; 
    uint64_t fsize = 0; 

    /* multiline xzver responses are yEnc encoded, for decoding: */
    char *end_of_message = NULL, *yenc_decode_buffer = NULL;
    size_t yenc_decode_size=0;

    /* decoded buffer is zlib inflated, resulting ASCII plaintext that is 
     * read line-by-line */
    char *plain = NULL, *line = NULL;
    size_t plain_size=0;

    /* to measure the speed of reading the file */
    uint64_t time_start, stat_chunknum=0, stat_messages=0;

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
    INFO("opened file %s (size: %s)\n", fname, hsize(fsize).str);

    /* sigint (C-c) aborts file read */
    signal(SIGINT, sigint_handler);

    /* configuration TODO: config file via argument */
    if(!config_load("useparser.cfg")) {
        ERROR("unable to load config file!\n");
        return -1;
    }
    //DEBUG("Test: >%s<\n", config_string("storage_disk_path"));
    
    if(!parse_init()) {
        ERROR("unable to initialize parser!\n");
        return -1;
    }

    time_start = gettime();
    while(feof(fd) == 0 && !abort_fread) {
        if(fread(&fchunk, sizeof(char), FILE_CHUNK_SIZE, fd) != FILE_CHUNK_SIZE 
                && ferror(fd) != 0) {
            ERROR("fread error occured\n");
            break;
        }
        stat_chunknum++;

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

            stat_messages++;

#ifndef ENABLE_DEBUG
            uint64_t stat_delta_time = gettime() - time_start,
                     stat_read = stat_chunknum * FILE_CHUNK_SIZE;
            if(stat_delta_time <= 0) {
                stat_delta_time = 1;
            }

            printf("------------------------------------------------\n");
            printf("\x1b[2KRead:   %s/%s (%s/s)\n", 
                    hsize(stat_read).str,
                    hsize(fsize).str, 
                    hsize(stat_read/stat_delta_time).str);

            printf("\x1b[2K        %s allocated message memory\n", 
                    hsize(fbuffer_total).str); 
            printf("------------------------------------------------\n");
            printf("\x1b[2KTime:   %s (ETA: %s)\n",
                    formattime(stat_delta_time).str,
                    formattime(
                        (fsize - stat_read) / (stat_read/stat_delta_time)).str);
            printf("------------------------------------------------\n");
            printf("\x1b[2KParse:  %" PRIu64 "/~%" PRIu64 " message chunks\n", 
                    stat_messages, 
                    (fsize / fbuffer_used));
            printf("\x1b[2K        %d/%d complete/incomplete files\n",
                    parse_stat_completed, parse_stat_incomplete);
            printf("\x1b[2K        %d skipped completed files\n",
                    parse_stat_skip_completed);
            printf("\x1b[2K        %d segments (garbage: %d)\n",
                    parse_stat_segments,
                    parse_stat_lines - parse_stat_segments);
            printf("------------------------------------------------\n");
            printf("\x1b[2KCache:  %d/%d cache table slots\n",
                    cache_stat_slots,
                    *config_integer("cache_table_size"));
            printf("\x1b[12F");
#endif

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
                    parse_process(line);

                    /* proceed with next line */
                    if(i+2 < plain_size) {
                        line = &plain[i+2];
                    }
                }
            } /* end line by line parsing */
            DEBUG("read %d lines from message chunk\n", i);
            FREE(plain);

            /* exit(0); */
            abort_fread = true;
            
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
#ifndef ENABLE_DEBUG
    printf("\x1b[13E");
#endif
    printf("\n\n");

    parse_uninit();

    FREE(fbuffer);

    fclose(fd);

    config_unload();

    return 0;
}

void sigint_handler(int dummy)
{
    DEBUG("sigint signal caught\n");
    abort_fread = true;
}

