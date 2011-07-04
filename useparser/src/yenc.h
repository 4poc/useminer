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

