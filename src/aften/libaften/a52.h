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
 * A/52 common header
 */

#ifndef A52_H
#define A52_H

#include "common.h"
#include "a52tab.h"
#include "aften-types.h"

#define AFTEN_VERSION "git"

#define A52_MAX_CHANNELS 6

#define A52_NUM_BLOCKS 6

/* exponent encoding strategy */
#define EXP_REUSE 0
#define EXP_NEW   1
#define EXP_D15   1
#define EXP_D25   2
#define EXP_D45   3

#define QUALITY(csnr, fsnr) (((csnr) << 4) | (fsnr))

extern const uint8_t log2tab[256];

static inline int
log2i(uint32_t v)
{
    int n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += log2tab[v];

    return n;
}

/** Delta bit allocation strategy */
typedef enum {
    DBA_REUSE = 0,
    DBA_NEW,
    DBA_NONE,
    DBA_RESERVED
} AC3DeltaStrategy;

typedef struct A52Block {
    FLOAT *input_samples[A52_MAX_CHANNELS]; /* 512 per ch */
    FLOAT *mdct_coef[A52_MAX_CHANNELS]; /* 256 per ch */
    FLOAT transient_samples[A52_MAX_CHANNELS][512];
    int block_num;
    int blksw[A52_MAX_CHANNELS];
    int dithflag[A52_MAX_CHANNELS];
    int dynrng;
    uint8_t exp[A52_MAX_CHANNELS][256];
    int16_t psd[A52_MAX_CHANNELS][256];
    int16_t mask[A52_MAX_CHANNELS][50];
    uint8_t exp_strategy[A52_MAX_CHANNELS];
    uint8_t nexpgrps[A52_MAX_CHANNELS];
    uint8_t grp_exp[A52_MAX_CHANNELS][85];
    uint8_t bap[A52_MAX_CHANNELS][256];
    uint16_t qmant[A52_MAX_CHANNELS][256];
    int fgaincod[A52_MAX_CHANNELS];
    int write_snr;
} A52Block;

typedef struct A52BitAllocParams {
    int fscod;
    int halfratecod;
    int fgain[A52_NUM_BLOCKS][A52_MAX_CHANNELS];
    int sgain, sdecay, fdecay, dbknee, floor;
    int cplfleak, cplsleak;
} A52BitAllocParams;

typedef struct A52Frame {
    int quality;
    int bit_rate;
    int bwcode;

    FLOAT input_audio[A52_MAX_CHANNELS][A52_SAMPLES_PER_FRAME];
    A52Block blocks[A52_NUM_BLOCKS];
    int frame_bits;
    int exp_bits;
    int mant_bits;
    unsigned int frame_size_min; // minimum frame size
    unsigned int frame_size;     // current frame size in words
    unsigned int frmsizecod;

    // bitrate allocation control
    int sgaincod, sdecaycod, fdecaycod, dbkneecod, floorcod;
    A52BitAllocParams bit_alloc;
    int csnroffst;
    int fsnroffst;
    int ncoefs[A52_MAX_CHANNELS];
    int expstr_set[A52_MAX_CHANNELS];
    uint8_t rematflg[4];
} A52Frame;

void a52_common_init(void);

/**
 * Calculates the log power-spectral density of the input signal.
 * This gives a rough estimate of signal power in the frequency domain by using
 * the spectral envelope (exponents).  The psd is also separately grouped
 * into critical bands for use in the calculating the masking curve.
 * 128 units in psd = -6 dB.  The dbknee parameter in AC3BitAllocParameters
 * determines the reference level.
 *
 * @param[in]  exp        frequency coefficient exponents
 * @param[in]  start      starting bin location
 * @param[in]  end        ending bin location
 * @param[out] psd        signal power for each frequency bin
 * @param[out] band_psd   signal power for each critical band
 */
void a52_bit_alloc_calc_psd(uint8_t *exp, int start, int end, int16_t *psd,
                               int16_t *band_psd);

/**
 * Calculates the masking curve.
 * First, the excitation is calculated using parameters in \p s and the signal
 * power in each critical band.  The excitation is compared with a predefined
 * hearing threshold table to produce the masking curve.  If delta bit
 * allocation information is provided, it is used for adjusting the masking
 * curve, usually to give a closer match to a better psychoacoustic model.
 *
 * @param[in]  s            adjustable bit allocation parameters
 * @param[in]  band_psd     signal power for each critical band
 * @param[in]  start        starting bin location
 * @param[in]  end          ending bin location
 * @param[in]  fast_gain    fast gain (estimated signal-to-mask ratio)
 * @param[in]  dba_mode     delta bit allocation mode (none, reuse, or new)
 * @param[in]  dba_nsegs    number of delta segments
 * @param[in]  dba_offsets  location offsets for each segment
 * @param[in]  dba_lengths  length of each segment
 * @param[in]  dba_values   delta bit allocation for each segment
 * @param[out] mask         calculated masking curve
 */
void a52_bit_alloc_calc_mask(A52BitAllocParams *s, int16_t *band_psd,
                                int start, int end, int fast_gain,
                                int dba_mode, int dba_nsegs, uint8_t *dba_offsets,
                                uint8_t *dba_lengths, uint8_t *dba_values,
                                int16_t *mask);

/**
 * Calculates bit allocation pointers.
 * The SNR is the difference between the masking curve and the signal.  AC-3
 * uses this value for each frequency bin to allocate bits.  The \p snroffset
 * parameter is a global adjustment to the SNR for all bins.
 *
 * @param[in]  mask       masking curve
 * @param[in]  psd        signal power for each frequency bin
 * @param[in]  start      starting bin location
 * @param[in]  end        ending bin location
 * @param[in]  snr_offset SNR adjustment
 * @param[in]  floor      noise floor
 * @param[out] bap        bit allocation pointers
 */
void a52_bit_alloc_calc_bap(int16_t *mask, int16_t *psd, int start, int end,
                               int snr_offset, int floor, uint8_t *bap);

#endif /* A52_H */
