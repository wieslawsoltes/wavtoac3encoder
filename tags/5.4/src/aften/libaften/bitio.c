/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
 *
 * Based on "The simplest AC3 encoder" from FFmpeg
 * Copyright (c) 2000 Fabrice Bellard.
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
 * @file bitio.c
 * Bitwise file writer
 */

#include "bitio.h"

void
bitwriter_init(BitWriter *bw, void *buf, int len)
{
    if (len < 0) {
        len = 0;
        buf = NULL;
    }
    bw->buffer = buf;
    bw->buf_end = bw->buffer + len;
    bw->buf_ptr = bw->buffer;
    bw->bit_left = 32;
    bw->bit_buf = 0;
    bw->eof = 0;
}

void
bitwriter_flushbits(BitWriter *bw)
{
    bw->bit_buf <<= bw->bit_left;
    while (bw->bit_left < 32) {
        *bw->buf_ptr++ = bw->bit_buf >> 24;
        bw->bit_buf <<= 8;
        bw->bit_left += 8;
    }
    bw->bit_left = 32;
    bw->bit_buf = 0;
}

void
bitwriter_writebits(BitWriter *bw, int bits, uint32_t val)
{
    if (!bits || bits > 32)
        return;
    val = (val & ((1U << bits)-1));
    if (bits <= bw->bit_left) {
        bw->bit_buf = (bw->bit_buf << bits) | val;
        bw->bit_left -= bits;
    } else {
        uint32_t bb = (bw->bit_buf << bw->bit_left) | (val >> (bits - bw->bit_left));
        if (bw->buffer != NULL) {
            if (bw->eof)
                return;
            if ((bw->buf_ptr+3) >= bw->buf_end) {
                bw->eof = 1;
                return;
            }
            *(uint32_t *)bw->buf_ptr = be2me_32(bb);
        }
        bw->bit_left += (32 - bits);
        bw->buf_ptr += 4;
        bw->bit_buf = val;
    }
}

void
bitwriter_writebit(BitWriter *bw, uint8_t val)
{
    if (bw->bit_left) {
        bw->bit_buf = (bw->bit_buf << 1) | (val & 1);
        bw->bit_left--;
    } else {
        if (bw->buffer != NULL) {
            if (bw->eof)
                return;
            if ((bw->buf_ptr+3) >= bw->buf_end) {
                bw->eof = 1;
                return;
            }
            *(uint32_t *)bw->buf_ptr = be2me_32(bw->bit_buf);
        }
        bw->bit_left = 31;
        bw->buf_ptr += 4;
        bw->bit_buf = (val & 1);
    }
}

uint32_t
bitwriter_bitcount(BitWriter *bw)
{
    return (((bw->buf_ptr - bw->buffer) << 3) + 32 - bw->bit_left);
}
