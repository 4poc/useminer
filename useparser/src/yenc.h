#ifndef _YENC_H
#define _YENC_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"

#define ZERO    0x00
#define CR      0x0d
#define	LF      0x0a
#define	ESC     0x3d
#define TAB     0x09
#define SPACE   0x20

/**
 * yEnc decode string to data buffer
 * returns the size of the buffer pointed to by decoded
 */
size_t yenc_decode(char *encoded, unsigned char **p_decoded);

#endif /* _YENC_H */

