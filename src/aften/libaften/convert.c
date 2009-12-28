/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
 *               2007 Prakash Punnoor <prakash@punnoor.de>
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
 * @file convert.c
 * converts external format to internal format
 */

#include "a52enc.h"
#include "convert.h"

static void
fmt_convert_from_u8(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                    const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const uint8_t *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const uint8_t *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = (src_ch[j]-FCONST(128.0)) / FCONST(128.0);
        }
    }
}

static void
fmt_convert_from_s8(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                    const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const int8_t *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const int8_t *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = src_ch[j] / FCONST(128.0);
        }
    }
}

static void
fmt_convert_from_s16(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                     const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const int16_t *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const int16_t *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = src_ch[j] / FCONST(32768.0);
        }
    }
}

static void
fmt_convert_from_s20(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                     const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const int32_t *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const int32_t *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = src_ch[j] / FCONST(524288.0);
        }
    }
}

static void
fmt_convert_from_s24(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                     const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const int32_t *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const int32_t *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = src_ch[j] / FCONST(8388608.0);
        }
    }
}

static void
fmt_convert_from_s32(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                     const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const int32_t *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const int32_t *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = src_ch[j] / FCONST(2147483648.0);
        }
    }
}

static void
fmt_convert_from_float(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                       const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const float *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const float *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = src_ch[j];
        }
    }
}

static void
fmt_convert_from_double(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
                        const void *vsrc, int nch, int n)
{
    int i, j, ch;
    const double *src = vsrc;

    for (ch = 0; ch < nch; ch++) {
        FLOAT *dest_ch = dest[ch];
        const double *src_ch = src + ch;
        for (i = 0, j = 0; i < n; i++, j += nch) {
            dest_ch[i] = (FLOAT)src_ch[j];
        }
    }
}

void
set_converter(A52Context *ctx, A52SampleFormat sample_format)
{
    switch (sample_format) {
    case A52_SAMPLE_FMT_U8:  ctx->fmt_convert_from_src = fmt_convert_from_u8;
        break;
    case A52_SAMPLE_FMT_S8:  ctx->fmt_convert_from_src = fmt_convert_from_s8;
        break;
    case A52_SAMPLE_FMT_S16: ctx->fmt_convert_from_src = fmt_convert_from_s16;
        break;
    case A52_SAMPLE_FMT_S20: ctx->fmt_convert_from_src = fmt_convert_from_s20;
        break;
    case A52_SAMPLE_FMT_S24: ctx->fmt_convert_from_src = fmt_convert_from_s24;
        break;
    case A52_SAMPLE_FMT_S32: ctx->fmt_convert_from_src = fmt_convert_from_s32;
        break;
    case A52_SAMPLE_FMT_FLT: ctx->fmt_convert_from_src = fmt_convert_from_float;
        break;
    case A52_SAMPLE_FMT_DBL: ctx->fmt_convert_from_src = fmt_convert_from_double;
        break;
    default: break;
    }
}
