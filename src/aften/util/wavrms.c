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
 * @file wavrms.c
 * Console WAV File RMS/Dialog Normalization Utility
 */

#include "common.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "pcm.h"

static int
calculate_rms(FLOAT *samples, int ch, int n)
{
    FLOAT rms_all, rms_left, rms_right;
    int i;

    // Calculate RMS values
    if (ch == 1) {
        rms_all = 0.0;
        for (i = 0; i < n; i++) {
            rms_all += (samples[i] * samples[i]);
        }
        rms_all /= n;
    } else {
        rms_left = rms_right = 0.0;
        for (i = 0; i < n*ch; i += ch) {
            rms_left  += (samples[i  ] * samples[i  ]);
            rms_right += (samples[i+1] * samples[i+1]);
        }
        rms_left /= n;
        rms_right /= n;
        rms_all = (rms_left + rms_right) / FCONST(2.0);
    }

    // Convert to dB
    rms_all = FCONST(10.0) * AFT_LOG10(rms_all + FCONST(1e-10));
    return -((int)(rms_all + FCONST(0.5)));
}

static void
print_intro(FILE *stream)
{
    fprintf(stream, "\nWavRMS: utility program to calculate AC-3 dialnorm.\n"
                    "(c) 2006-2007 Justin Ruggles, et al.\n\n");
}

static void
print_usage(FILE *stream)
{
    fprintf(stream, "usage: wavrms <test.wav> [<start> [<end>]]\n"
                    "    use '-' to input from stdin.\n"
                    "    unit for start and end is seconds.\n"
                    "\n");
}

int
main(int argc, char **argv)
{
    FILE *fp;
    PcmFile pf;
    FLOAT *buf;
    uint32_t start_sec, end_sec;
    int frame_size, nr, rms;
    uint64_t avg_rms, avg_cnt;
    uint64_t time_ms;
    enum PcmSampleFormat read_format;

    /* open file */
    if (argc < 2 || argc > 4) {
        print_intro(stderr);
        print_usage(stderr);
        exit(1);
    }
    print_intro(stdout);
    if (argc == 2 && !strncmp(argv[1], "-h", 3)) {
        print_usage(stdout);
        return 0;
    }
    start_sec = 0;
    end_sec = UINT32_MAX;
    if (argc == 3 || argc == 4) {
        start_sec = MAX(atoi(argv[2]), 0);
        if (argc == 4) {
            end_sec = MAX(atoi(argv[3]), 0);
        }
        if (end_sec <= start_sec) {
            fprintf(stderr, "invalid time range\n");
            exit(1);
        }
    }
    if (!strncmp(argv[1], "-", 2)) {
#ifdef _WIN32
        _setmode(_fileno(stdin), _O_BINARY);
#endif
        fp = stdin;
    } else {
        fp = fopen(argv[1], "rb");
    }
    if (!fp) {
        fprintf(stderr, "cannot open file\n");
        exit(1);
    }

#ifdef CONFIG_DOUBLE
    read_format = PCM_SAMPLE_FMT_DBL;
#else
    read_format = PCM_SAMPLE_FMT_FLT;
#endif

    if (pcmfile_init(&pf, fp, read_format, PCM_FORMAT_WAVE)) {
        fprintf(stderr, "error initializing wav reader\n\n");
        exit(1);
    }
    frame_size = pf.sample_rate * 50 / 1000;
    // seek to start of time range
    pcmfile_seek_time_ms(&pf, start_sec*1000, PCM_SEEK_SET);

    buf = calloc(frame_size * pf.channels, sizeof(FLOAT));

    avg_rms = 31;
    avg_cnt = 1;
    time_ms = pcmfile_position_time_ms(&pf);
    nr = pcmfile_read_samples(&pf, buf, frame_size);
    while (nr > 0) {
        // check for end of time range
        if (time_ms > (end_sec*1000)) {
            break;
        }

        rms = calculate_rms(buf, pf.channels, nr);
        // use a reasonable dialog range
        if(rms < 40 && rms > 15) {
            avg_rms += rms;
            avg_cnt++;
        }

        time_ms = pcmfile_position_time_ms(&pf);
        nr = pcmfile_read_samples(&pf, buf, frame_size);
    }
    avg_rms /= avg_cnt;

    time_ms /= 1000;
    fprintf(stdout, "Time Range: %"PRIu64" to %"PRIu64" sec\n",
            MIN(start_sec, time_ms), time_ms);
    fprintf(stdout, "Dialnorm: -%d dB\n\n", (int)MIN(avg_rms, 31));

    free(buf);
    pcmfile_close(&pf);
    fclose(fp);

    return 0;
}
