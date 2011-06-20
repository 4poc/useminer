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

/* decode string buffer, parse yenc head and tail, return size of decode */
size_t yenc_decode(char *encoded, char **p_decoded);

#endif /* _YENC_H */

