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
 * @file caff.c
 * Apple Core Audio File Format (CAFF)
 */

#include "pcm.h"

#define CAFF_ID 0x63616666
#define DESC_ID 0x64657363
#define DATA_ID 0x64617461

#define LPCM_TAG 0x6C70636D

#define FLAG_IS_FLOAT         0x1
#define FLAG_IS_LITTLEENDIAN  0x2

/**
 * Reads a 8-byte big-endian word from the input stream
 */
static inline uint64_t
read8be(PcmFile *pf)
{
    uint64_t x;
    if (byteio_read(&x, 8, &pf->io) != 8)
        return 0;
    pf->filepos += 8;
    return be2me_64(x);
}

/**
 * Reads a 8-byte big-endian word from the input stream as a double value
 */
static inline double
read_dbl_be(PcmFile *pf)
{
    union {
        uint64_t i;
        double d;
    } x;
    x.i = read8be(pf);
    return x.d;
}

/**
 * Reads a 4-byte big-endian word from the input stream
 */
static inline uint32_t
read4be(PcmFile *pf)
{
    uint32_t x;
    if (byteio_read(&x, 4, &pf->io) != 4)
        return 0;
    pf->filepos += 4;
    return be2me_32(x);
}

/**
 * Reads a 2-byte big-endian word from the input stream
 */
static inline uint16_t
read2be(PcmFile *pf)
{
    uint16_t x;
    if (byteio_read(&x, 2, &pf->io) != 2)
        return 0;
    pf->filepos += 2;
    return be2me_16(x);
}

static int
caff_probe(uint8_t *data, int size)
{
    int id;

    if (!data || size < 6)
        return 0;
    id = data[3] | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
    if (id == CAFF_ID && data[4] == 0 && data[5] == 1)
        return 100;

    return 0;
}

static int
caff_init(PcmFile *pf)
{
    int id, found_data, format_flags;
    enum PcmSampleFormat src_fmt;
    uint64_t chunksize;

    // read "caff" id
    if (read4be(pf) != CAFF_ID) {
        fprintf(stderr, "CAFF: file type check failed\n");
        return -1;
    }

    // check file version
    if (read2be(pf) != 1) {
        fprintf(stderr, "CAFF: file version check failed\n");
        return -1;
    }

    // skip file flags
    read2be(pf);

    // audio description chunk
    if (read4be(pf) != DESC_ID) {
        fprintf(stderr, "CAFF: 'desc' chunk not present\n");
        return -1;
    }
    chunksize = read8be(pf);
    if (chunksize != 32) {
        fprintf(stderr, "CAFF: invalid 'desc' chunk size\n");
        return -1;
    }
    pf->sample_rate = read_dbl_be(pf);
    pf->internal_fmt = read4be(pf);
    format_flags = read4be(pf);
    pf->order = (format_flags & FLAG_IS_LITTLEENDIAN) ? PCM_BYTE_ORDER_LE: PCM_BYTE_ORDER_BE;
    pf->sample_type = (format_flags & FLAG_IS_FLOAT) ? PCM_SAMPLE_TYPE_FLOAT: PCM_SAMPLE_TYPE_INT;
    pf->block_align = read4be(pf);
    read4be(pf);
    pf->channels = read4be(pf);
    pf->ch_mask = pcm_get_default_ch_mask(pf->channels);
    pf->bit_width = read4be(pf);

    // validate some parameters
    if (pf->sample_rate < 1) {
        fprintf(stderr, "CAFF: Invalid sample rate: %d\n", pf->sample_rate);
        return -1;
    }
    if (pf->block_align < 1) {
        fprintf(stderr, "CAFF: Invalid block align: %d\n", pf->block_align);
        return -1;
    }
    if (pf->channels < 1 || pf->channels > PCM_MAX_CHANNELS) {
        fprintf(stderr, "CAFF: Invalid number of channels: %d\n", pf->channels);
        return -1;
    }
    if (pf->internal_fmt != LPCM_TAG) {
        fprintf(stderr, "CAFF: Unsupported codec: 0x%04X\n", pf->internal_fmt);
        return -1;
    }

    // set audio data format based on bit depth and sample type
    src_fmt = PCM_SAMPLE_FMT_UNKNOWN;
    switch (pf->bit_width) {
        case 8:  src_fmt = PCM_SAMPLE_FMT_S8;  break;
        case 16: src_fmt = PCM_SAMPLE_FMT_S16; break;
        case 20: src_fmt = PCM_SAMPLE_FMT_S20; break;
        case 24: src_fmt = PCM_SAMPLE_FMT_S24; break;
        case 32:
            if (pf->sample_type == PCM_SAMPLE_TYPE_FLOAT)
                src_fmt = PCM_SAMPLE_FMT_FLT;
            else if (pf->sample_type == PCM_SAMPLE_TYPE_INT)
                src_fmt = PCM_SAMPLE_FMT_S32;
            break;
        case 64:
            if (pf->sample_type == PCM_SAMPLE_TYPE_FLOAT) {
                src_fmt = PCM_SAMPLE_FMT_DBL;
            } else {
                fprintf(stderr, "64-bit integer samples not supported\n");
                return -1;
            }
            break;
    }
    pcmfile_set_source_format(pf, src_fmt);

    // read all header chunks. skip unknown chunks.
    found_data = 0;
    while (!found_data) {
        id = read4be(pf);
        chunksize = read8be(pf);
        switch (id) {
            case DATA_ID:
                read4be(pf);
                pf->data_size = chunksize - 4;
                pf->data_start = pf->filepos;
                if (pf->seekable && pf->file_size > 0) {
                    // limit data size to end-of-file
                    if (pf->data_size > 0)
                        pf->data_size = MIN(pf->data_size, pf->file_size - pf->data_start);
                    else
                        pf->data_size = pf->file_size - pf->data_start;
                }
                pf->samples = (pf->data_size / pf->block_align);
                found_data = 1;
                break;
            default:
                // skip unknown chunk
                if (chunksize > 0 && pcmfile_seek_set(pf, pf->filepos + chunksize)) {
                    fprintf(stderr, "error seeking in CAFF file\n");
                    return -1;
                }
        }
    }

    return 0;
}

PcmFormat caff_format = {
    "caff",
    "Apple CAFF",
    PCM_FORMAT_CAFF,
    caff_probe,
    caff_init,
    NULL
};
