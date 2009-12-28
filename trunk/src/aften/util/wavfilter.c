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
 * @file wavfilter.c
 * Console WAV File Filter Utility
 */

#include "common.h"

#include "pcm.h"
#include "filter.h"

static void
wav_filter(FLOAT *samples, int ch, int n, FilterContext *f)
{
    FLOAT *samples2_buffer = malloc(ch * n * sizeof(FLOAT));
    FLOAT **samples2 = malloc(ch * sizeof(FLOAT*));
    FLOAT *tmp = malloc(n * sizeof(FLOAT));
    int j, i, c;

    for (i = 0, j = 0; i < ch; i++, j += n)
        samples2[i] = samples2_buffer + j;

    for (i = 0, j = 0; i < n; i++) {
        for (c = 0; c < ch; c++, j++)
            samples2[c][i] = samples[j];
    }

    for (c = 0; c < ch; c++) {
        memcpy(tmp, samples2[c], n * sizeof(FLOAT));
        filter_run(&f[c], samples2[c], tmp, n);
    }

    for (i = 0, j = 0; i < n; i++) {
        for (c = 0; c < ch; c++, j++) {
            samples[j] = samples2[c][i];
            if (samples[j] > 1.0)
                samples[j] = 1.0;
            if (samples[j] < -1.0)
                samples[j] = -1.0;
        }
    }
    free(tmp);
    free(samples2_buffer);
}

static void
write2le(uint16_t v, FILE *ofp)
{
    fputc((v     ) & 0xFF, ofp);
    fputc((v >> 8) & 0xFF, ofp);
}

static void
write4le(uint32_t v, FILE *ofp)
{
    fputc((v      ) & 0xFF, ofp);
    fputc((v >>  8) & 0xFF, ofp);
    fputc((v >> 16) & 0xFF, ofp);
    fputc((v >> 24) & 0xFF, ofp);
}

static void
output_wav_header(FILE *ofp, PcmFile *wf)
{
    fwrite("RIFF", 1, 4, ofp);
    write4le(((uint32_t)wf->data_size + 36), ofp);
    fwrite("WAVE", 1, 4, ofp);
    fwrite("fmt ", 1, 4, ofp);
    write4le(16, ofp);
    write2le(WAVE_FORMAT_PCM, ofp);
    write2le(wf->channels, ofp);
    write4le(wf->sample_rate, ofp);
    write4le(wf->sample_rate * wf->channels * 2, ofp);
    write2le(wf->channels * 2, ofp);
    write2le(16, ofp);
    fwrite("data", 1, 4, ofp);
    write4le((uint32_t)wf->data_size, ofp);
}

static void
output_wav_data(FILE *ofp, FLOAT *samples, int ch, int n)
{
    int16_t s16[1];
    uint16_t *u16 = (uint16_t *)s16;
    int i;

    for (i = 0; i < n*ch; i++) {
        s16[0] = (int16_t)(samples[i] * 32767.0);
        write2le(*u16, ofp);
    }
}

static const char *usage = "usage: wavfilter <lp | hp> <cutoff> <in.wav> <out.wav>";

int
main(int argc, char **argv)
{
    FILE *ifp, *ofp;
    PcmFile pf;
    FLOAT *buf;
    int frame_size;
    int nr;
    int i;
    FilterContext f[6];
    int ftype=0;
    enum PcmSampleFormat read_format;

    if (argc != 5) {
        fprintf(stderr, "\n%s\n\n", usage);
        exit(1);
    }

    if (!strncmp(argv[1], "lp", 3)) {
        ftype = FILTER_TYPE_LOWPASS;
    } else if (!strncmp(argv[1], "hp", 3)) {
        ftype = FILTER_TYPE_HIGHPASS;
    } else {
        fprintf(stderr, "\n%s\n\n", usage);
        exit(1);
    }

    ifp = fopen(argv[3], "rb");
    if (!ifp) {
        fprintf(stderr, "cannot open input file\n");
        exit(1);
    }
    ofp = fopen(argv[4], "wb");
    if (!ofp) {
        fprintf(stderr, "cannot open output file\n");
        exit(1);
    }

#ifdef CONFIG_DOUBLE
    read_format = PCM_SAMPLE_FMT_DBL;
#else
    read_format = PCM_SAMPLE_FMT_FLT;
#endif

    if (pcmfile_init(&pf, ifp, read_format, PCM_FORMAT_WAVE)) {
        fprintf(stderr, "error initializing wav reader\n\n");
        exit(1);
    }
    output_wav_header(ofp, &pf);

    for (i = 0; i < pf.channels; i++) {
        int cutoff;
        f[i].type = (enum FilterType)ftype;
        f[i].cascaded = 1;
        cutoff = atoi(argv[2]);
        f[i].cutoff = (FLOAT)cutoff;
        f[i].samplerate = (FLOAT)pf.sample_rate;
        if (filter_init(&f[i], FILTER_ID_BUTTERWORTH_II)) {
            fprintf(stderr, "error initializing filter\n");
            exit(1);
        }
    }

    frame_size = 512;
    buf = calloc(frame_size * pf.channels, sizeof(FLOAT));

    nr = pcmfile_read_samples(&pf, buf, frame_size);
    while (nr > 0) {
        wav_filter(buf, pf.channels, nr, f);
        output_wav_data(ofp, buf, pf.channels, nr);
        nr = pcmfile_read_samples(&pf, buf, frame_size);
    }

    for (i = 0; i < pf.channels; i++)
        filter_close(&f[i]);

    free(buf);
    pcmfile_close(&pf);
    fclose(ifp);
    fclose(ofp);

    return 0;
}
