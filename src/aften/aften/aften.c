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
 * @file aften.c
 * Commandline encoder frontend
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "aften.h"
#include "pcm.h"
#include "helptext.h"
#include "opts.h"

static const int acmod_to_ch[8] = { 2, 1, 2, 3, 3, 4, 4, 5 };

static const char *acmod_str[8] = {
    "dual mono (1+1)", "mono (1/0)", "stereo (2/0)",
    "3/0", "2/1", "3/1", "2/2", "3/2"
};

static void
print_intro(FILE *out)
{
    const char *vers = aften_get_version();
    fprintf(out, "\nAften: A/52 audio encoder\n"
                 "Version %s\n"
                 "(c) 2006-2009 Justin Ruggles, Prakash Punnoor, et al.\n\n",
                 vers);
}

static void
print_simd_in_use(FILE *out, AftenSimdInstructions *simd_instructions)
{
    fprintf(out, "SIMD usage:");
    if (simd_instructions->mmx)
        fprintf(out, " MMX");
    if (simd_instructions->sse)
        fprintf(out, " SSE");
    if (simd_instructions->sse2)
        fprintf(out, " SSE2");
    if (simd_instructions->sse3)
        fprintf(out, " SSE3");
    if (simd_instructions->ssse3)
        fprintf(out, " SSSE3");
    if (simd_instructions->amd_3dnow)
        fprintf(out, " 3DNOW");
    if (simd_instructions->amd_3dnowext)
        fprintf(out, " 3DNOWEXT");
    if (simd_instructions->amd_sse_mmx)
        fprintf(out, " SSE-MMX");
    if (simd_instructions->altivec)
        fprintf(out, " Altivec");
    fprintf(out, "\n");
}

int
main(int argc, char **argv)
{
    void (*aften_remap)(void *samples, int n, int ch,
                        A52SampleFormat fmt, int acmod) = NULL;
    uint8_t *frame = NULL;
    FLOAT *fwav = NULL;
    int nr, fs, err;
    FILE *ifp[A52_NUM_SPEAKERS];
    FILE *ofp = NULL;
    PcmContext pf;
    CommandOptions opts;
    AftenContext s;
    uint32_t samplecount, bytecount, t0, t1, percent;
    FLOAT kbps, qual, bw;
    int frame_cnt;
    int input_file_format;
    enum PcmSampleFormat read_format;
    /* update output every 200ms */
    clock_t update_clock_span = (clock_t)(0.2f * CLOCKS_PER_SEC);
    clock_t current_clock;
    clock_t last_update_clock = clock() - update_clock_span;
    int ret_val = 0;
    int i;

    opts.s = &s;
    aften_set_defaults(&s);
    err = parse_commandline(argc, argv, &opts);
    if (err) {
        if (err == 1) {
            print_intro(stderr);
            print_usage(stderr);
            return 1;
        } else {
            print_intro(stdout);
            if (err == 2) {
                print_help(stdout);
            } else if (err == 3) {
                print_long_help(stdout);
            }
            return 0;
        }
    }

    if (s.verbose > 0) {
        print_intro(stderr);
    }

    memset(ifp, 0, A52_NUM_SPEAKERS * sizeof(FILE *));
    for (i = 0; i < opts.num_input_files; i++) {
        if (!strncmp(opts.infile[i], "-", 2)) {
#ifdef _WIN32
            _setmode(_fileno(stdin), _O_BINARY);
#endif
            ifp[i] = stdin;
        } else {
            ifp[i] = fopen(opts.infile[i], "rb");
            if (!ifp[i]) {
                fprintf(stderr, "error opening input file: %s\n", opts.infile[i]);
                goto error_end;
            }
        }
    }

#ifdef CONFIG_DOUBLE
    read_format = PCM_SAMPLE_FMT_DBL;
#else
    read_format = PCM_SAMPLE_FMT_FLT;
#endif

    // initialize pcmfile using input
    input_file_format = PCM_FORMAT_UNKNOWN;
    if (opts.raw_input)
        input_file_format = PCM_FORMAT_RAW;
    if (pcm_init(&pf, opts.num_input_files, ifp, read_format, input_file_format)) {
        fprintf(stderr, "invalid input file(s)\n");
        goto error_end;
    }
    if (opts.read_to_eof)
        pcm_set_read_to_eof(&pf, 1);
    if (opts.raw_input) {
        pcm_set_source_params(&pf, opts.raw_ch, opts.raw_fmt,
                                  opts.raw_order, opts.raw_sr);
    }

    // print wav info to console
    if (s.verbose > 0) {
        fprintf(stderr, "input format: ");
        if (opts.num_input_files > 1)
            fprintf(stderr, "\n");
        pcm_print(&pf, stderr);
    }

    // if acmod is given on commandline, determine lfe from number of channels
    if (s.acmod >= 0) {
        int ch = acmod_to_ch[s.acmod];
        if (ch == pf.channels) {
            if (s.lfe < 0) {
                s.lfe = 0;
            } else {
                if (s.lfe != 0) {
                    fprintf(stderr, "acmod and lfe do not match number of channels\n");
                    goto error_end;
                }
            }
        } else if (ch == (pf.channels - 1)) {
            if (s.lfe < 0) {
                s.lfe = 1;
            } else {
                if (s.lfe != 1) {
                    fprintf(stderr, "acmod and lfe do not match number of channels\n");
                    goto error_end;
                }
            }
        } else {
            fprintf(stderr, "acmod does not match number of channels\n");
            goto error_end;
        }
    } else {
        // if acmod is not given on commandline, determine from WAVE file
        int ch = pf.channels;
        if (s.lfe >= 0) {
            if (s.lfe == 0 && ch == 6) {
                fprintf(stderr, "cannot encode 6 channels w/o LFE\n");
                goto error_end;
            } else if (s.lfe == 1 && ch == 1) {
                fprintf(stderr, "cannot encode LFE channel only\n");
                goto error_end;
            }
            if (s.lfe) {
                pf.ch_mask |= 0x08;
            }
        }
        if (aften_wav_channels_to_acmod(ch, pf.ch_mask, &s.acmod, &s.lfe)) {
            fprintf(stderr, "mismatch in channels, acmod, and lfe params\n");
            goto error_end;
        }
    }
    // set some encoding parameters using wav info
    s.channels = pf.channels;
    s.samplerate = pf.sample_rate;
#ifdef CONFIG_DOUBLE
    s.sample_format = A52_SAMPLE_FMT_DBL;
#else
    s.sample_format = A52_SAMPLE_FMT_FLT;
#endif

    // open output file
    if (!strncmp(opts.outfile, "-", 2)) {
#ifdef _WIN32
        _setmode(_fileno(stdout), _O_BINARY);
#endif
        ofp = stdout;
    } else {
        ofp = fopen(opts.outfile, "wb");
        if (!ofp) {
            fprintf(stderr, "error opening output file: %s\n", opts.outfile);
            goto error_end;
        }
    }

    // print ac3 info to console
    if (s.verbose > 0) {
        fprintf(stderr, "output format: %d Hz %s", s.samplerate,
                acmod_str[s.acmod]);
        if (s.lfe) {
            fprintf(stderr, " + LFE");
        }
        fprintf(stderr, "\n\n");
    }

    // allocate memory for coded frame and sample buffer
    frame = calloc(A52_MAX_CODED_FRAME_SIZE, 1);
    fwav = calloc(A52_SAMPLES_PER_FRAME * s.channels, sizeof(FLOAT));
    if (frame == NULL || fwav == NULL)
        goto error_end;

    samplecount = bytecount = t0 = t1 = percent = 0;
    qual = bw = 0.0;
    frame_cnt = 0;
    fs = 0;
    nr = 0;

    if (opts.chmap == 0)
        aften_remap = aften_remap_wav_to_a52;
    else if (opts.chmap == 2)
        aften_remap = aften_remap_mpeg_to_a52;

    // Don't pad start with zero samples, use input audio instead.
    if (!opts.pad_start) {
        int diff;
        nr = pcm_read_samples(&pf, fwav, 256);
        diff = 256 - nr;
        if (diff > 0) {
            memmove(fwav + diff * s.channels, fwav, nr);
            memset(fwav, 0, diff * s.channels * sizeof(FLOAT));
        }
        if (aften_remap)
            aften_remap(fwav + diff, nr, s.channels, s.sample_format, s.acmod);

        s.initial_samples = fwav;
    }
    // initialize encoder
    if (aften_encode_init(&s)) {
        fprintf(stderr, "error initializing encoder\n");
        goto error_end;
    }
    // print SIMD instructions used
    print_simd_in_use(stderr, &s.system.wanted_simd_instructions);

    // print number of threads used
    fprintf(stderr, "Threads: %i\n\n", s.system.n_threads);

    do {
        nr = pcm_read_samples(&pf, fwav, A52_SAMPLES_PER_FRAME);
        if (aften_remap)
            aften_remap(fwav, nr, s.channels, s.sample_format, s.acmod);

        fs = aften_encode_frame(&s, frame, fwav, nr);

        if (fs < 0) {
            fprintf(stderr, "Error encoding frame %d\n", frame_cnt);
            break;
        } else if (fs > 0) {
            if (s.verbose > 0) {
                samplecount += A52_SAMPLES_PER_FRAME;
                bytecount += fs;
                qual += s.status.quality;
                bw += s.status.bwcode;
                if (s.verbose == 1) {
                    current_clock = clock();
                    if (current_clock - last_update_clock >= update_clock_span) {
                        t1 = samplecount / pf.sample_rate;
                        if (frame_cnt > 0 && (t1 > t0 || samplecount >= pf.samples)) {
                            kbps = (bytecount * FCONST(8.0) * pf.sample_rate) /
                                (FCONST(1000.0) * samplecount);
                            percent = 0;
                            if (pf.samples > 0) {
                                percent = (uint32_t)((samplecount * FCONST(100.0)) /
                                                        pf.samples);
                                percent = CLIP(percent, 0, 100);
                            }
                            fprintf(stderr, "\rprogress: %3u%% | q: %4.1f | "
                                    "bw: %2.1f | bitrate: %4.1f kbps ",
                                    percent, (qual / (frame_cnt+1)),
                                    (bw / (frame_cnt+1)), kbps);
                        }
                        t0 = t1;
                        last_update_clock = current_clock;
                    }
                } else if (s.verbose == 2) {
                    fprintf(stderr, "frame: %7d | q: %4d | bw: %2d | bitrate: %3d kbps\n",
                            frame_cnt, s.status.quality, s.status.bwcode,
                            s.status.bit_rate);
                }
            }
            fwrite(frame, 1, fs, ofp);
            frame_cnt++;
        }
    } while (nr > 0 || fs > 0 || !frame_cnt);

    if (s.verbose >= 1) {
        if (samplecount > 0) {
            kbps = (bytecount * FCONST(8.0) * pf.sample_rate) / (FCONST(1000.0) * samplecount);
        } else {
            kbps = 0;
        }
        frame_cnt = MAX(frame_cnt, 1);
        if (s.verbose == 1) {
            fprintf(stderr, "\rprogress: 100%% | q: %4.1f | bw: %2.1f | bitrate: %4.1f kbps\n\n",
                    (qual / frame_cnt), (bw / frame_cnt), kbps);
        } else if (s.verbose == 2) {
            fprintf(stderr, "\n");
            fprintf(stderr, "average quality:   %4.1f\n", (qual / frame_cnt));
            fprintf(stderr, "average bandwidth: %2.1f\n", (bw / frame_cnt));
            fprintf(stderr, "average bitrate:   %4.1f kbps\n\n", kbps);
        }
    }
    goto end;
error_end:
    ret_val = 1;
end:
    if (fwav)
        free(fwav);

    if (frame)
        free(frame);

        pcm_close(&pf);
    for (i = 0; i < opts.num_input_files; i++) {
        if (ifp[i])
            fclose(ifp[i]);
    }
    if (ofp)
        fclose(ofp);

    if (aften_encode_close(&s))
        return 1;

    return ret_val;
}
