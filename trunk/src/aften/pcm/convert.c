/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
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
 * Raw audio sample format conversion
 */

#include "pcm.h"

static void
fmt_convert_u8_to_u8(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(uint8_t));
}

static void
fmt_convert_s8_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int)src[i] + 128;
}

static void
fmt_convert_s16_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 8) + 128;
}

static void
fmt_convert_s20_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 12) + 128;
}

static void
fmt_convert_s24_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 16) + 128;
}

static void
fmt_convert_s32_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 24) + 128;
}

static void
fmt_convert_float_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (uint8_t)CLIP(((src[i] * 128) + 128), 0, 255);
}

static void
fmt_convert_double_to_u8(void *dest_v, void *src_v, int n)
{
    uint8_t *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (uint8_t)CLIP(((src[i] * 128) + 128), 0, 255);
}

static void
fmt_convert_u8_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int)src[i] - 128;
}

static void
fmt_convert_s8_to_s8(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int8_t));
}

static void
fmt_convert_s16_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] >> 8;
}

static void
fmt_convert_s20_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] >> 12;
}

static void
fmt_convert_s24_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] >> 16;
}

static void
fmt_convert_s32_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] >> 24;
}

static void
fmt_convert_float_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int8_t)CLIP((src[i] * 128), -128, 127);
}

static void
fmt_convert_double_to_s8(void *dest_v, void *src_v, int n)
{
    int8_t *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int8_t)CLIP((src[i] * 128), -128, 127);
}

static void
fmt_convert_u8_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] - 128) << 8;
}

static void
fmt_convert_s8_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] << 8;
}

static void
fmt_convert_s16_to_s16(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int16_t));
}

static void
fmt_convert_s20_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 4);
}

static void
fmt_convert_s24_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 8);
}

static void
fmt_convert_s32_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 16);
}

static void
fmt_convert_float_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int16_t)CLIP((src[i] * 32768), -32768, 32767);
}

static void
fmt_convert_double_to_s16(void *dest_v, void *src_v, int n)
{
    int16_t *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int16_t)CLIP((src[i] * 32768), -32768, 32767);
}

static void
fmt_convert_u8_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] - 128) << 12;
}

static void
fmt_convert_s8_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] << 12;
}

static void
fmt_convert_s16_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] << 4);
}

static void
fmt_convert_s20_to_s20(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int32_t));
}

static void
fmt_convert_s24_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 4);
}

static void
fmt_convert_s32_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 12);
}

static void
fmt_convert_float_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 524288), -524288, 524287);
}

static void
fmt_convert_double_to_s20(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 524288), -524288, 524287);
}

static void
fmt_convert_u8_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] - 128) << 16;
}

static void
fmt_convert_s8_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] << 16;
}

static void
fmt_convert_s16_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] << 8);
}

static void
fmt_convert_s20_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] << 4);
}

static void
fmt_convert_s24_to_s24(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int32_t));
}

static void
fmt_convert_s32_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] >> 8);
}

static void
fmt_convert_float_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 8388608), -8388608, 8388607);
}

static void
fmt_convert_double_to_s24(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int32_t)CLIP((src[i] * 8388608), -8388608, 8388607);
}

static void
fmt_convert_u8_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] - 128) << 24;
}

static void
fmt_convert_s8_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] << 24;
}

static void
fmt_convert_s16_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] << 16);
}

static void
fmt_convert_s20_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] << 12);
}

static void
fmt_convert_s24_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] << 8);
}

static void
fmt_convert_s32_to_s32(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(int32_t));
}

static void
fmt_convert_float_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int32_t)(src[i] * 2147483648LL);
}

static void
fmt_convert_double_to_s32(void *dest_v, void *src_v, int n)
{
    int32_t *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (int32_t)(src[i] * 2147483648LL);
}

static void
fmt_convert_u8_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] - FCONST(128.0)) / FCONST(128.0);
}

static void
fmt_convert_s8_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(128.0);
}

static void
fmt_convert_s16_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(32768.0);
}

static void
fmt_convert_s20_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(524288.0);
}

static void
fmt_convert_s24_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(8388608.0);
}

static void
fmt_convert_s32_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(2147483648.0);
}

static void
fmt_convert_float_to_float(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(float));
}

static void
fmt_convert_double_to_float(void *dest_v, void *src_v, int n)
{
    float *dest = dest_v;
    double *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (float)src[i];
}

static void
fmt_convert_u8_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    uint8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = (src[i] - FCONST(128.0)) / FCONST(128.0);
}

static void
fmt_convert_s8_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int8_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(128.0);
}

static void
fmt_convert_s16_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int16_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(32768.0);
}

static void
fmt_convert_s20_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(524288.0);
}

static void
fmt_convert_s24_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(8388608.0);
}

static void
fmt_convert_s32_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    int32_t *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i] / FCONST(2147483648.0);
}

static void
fmt_convert_float_to_double(void *dest_v, void *src_v, int n)
{
    double *dest = dest_v;
    float *src = src_v;
    int i;

    for (i = 0; i < n; i++)
        dest[i] = src[i];
}

static void
fmt_convert_double_to_double(void *dest_v, void *src_v, int n)
{
    memcpy(dest_v, src_v, n * sizeof(double));
}

#define SET_FMT_CONVERT_FROM(srcfmt, pf) \
{ \
    enum PcmSampleFormat rfmt = pf->read_format; \
\
    if (rfmt == PCM_SAMPLE_FMT_U8) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_u8; \
    else if (rfmt == PCM_SAMPLE_FMT_S8) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_s8; \
    else if (rfmt == PCM_SAMPLE_FMT_S16) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_s16; \
    else if (rfmt == PCM_SAMPLE_FMT_S20) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_s20; \
    else if (rfmt == PCM_SAMPLE_FMT_S24) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_s24; \
    else if (rfmt == PCM_SAMPLE_FMT_S32) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_s32; \
    else if (rfmt == PCM_SAMPLE_FMT_FLT) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_float; \
    else if (rfmt == PCM_SAMPLE_FMT_DBL) \
        pf->fmt_convert = fmt_convert_##srcfmt##_to_double; \
}

void
pcmfile_set_source_format(PcmFile *pf, enum PcmSampleFormat fmt)
{
    static const int fmt_bits[8] = { 8, 8, 16, 20, 24, 32, 32, 64 };

    fmt = CLIP(fmt, PCM_SAMPLE_FMT_U8, PCM_SAMPLE_FMT_DBL);
    pf->source_format = fmt;
    pf->bit_width = fmt_bits[fmt];
    switch (fmt) {
        case PCM_SAMPLE_FMT_U8:  SET_FMT_CONVERT_FROM(u8, pf);     break;
        case PCM_SAMPLE_FMT_S8:  SET_FMT_CONVERT_FROM(s8, pf);     break;
        case PCM_SAMPLE_FMT_S16: SET_FMT_CONVERT_FROM(s16, pf);    break;
        case PCM_SAMPLE_FMT_S20: SET_FMT_CONVERT_FROM(s20, pf);    break;
        case PCM_SAMPLE_FMT_S24: SET_FMT_CONVERT_FROM(s24, pf);    break;
        case PCM_SAMPLE_FMT_S32: SET_FMT_CONVERT_FROM(s32, pf);    break;
        case PCM_SAMPLE_FMT_FLT: SET_FMT_CONVERT_FROM(float, pf);  break;
        case PCM_SAMPLE_FMT_DBL: SET_FMT_CONVERT_FROM(double, pf); break;
        default:                                                   break;
    }
    if (fmt == PCM_SAMPLE_FMT_FLT || fmt == PCM_SAMPLE_FMT_DBL)
        pf->sample_type = PCM_SAMPLE_TYPE_FLOAT;
    else
        pf->sample_type = PCM_SAMPLE_TYPE_INT;
    pf->block_align = MAX(1, ((pf->bit_width + 7) >> 3) * pf->channels);
    pf->samples = (pf->data_size / pf->block_align);
}

void
pcmfile_set_source_params(PcmFile *pf, int ch, enum PcmSampleFormat fmt, int order, int sr)
{
    pf->channels = MAX(ch, 1);
    pf->ch_mask = pcm_get_default_ch_mask(ch);
    pf->order = CLIP(order, PCM_BYTE_ORDER_LE, PCM_BYTE_ORDER_BE);
    pf->sample_rate = MAX(sr, 1);
    pcmfile_set_source_format(pf, fmt);
}

void
pcmfile_set_read_format(PcmFile *pf, enum PcmSampleFormat read_format)
{
    pf->read_format = CLIP(read_format, PCM_SAMPLE_FMT_U8, PCM_SAMPLE_FMT_DBL);
    pcmfile_set_source_format(pf, pf->source_format);
}
