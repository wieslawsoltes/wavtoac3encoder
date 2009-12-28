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
 * @file pcm.c
 * raw PCM multi-file decoder
 */

#include "pcm.h"

int
pcm_init(PcmContext *pc, int num_files, FILE **fp_list, enum PcmSampleFormat read_format,
         int file_format)
{
    int i;
    uint64_t samples;

    // check parameters
    if (num_files < 1 || num_files > PCM_MAX_CHANNELS) {
        fprintf(stderr, "invalid number of files: %d. must be > 0\n", num_files);
        return -1;
    }
    if (read_format < PCM_SAMPLE_FMT_U8 || read_format > PCM_SAMPLE_FMT_DBL) {
        fprintf(stderr, "invalid read format: %d\n", read_format);
        return -1;
    }
    if (file_format < PCM_FORMAT_UNKNOWN || file_format > PCM_FORMAT_CAFF) {
        fprintf(stderr, "invalid file format: %d\n", file_format);
        return -1;
    }

    memset(pc, 0, sizeof(PcmContext));
    samples = 0;
    for (i = 0; i < num_files; i++) {
        PcmFile *pf = &pc->pcm_file[i];
        if (pcmfile_init(pf, fp_list[i], read_format, file_format)) {
            fprintf(stderr, "error initializing file #%d\n", i);
            pcm_close(pc);
            return -1;
        }
        if (num_files > 1 && pf->channels != 1) {
            fprintf(stderr, "all files must be mono when using multiple input files\n");
            pcm_close(pc);
            return -1;
        }
        samples = MAX(samples, pc->pcm_file[i].samples);
    }

    pc->samples = samples;
    pc->num_files = num_files;
    pc->read_format = read_format;
    if (num_files == 1) {
        pc->channels = pc->pcm_file[0].channels;
        pc->ch_mask = pc->pcm_file[0].ch_mask;
    } else {
        pc->channels = num_files;
        pc->ch_mask = pcm_get_default_ch_mask(pc->channels);
    }
    pcm_set_sample_rate(pc, pc->pcm_file[0].sample_rate);

    return 0;
}

void
pcm_close(PcmContext *pc)
{
    int i;
    for (i = 0; i < pc->num_files; i++)
        pcmfile_close(&pc->pcm_file[i]);
    memset(pc, 0, sizeof(PcmContext));
}

void
pcm_set_source_format(PcmContext *pc, enum PcmSampleFormat fmt)
{
    int i;
    for (i = 0; i < pc->num_files; i++)
        pcmfile_set_source_format(&pc->pcm_file[i], fmt);
}

void
pcm_set_source_params(PcmContext *pc, int ch, enum PcmSampleFormat fmt, int order, int sr)
{
    int i;
    if (pc->num_files > 1 && ch != 1) {
        fprintf(stderr, "all files must be mono when using multiple input files\n");
        return;
    }
    for (i = 0; i < pc->num_files; i++)
        pcmfile_set_source_params(&pc->pcm_file[i], ch, fmt, order, sr);
    pc->sample_rate = sr;
    if (pc->num_files == 1) {
        pc->channels = pc->pcm_file[0].channels;
        pc->ch_mask = pcm_get_default_ch_mask(pc->channels);
    }
}

void
pcm_set_sample_rate(PcmContext *pc, int sample_rate)
{
    int i;
    pc->sample_rate = sample_rate;
    for (i = 0; i < pc->num_files; i++)
        pc->pcm_file[i].sample_rate = sample_rate;
}

void
pcm_set_read_to_eof(PcmContext *pc, int read_to_eof)
{
    int i;
    pc->read_to_eof = read_to_eof;
    for (i = 0; i < pc->num_files; i++)
        pc->pcm_file[i].read_to_eof = read_to_eof;
}

void
pcm_set_read_format(PcmContext *pc, enum PcmSampleFormat read_format)
{
    int i;
    pc->read_format = read_format;
    for (i = 0; i < pc->num_files; i++)
        pcmfile_set_read_format(&pc->pcm_file[i], read_format);
}

void
pcm_print(PcmContext *pc, FILE *st)
{
    int i;
    for (i = 0; i < pc->num_files; i++)
        pcmfile_print(&pc->pcm_file[i], st);
}

static const uint8_t sample_sizes[8] = { 1, 1, 2, 4, 4, 4, 4, 8 };

#define CHANNEL_INTERLEAVE_COMMON(DATA_TYPE) \
{ \
    DATA_TYPE *input = (DATA_TYPE *)buf; \
    DATA_TYPE *output = buffer; \
    int j, k; \
\
    for (i = 0, k = 0; i < samples_read; i++) \
        for (j = 0; j < pc->num_files; j++, k++) \
            output[k] = input[num_samples * j + i]; \
}

int
pcm_read_samples(PcmContext *pc, void *buffer, int num_samples)
{
    int i;
    int samples_read, chansize;
    uint8_t *buf;
    uint8_t *buf_ptr;

    if (pc->num_files == 1)
        return pcmfile_read_samples(&pc->pcm_file[0], buffer, num_samples);

    /* allocate buffer */
    chansize = num_samples * sample_sizes[pc->read_format];
    buf = calloc(1, chansize * pc->channels);
    if (!buf)
        return -1;

    /* read samples from each channel */
    samples_read = 0;
    buf_ptr = buf;
    for (i = 0; i < pc->num_files; i++) {
        int nr = pcmfile_read_samples(&pc->pcm_file[i], buf_ptr, num_samples);
        if (nr < 0) {
            free(buf);
            return -1;
        }
        samples_read = MAX(samples_read, nr);
        buf_ptr += chansize;
    }

    /* interleave samples to create multichannel */
    switch (pc->read_format) {
        case PCM_SAMPLE_FMT_U8:
            CHANNEL_INTERLEAVE_COMMON(uint8_t)
            break;
        case PCM_SAMPLE_FMT_S8:
            CHANNEL_INTERLEAVE_COMMON(int8_t)
            break;
        case PCM_SAMPLE_FMT_S16:
            CHANNEL_INTERLEAVE_COMMON(int16_t)
            break;
        case PCM_SAMPLE_FMT_S20:
        case PCM_SAMPLE_FMT_S24:
        case PCM_SAMPLE_FMT_S32:
            CHANNEL_INTERLEAVE_COMMON(int32_t)
            break;
        case PCM_SAMPLE_FMT_FLT:
            CHANNEL_INTERLEAVE_COMMON(float)
            break;
        case PCM_SAMPLE_FMT_DBL:
            CHANNEL_INTERLEAVE_COMMON(double)
            break;
    }

    free(buf);
    return samples_read;
}
