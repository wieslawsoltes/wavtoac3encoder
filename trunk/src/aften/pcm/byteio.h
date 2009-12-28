/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2007 Justin Ruggles
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file byteio.h
 * Byte buffer header
 */

#ifndef BYTEIO_H
#define BYTEIO_H

#include "common.h"

#define BYTEIO_BUFFER_SIZE 16384

typedef struct ByteIOContext {
    FILE *fp;
    uint8_t *buffer;
    int index;
    int size;
} ByteIOContext;

extern int byteio_init(ByteIOContext *ctx, FILE *fp);

extern void byteio_align(ByteIOContext *ctx);

extern int byteio_flush(ByteIOContext *ctx);

extern int byteio_read(void *ptr, int n, ByteIOContext *ctx);

extern int byteio_peek(void *ptr, int n, ByteIOContext *ctx);

extern void byteio_close(ByteIOContext *ctx);

#endif /* BYTEIO_H */
