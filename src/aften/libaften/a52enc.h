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
 * @file a52.h
 * A/52 encoder header
 */

#ifndef A52ENC_H
#define A52ENC_H

#include "common.h"

#include "a52.h"
#include "bitio.h"
#include "aften.h"
#include "exponent.h"
#include "filter.h"
#include "mdct.h"
#include "threading.h"
#include "window.h"
#include "a52dec.h"


typedef struct A52ThreadContext {
    struct A52Context *ctx;
    A52DecodeContext *dctx;
#ifndef NO_THREADS
    A52ThreadSync ts;
#endif
    ThreadState state;
    int thread_num;
    int framesize;

    AftenStatus status;
    A52Frame frame;
    BitWriter bw;
    uint8_t frame_buffer[A52_MAX_CODED_FRAME_SIZE];

    uint32_t bit_cnt;
    uint32_t sample_cnt;

    int last_quality;

    MDCTThreadContext mdct_tctx_512;
    MDCTThreadContext mdct_tctx_256;
} A52ThreadContext;

typedef struct A52Context {
    A52ThreadContext *tctx;
#ifndef NO_THREADS
    A52GlobalThreadSync ts;
    int (*prepare_work)(A52ThreadContext *tctx, const void *input_buffer, int count, int *info);
#endif
    int (*begin_process_frame)(A52ThreadContext *tctx);
    AftenEncParams params;
    AftenMetadata meta;
    void (*fmt_convert_from_src)(FLOAT dest[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME],
          const void *vsrc, int nch, int n);
    A52WindowFunctions winf;
    A52ExponentFunctions expf;

    int n_threads;
    int last_samples_count;
    int n_channels;
    int n_all_channels;
    int acmod;
    int lfe;
    int lfe_channel;
    int sample_rate;
    int halfratecod;
    int bsid;
    int fscod;
    int bsmod;
    int target_bitrate;
    int frmsizecod;
    int fixed_bwcode;

    FilterContext bs_filter[A52_MAX_CHANNELS];
    FilterContext dc_filter[A52_MAX_CHANNELS];
    FilterContext bw_filter[A52_MAX_CHANNELS];
    FilterContext lfe_filter;

    FLOAT last_samples[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME]; // 256 would be enough, but want to use converting functions
    FLOAT last_transient_samples[A52_MAX_CHANNELS][256];

    MDCTContext mdct_ctx_512;
    MDCTContext mdct_ctx_256;
} A52Context;

#endif /* A52ENC_H */
