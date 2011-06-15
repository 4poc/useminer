#include "zlib.h"

size_t zlib_inflate(unsigned char *in_buffer, size_t in_size, char **p_out_buffer)
{
    int ret;
    unsigned char *out_realloc, *out_buffer, *out_cur;
    size_t out_size = ZLIB_CHUNK_SIZE;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, ZLIB_WINDOW_BITS);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = in_size;
    strm.next_in = in_buffer;

    /* allocate first chunk of out buffer */
    out_buffer = out_cur = malloc(out_size);
    if(out_buffer == NULL) {
        fprintf(stderr, "Error allocate output buffer!\n");
        return -1;
    }

    for(;;) { /* until input buffer is consumed */
        strm.avail_out = ZLIB_CHUNK_SIZE;
        strm.next_out = out_cur;

        ret = inflate(&strm, Z_NO_FLUSH);
        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return -1;
        }

        if(ret == Z_STREAM_END) {
            break;
        }

        out_size += ZLIB_CHUNK_SIZE;
        out_realloc = realloc(out_buffer, out_size);
        if(!out_realloc) {
            inflateEnd(&strm);
            free(out_buffer);
            fprintf(stderr, "[err] unable to grow out buffer (%zu bytes)\n", 
                    out_size);
            return -1;
        }
        out_buffer = out_realloc;
        out_cur = &out_buffer[out_size - ZLIB_CHUNK_SIZE];
    }
    *p_out_buffer = (char*)out_buffer; /* FIXME: unsigned -> signed ? */

    inflateEnd(&strm);
    return ret == Z_STREAM_END ? out_size : -1;
}

