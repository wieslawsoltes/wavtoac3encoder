/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2007 Justin Ruggles
 *
 * Conversion of 80-bit float to double is from FFmpeg intfloat_readwrite.c
 * Copyright (c) 2005 Michael Niedermayer <michaelni@gmx.at>
 *
 * Aften is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Aften is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Aften; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file aiff.c
 * AIFF file format
 */

#include "common.h"

#include "pcm.h"

/* IEEE 80 bits extended float */
typedef struct ExtFloat  {
    uint8_t exponent[2];
    uint8_t mantissa[8];
} ExtFloat;

static double
ext2dbl(const ExtFloat ext)
{
    uint64_t m = 0;
    int e, i;

    for (i = 0; i < 8; i++)
        m = (m<<8) + ext.mantissa[i];
    e = (((int)ext.exponent[0]&0x7f)<<8) | ext.exponent[1];
    if (e == 0x7fff && m)
        return 0.0; // really should be NaN, but simplifying for portability
    e -= 16383 + 63;        /* In IEEE 80 bits, the whole (i.e. 1.xxxx)
                             * mantissa bit is written as opposed to the
                             * single and double precision formats */
    if (ext.exponent[0]&0x80)
        m= -m;
    if (e > 0)
        m <<= e;
    else if (e < 0)
        m >>= -e;
    return m;
}

/* chunk id's */
#define FORM_ID     0x464F524D
#define AIFF_ID     0x41494646
#define COMM_ID     0x434F4D4D
#define SSND_ID     0x53534E44

/**
 * Reads an 80-bit extended float from the input stream
 */
static inline ExtFloat
readext(PcmFile *pf)
{
    ExtFloat x;
    if (byteio_read(&x, 10, &pf->io) != 10) {
        memset(&x, 0, sizeof(x));
        return x;
    }
    pf->filepos += 10;
    return x;
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
aiff_probe(uint8_t *data, int size)
{
    int id;

    if (!data || size < 12)
        return 0;
    id = data[3] | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
    if (id != FORM_ID)
        return 0;
    id = data[11] | (data[10] << 8) | (data[9] << 16) | (data[8] << 24);
    if (id != AIFF_ID)
        return 0;
    return 100;
}

static int
aiff_init(PcmFile *pf)
{
    int id, found_ssnd, found_comm, chunksize, offset;
    ExtFloat srate_ext;
    int channels=2, sample_rate=44100, block_align=4, bits=16;
    enum PcmSampleFormat fmt = PCM_SAMPLE_FMT_S16;

    // read FORM id. ignore size.
    id = read4be(pf);
    if (id != FORM_ID) {
        fprintf(stderr, "invalid FORM id in aiff header\n");
        return -1;
    }
    read4be(pf);

    // read AIFF id. ignore size.
    id = read4be(pf);
    if (id != AIFF_ID) {
        fprintf(stderr, "invalid AIFF id in aiff header\n");
        return -1;
    }

    // read all header chunks. skip unknown chunks.
    found_ssnd = found_comm = 0;
    while (!found_ssnd) {
        id = read4be(pf);
        chunksize = read4be(pf);
        switch (id) {
            case COMM_ID:
                if (chunksize < 18) {
                    fprintf(stderr, "invalid COMM chunk in aiff header\n");
                    return -1;
                }
                channels = read2be(pf);
                pf->samples = read4be(pf);
                bits = read2be(pf);
                srate_ext = readext(pf);
                sample_rate = ext2dbl(srate_ext);
                block_align = MAX(1, ((bits + 7) >> 3) * channels);
                pf->ch_mask = pcm_get_default_ch_mask(channels);

                chunksize -= 18;

                if (channels == 0) {
                    fprintf(stderr, "invalid number of channels in aiff header\n");
                    return -1;
                }
                if (sample_rate <= 0) {
                    fprintf(stderr, "invalid sample rate in aiff header\n");
                    return -1;
                }
                if (bits == 0) {
                    fprintf(stderr, "invalid sample bit width in aiff header\n");
                    return -1;
                }

                // skip any leftover bytes in fmt chunk
                chunksize += chunksize & 1;
                if (pcmfile_seek_set(pf, pf->filepos + chunksize)) {
                    fprintf(stderr, "error seeking in aiff file\n");
                    return -1;
                }
                found_comm = 1;
                break;
            case SSND_ID:
                if (!found_comm) {
                    fprintf(stderr, "COMM after SSND in aiff is not supported\n");
                    return -1;
                }
                offset = read4be(pf);
                read4be(pf);
                if (pcmfile_seek_set(pf, pf->filepos + offset)) {
                    fprintf(stderr, "error seeking in aiff file\n");
                    return -1;
                }
                pf->data_size = block_align * pf->samples;
                pf->data_start = pf->filepos;
                if (pf->seekable && pf->file_size > 0) {
                    // limit data size to end-of-file
                    pf->data_size = MIN(pf->data_size, pf->file_size - pf->data_start);
                    pf->samples = pf->data_size / block_align;
                }
                found_ssnd = 1;
                break;
            default:
                // skip unknown chunk
                chunksize += chunksize & 1;
                if (chunksize > 0 && pcmfile_seek_set(pf, pf->filepos + chunksize)) {
                    fprintf(stderr, "error seeking in aiff file\n");
                    return -1;
                }
        }
    }

    // set audio data format based on bit depth and sample type
    fmt = PCM_SAMPLE_FMT_UNKNOWN;
    switch (bits) {
        case  8: fmt = PCM_SAMPLE_FMT_S8;  break;
        case 16: fmt = PCM_SAMPLE_FMT_S16; break;
        case 20: fmt = PCM_SAMPLE_FMT_S20; break;
        case 24: fmt = PCM_SAMPLE_FMT_S24; break;
        case 32: fmt = PCM_SAMPLE_FMT_S32; break;
        default:
            fprintf(stderr, "unsupported bit depth: %d\n", bits);
            return -1;
    }
    pf->internal_fmt = 0;
    pcmfile_set_source_params(pf, channels, fmt, PCM_BYTE_ORDER_BE, sample_rate);

    return 0;
}

PcmFormat aiff_format = {
    "aiff",
    "Apple AIFF",
    PCM_FORMAT_AIFF,
    aiff_probe,
    aiff_init,
    NULL
};
