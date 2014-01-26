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
 * @file pcm_io.c
 * raw PCM file i/o
 */

#include "pcmfile.h"

#ifdef WORDS_BIGENDIAN
#define PCM_NON_NATIVE_BYTE_ORDER  PCM_BYTE_ORDER_LE
#else
#define PCM_NON_NATIVE_BYTE_ORDER  PCM_BYTE_ORDER_BE
#endif

int
pcmfile_seek_set(PcmFile *pf, uint64_t dest)
{
    FILE *fp = pf->io.fp;
    int slow_seek = !(pf->seekable);

    if (pf->seekable) {
        if (dest <= INT32_MAX) {
            // destination is within first 2GB
            if (fseek(fp, (long)dest, SEEK_SET))
                return -1;
        } else {
            int64_t offset = (int64_t)dest - (int64_t)pf->filepos;
            if (offset >= INT32_MIN && offset <= INT32_MAX) {
                // offset is within +/- 2GB of file start
                if (fseek(fp, (long)offset, SEEK_CUR))
                    return -1;
            } else {
                // absolute offset is more than 2GB
                if (offset < 0) {
                    fprintf(stderr, "error: backward seeking is limited to 2GB\n");
                    return -1;
                } else {
                    fprintf(stderr, "warning: forward seeking more than 2GB will be slow.\n");
                }
                slow_seek = 1;
            }
        }
        byteio_flush(&pf->io);
    }
    if (slow_seek) {
        // do forward-only seek by reading data to temp buffer
        uint64_t offset;
        uint8_t buf[1024];

        if (dest < pf->filepos)
            return -1;

        for (offset = dest - pf->filepos; offset > 1024; offset -= 1024)
            byteio_read(buf, 1024, &pf->io);

        byteio_read(buf, (int)offset, &pf->io);
    }
    pf->filepos = dest;

    return 0;
}

int
pcmfile_read_samples(PcmFile *pf, void *output, int num_samples)
{
    uint8_t *buffer;
    uint8_t *read_buffer;
    uint32_t bytes_needed, buffer_size;
    int nr, i, j, bps, nsmp;

    // check input and limit number of samples
    if (pf == NULL || pf->io.fp == NULL || output == NULL || pf->fmt_convert == NULL) {
        fprintf(stderr, "null input to pcmfile_read_samples()\n");
        return -1;
    }
    if (pf->block_align <= 0) {
        fprintf(stderr, "invalid block_align\n");
        return -1;
    }
    num_samples = MIN(num_samples, PCM_MAX_READ);

    // calculate number of bytes to read, being careful not to read past
    // the end of the data chunk
    bytes_needed = pf->block_align * num_samples;
    if (!pf->read_to_eof) {
        if ((pf->filepos + bytes_needed) >= (pf->data_start + pf->data_size)) {
            bytes_needed = (uint32_t)((pf->data_start + pf->data_size) - pf->filepos);
            num_samples = bytes_needed / pf->block_align;
        }
    }
    if (num_samples <= 0)
        return 0;

    // allocate temporary buffer for raw input data
    bps = pf->block_align / pf->channels;
    buffer_size = (bps != 3) ? bytes_needed : num_samples * sizeof(int32_t) * pf->channels;
    buffer = calloc(buffer_size + 1, 1);
    if (!buffer) {
        fprintf(stderr, "error allocating read buffer\n");
        return -1;
    }
    read_buffer = buffer + (buffer_size - bytes_needed);

    // read raw audio samples from input stream into temporary buffer
    nr = byteio_read(read_buffer, bytes_needed, &pf->io);
    if (nr <= 0) {
        free(buffer);
        return nr;
    }
    pf->filepos += nr;
    nr /= pf->block_align;
    nsmp = nr * pf->channels;

    // do any necessary conversion based on source_format and read_format.
    // also do byte swapping when necessary based on source audio and system
    // byte orders.
    switch (bps) {
    case 2:
        if (pf->order == PCM_NON_NATIVE_BYTE_ORDER) {
            uint16_t *buf16 = (uint16_t *)buffer;
            for (i = 0; i < nsmp; i++)
                buf16[i] = bswap_16(buf16[i]);
        }
        break;
    case 3:
        {
            int32_t *input = (int32_t*)buffer;
            int unused_bits = 32 - pf->bit_width;
            int32_t v;
            if (pf->order == PCM_NON_NATIVE_BYTE_ORDER) {
                for (i = 0, j = 0; i < nsmp*bps; i += bps, j++) {
                    v = bswap_32(*(uint32_t*)(read_buffer + i) << 8);
                    v <<= unused_bits; // clear unused high bits
                    v >>= unused_bits; // sign extend
                    input[j] = v;
                }
            } else {
                for (i = 0, j = 0; i < nsmp*bps; i += bps, j++) {
                    v = *(int32_t*)(read_buffer + i);
                    v <<= unused_bits; // clear unused high bits
                    v >>= unused_bits; // sign extend
                    input[j] = v;
                }
            }
        }
        break;
    case 4:
        if (pf->order == PCM_NON_NATIVE_BYTE_ORDER) {
            uint32_t *buf32 = (uint32_t *)buffer;
            for (i = 0; i < nsmp; i++)
                buf32[i] = bswap_32(buf32[i]);
        }
        break;
    case 8:
        if (pf->order == PCM_NON_NATIVE_BYTE_ORDER) {
            uint64_t *buf64 = (uint64_t *)buffer;
            for (i = 0; i < nsmp; i++)
                buf64[i] = bswap_64(buf64[i]);
        }
        break;
    }
    pf->fmt_convert(output, buffer, nsmp);

    // free temporary buffer
    free(buffer);

    return nr;
}

int
pcmfile_seek_samples(PcmFile *pf, int64_t offset, int whence)
{
    int64_t byte_offset;
    uint64_t newpos, fpos, dst, dsz;

    if (pf == NULL || pf->io.fp == NULL)
        return -1;
    if (pf->block_align <= 0)
        return -1;
    if (pf->filepos < pf->data_start)
        return -1;
    if (pf->data_size == 0)
        return 0;

    fpos = pf->filepos;
    dst = pf->data_start;
    dsz = pf->data_size;
    byte_offset = offset;
    byte_offset *= pf->block_align;

    // calculate new destination within file
    switch (whence) {
        case PCM_SEEK_SET:
            newpos = dst + CLIP(byte_offset, 0, (int64_t)dsz);
            break;
        case PCM_SEEK_CUR:
            newpos = fpos - MIN(-byte_offset, (int64_t)(fpos - dst));
            newpos = MIN(newpos, dst + dsz);
            break;
        case PCM_SEEK_END:
            newpos = dst + dsz - CLIP(byte_offset, 0, (int64_t)dsz);
            break;
        default: return -1;
    }

    // seek to the destination point
    if (pcmfile_seek_set(pf, newpos))
        return -1;

    return 0;
}

int
pcmfile_seek_time_ms(PcmFile *pf, int64_t offset, int whence)
{
    int64_t samples;
    if (pf == NULL)
        return -1;
    samples = offset * pf->sample_rate / 1000;
    return pcmfile_seek_samples(pf, samples, whence);
}

uint64_t
pcmfile_position(PcmFile *pf)
{
    uint64_t cur;

    if (pf == NULL)
        return -1;
    if (pf->block_align <= 0)
        return -1;
    if (pf->data_start == 0 || pf->data_size == 0)
        return 0;

    cur = (pf->filepos - pf->data_start) / pf->block_align;
    return cur;
}

uint64_t
pcmfile_position_time_ms(PcmFile *pf)
{
    return (pcmfile_position(pf) * 1000 / pf->sample_rate);
}
