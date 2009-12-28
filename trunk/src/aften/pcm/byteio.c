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
 * @file byteio.c
 * Byte buffer
 */

#include "byteio.h"

int
byteio_init(ByteIOContext *ctx, FILE *fp)
{
    ctx->buffer = calloc(BYTEIO_BUFFER_SIZE, 1);
    if (!ctx->buffer)
        return -1;
    ctx->fp = fp;
    ctx->index = 0;
    ctx->size = 0;
    byteio_flush(ctx);
    return 0;
}

void
byteio_align(ByteIOContext *ctx)
{
    memmove(ctx->buffer, &ctx->buffer[ctx->index], ctx->size);
    ctx->size += fread(&ctx->buffer[ctx->size], 1, BYTEIO_BUFFER_SIZE-ctx->size,
                       ctx->fp);
    ctx->index = 0;
}

int
byteio_flush(ByteIOContext *ctx)
{
    ctx->index = 0;
    ctx->size = fread(ctx->buffer, 1, BYTEIO_BUFFER_SIZE, ctx->fp);
    return ctx->size;
}

int
byteio_read(void *ptr, int n, ByteIOContext *ctx)
{
    uint8_t *ptr8 = ptr;
    int count = 0;

    while (n > ctx->size) {
        memcpy(&ptr8[count], &ctx->buffer[ctx->index], ctx->size);
        count += ctx->size;
        n -= ctx->size;
        if (byteio_flush(ctx) == 0)
            break;
    }
    if (ctx->size >= n && n > 0) {
        memcpy(&ptr8[count], &ctx->buffer[ctx->index], n);
        count += n;
        ctx->size -= n;
        ctx->index += n;
    }
    return count;
}

int
byteio_peek(void *ptr, int n, ByteIOContext *ctx)
{
    int nr;

    if (n > ctx->size)
        byteio_align(ctx);
    nr = MIN(n, ctx->size);
    memcpy(ptr, &ctx->buffer[ctx->index], nr);
    return nr;
}

void
byteio_close(ByteIOContext *ctx)
{
    if (ctx) {
        ctx->fp = NULL;
        if (ctx->buffer)
            free(ctx->buffer);
        ctx->index = 0;
        ctx->size = 0;
    }
}
