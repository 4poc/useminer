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
#ifndef _ZLIB_H
#define _ZLIB_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "common.h"

#define ZLIB_CHUNK_SIZE     16384
#define ZLIB_WINDOW_BITS    -15

size_t zlib_inflate(char *in_buffer, size_t in_size, char **p_out_buffer);

#endif

