/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
 *                    Prakash Punnoor <prakash@punnoor.de>
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
 * @file bitalloc.c
 * A/52 bit allocation
 */

#include "a52enc.h"
#include "bitalloc.h"

/**
 * A52 bit allocation preparation to speed up matching left bits.
 * This generates the power-spectral densities and the masking curve based on
 * the mdct coefficient exponents and bit allocation parameters.
 */
static void
a52_bit_allocation_prepare(A52BitAllocParams *s,
                   uint8_t *exp, int16_t *psd, int16_t *mask,
                   int fgain, int start, int end)
//                 int deltbae,int deltnseg, uint8_t *deltoffst,
//                 uint8_t *deltlen, uint8_t *deltba)
{
    int16_t bndpsd[50]; // power spectral density for critical bands

    a52_bit_alloc_calc_psd(exp, start, end, psd, bndpsd);

    a52_bit_alloc_calc_mask(s, bndpsd, start, end, fgain,
                            -1, -1, NULL, NULL, NULL,/* delta bit allocation not used */
                            mask);
}

/**
 * Calculate the size in bits taken by the mantissas.
 * This is determined solely by the bit allocation pointers.
 */
static int
compute_mantissa_size(int mant_cnt[5], uint8_t *bap, int ncoefs)
{
    int bits, b, i;

    bits = 0;
    for (i = 0; i < ncoefs; i++) {
        b = bap[i];
        if (b <= 4) {
            // bap=1 to bap=4 will be counted in compute_mantissa_size_final
            ++mant_cnt[b];
        } else if (b <= 13) {
            // bap=5 to bap=13 use (bap-1) bits
            bits += b-1;
        } else {
            // bap=14 uses 14 bits and bap=15 uses 16 bits
            bits += 14 + ((b-14)<<1);
        }
    }
    return bits;
}

/** Finalize the mantissa bit count by adding in the grouped mantissas */
static int
compute_mantissa_size_final(int mant_cnt[5])
{
    // bap=1 : 3 mantissas in 5 bits
    int bits = (mant_cnt[1] / 3) * 5;
    // bap=2 : 3 mantissas in 7 bits
    // bap=4 : 2 mantissas in 7 bits
    bits += ((mant_cnt[2] / 3) + (mant_cnt[4] >> 1)) * 7;
    // bap=3 : each mantissa is 3 bits
    bits += mant_cnt[3] * 3;
    return bits;
}

/* call to prepare bit allocation */
static void
bit_alloc_prepare(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *block;
    int blk, ch;

    for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
        block = &frame->blocks[blk];
        for (ch = 0; ch < ctx->n_all_channels; ch++) {
            // We don't have to run the bit allocation when reusing exponents
            if (block->exp_strategy[ch] != EXP_REUSE) {
                a52_bit_allocation_prepare(&frame->bit_alloc,
                               block->exp[ch], block->psd[ch], block->mask[ch],
                               frame->bit_alloc.fgain[blk][ch],
                               0, frame->ncoefs[ch]);
//                             2, 0, NULL, NULL, NULL);
            }
        }
    }
}

/**
 * Run the bit allocation routine using the given snroffset values.
 * Returns number of mantissa bits used.
 */
static int
bit_alloc(A52ThreadContext *tctx, int snroffst)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *block;
    int mant_cnt[5];
    int blk, ch;
    int bits;

    bits = 0;
    snroffst = (snroffst << 2) - 960;

    for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
        block = &frame->blocks[blk];
        // initialize grouped mantissa counts. these are set so that they are
        // padded to the next whole group size when bits are counted in
        // compute_mantissa_size_final
        mant_cnt[0] = mant_cnt[3] = 0;
        mant_cnt[1] = mant_cnt[2] = 2;
        mant_cnt[4] = 1;
        for (ch = 0; ch < ctx->n_all_channels; ch++) {
            // Currently the encoder is setup so that the only bit allocation
            // parameter which varies across blocks within a frame is the
            // exponent values.  We can take advantage of that by reusing the
            // bit allocation pointers whenever we reuse exponents.
            if (block->exp_strategy[ch] == EXP_REUSE) {
                memcpy(block->bap[ch], frame->blocks[blk-1].bap[ch], 256);
            } else {
                a52_bit_alloc_calc_bap(block->mask[ch], block->psd[ch], 0, frame->ncoefs[ch],
                                       snroffst, frame->bit_alloc.floor, block->bap[ch]);
            }
            bits += compute_mantissa_size(mant_cnt, block->bap[ch], frame->ncoefs[ch]);

        }
        bits += compute_mantissa_size_final(mant_cnt);
    }

    return bits;
}

/** Counts all frame bits except for mantissas and exponents */
static void
count_frame_bits(A52ThreadContext *tctx)
{
    static int frame_bits_inc[8] = { 8, 0, 2, 2, 2, 4, 2, 4 };
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *block;
    int blk, ch;
    int frame_bits;

    frame_bits = 0;

    // header size
    frame_bits += 65;
    frame_bits += frame_bits_inc[ctx->acmod];
    if (ctx->meta.xbsi1e)
        frame_bits += 14;
    if (ctx->meta.xbsi2e)
        frame_bits += 14;

    // audio blocks
    for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
        block = &frame->blocks[blk];
        frame_bits += ctx->n_channels; // blksw
        frame_bits += ctx->n_channels; // dithflg
        frame_bits += 1 + (ctx->acmod == A52_ACMOD_DUAL_MONO); // dynrnge, dynrng2e
        if (ctx->params.dynrng_profile != DYNRNG_PROFILE_NONE) {
            frame_bits += 8; // dynrng
            if (ctx->acmod == A52_ACMOD_DUAL_MONO)
                frame_bits += 8; // dynrng2
        }
        frame_bits++; // cplstre
        if (!blk) {
            frame_bits++; // cplinu
        }
        if (ctx->acmod == 2) {
            frame_bits++; // rematstr
            if (!blk)
                frame_bits += 4; // rematflg
        }
        frame_bits += 2 * ctx->n_channels; // chexpstr
        if (ctx->lfe)
            frame_bits++; // lfeexpstr
        for (ch = 0; ch < ctx->n_channels; ch++) {
            if (block->exp_strategy[ch] != EXP_REUSE) {
                frame_bits += 6; // chbwcod
                frame_bits += 2; // gainrng
            }
        }
        frame_bits++; // baie
        if (!blk) {
            // sdcycod[2], fdcycod[2], sgaincod[2], dbpbcod[2], floorcod[3]
            frame_bits += 2 + 2 + 2 + 2 + 3;
        }
        frame_bits++; // snr
        if (block->write_snr) {
            frame_bits += 6; // csnroffset
            frame_bits += ctx->n_all_channels * 4; // fsnroffset
            frame_bits += ctx->n_all_channels * 3; // fgaincod
        }
        frame_bits++; // delta
        frame_bits++; // skip
    }

    // auxdatae, crcrsv
    frame_bits += 2;

    // CRC
    frame_bits += 16;

    frame->frame_bits = frame_bits;
}

/**
 * Calculates the snroffset values which, when used, keep the size of the
 * encoded data within a fixed frame size.
 */
static int
cbr_bit_allocation(A52ThreadContext *tctx, int prepare)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    int current_bits, avail_bits, leftover;
    int snroffst=0;

    current_bits = frame->frame_bits + frame->exp_bits;
    avail_bits = (16 * frame->frame_size) - current_bits;

    if (prepare)
        bit_alloc_prepare(tctx);

    // starting point
    if (ctx->params.encoding_mode == AFTEN_ENC_MODE_VBR)
        snroffst = ctx->params.quality;
    else if (ctx->params.encoding_mode == AFTEN_ENC_MODE_CBR)
        snroffst = tctx->last_quality;
    leftover = avail_bits - bit_alloc(tctx, snroffst);

    if (ctx->params.bitalloc_fast) {
        // fast bit allocation
        int leftover0, leftover1, snr0, snr1;
        snr0 = snr1 = snroffst;
        leftover0 = leftover1 = leftover;
        if (leftover != 0) {
            if (leftover > 0) {
                while (leftover1 > 0 && snr1+16 <= 1023) {
                    snr0 = snr1;
                    leftover0 = leftover1;
                    snr1 += 16;
                    leftover1 = avail_bits - bit_alloc(tctx, snr1);
                }
            } else {
                while (leftover0 < 0 && snr0-16 >= 0) {
                    snr1 = snr0;
                    leftover1 = leftover0;
                    snr0 -= 16;
                    leftover0 = avail_bits - bit_alloc(tctx, snr0);
                }
            }
        }
        if (snr0 != snr1) {
            snroffst = snr0;
            leftover = avail_bits - bit_alloc(tctx, snroffst);
        }
    } else {
        // take up to 3 jumps based on estimated distance from optimal
        if (leftover < -400) {
            snroffst += (leftover / (16 * ctx->n_channels));
            leftover = avail_bits - bit_alloc(tctx, snroffst);
        }
        if (leftover > 400) {
            snroffst += (leftover / (24 * ctx->n_channels));
            leftover = avail_bits - bit_alloc(tctx, snroffst);
        }
        if (leftover < -200) {
            snroffst += (leftover / (40 * ctx->n_channels));
            leftover = avail_bits - bit_alloc(tctx, snroffst);
        }
        // adjust snroffst until leftover <= -100
        while (leftover > -100) {
            snroffst += (10 / ctx->n_channels);
            if (snroffst > 1023) {
                snroffst = 1023;
                leftover = avail_bits - bit_alloc(tctx, snroffst);
                break;
            }
            leftover = avail_bits - bit_alloc(tctx, snroffst);
        }
        // adjust snroffst until leftover is positive
        while (leftover < 0 && snroffst > 0) {
            snroffst--;
            leftover = avail_bits - bit_alloc(tctx, snroffst);
        }
    }

    frame->mant_bits = avail_bits - leftover;
    if (leftover < 0) {
        fprintf(stderr, "bitrate: %d kbps too small\n", frame->bit_rate);
        return -1;
    }

    // set encoding parameters
    frame->csnroffst = snroffst >> 4;
    frame->fsnroffst = snroffst & 0xF;
    frame->quality = snroffst;
    tctx->last_quality = snroffst;

    return 0;
}

/**
 * Finds the frame size which will hold all of the data when using an
 * snroffset value as determined by the user-selected quality setting.
 */
static int
vbr_bit_allocation(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    int i;
    int frame_size;
    int quality;
    int frame_bits, current_bits;

    current_bits = frame->frame_bits + frame->exp_bits;
    quality = ctx->params.quality;

    bit_alloc_prepare(tctx);
    // find an A52 frame size that can hold the data.
    frame_size = 0;
    frame_bits = current_bits + bit_alloc(tctx, quality);
    for (i = 0; i <= ctx->frmsizecod; i++) {
        frame_size = a52_frame_size_tab[i][ctx->fscod];
        if (frame_size >= frame_bits)
            break;
    }
    i = MIN(i, ctx->frmsizecod);
    frame->bit_rate = a52_bitrate_tab[i/2] >> ctx->halfratecod;

    frame->frmsizecod = i;
    frame->frame_size = frame_size / 16;
    frame->frame_size_min = frame->frame_size;

    // run CBR bit allocation.
    // this will increase snroffst to make optimal use of the frame bits.
    // also it will lower snroffst if vbr frame won't fit in largest frame.
    return cbr_bit_allocation(tctx, 0);
}

/**
 * Loads the bit allocation parameters and counts fixed frame bits.
 */
static void
start_bit_allocation(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *block;
    int blk, ch;

    // read bit allocation table values
    frame->bit_alloc.fscod = ctx->fscod;
    frame->bit_alloc.halfratecod = ctx->halfratecod;
    frame->bit_alloc.sdecay = a52_slow_decay_tab[frame->sdecaycod] >> ctx->halfratecod;
    frame->bit_alloc.fdecay = a52_fast_decay_tab[frame->fdecaycod] >> ctx->halfratecod;
    frame->bit_alloc.sgain = a52_slow_gain_tab[frame->sgaincod];
    frame->bit_alloc.dbknee = a52_db_per_bit_tab[frame->dbkneecod];
    frame->bit_alloc.floor = a52_floor_tab[frame->floorcod];

    // set fast gain based on exponent strategy
    for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
        block = &frame->blocks[blk];
        block->write_snr = 0;
        for (ch = 0; ch < ctx->n_all_channels; ch++) {
            if (block->exp_strategy[ch] != EXP_REUSE) {
                block->fgaincod[ch] = 4 - block->exp_strategy[ch];
                block->write_snr |= !blk || (block->fgaincod[ch] != frame->blocks[blk-1].fgaincod[ch]);
            } else {
                block->fgaincod[ch] = frame->blocks[blk-1].fgaincod[ch];
            }
            frame->bit_alloc.fgain[blk][ch] = a52_fast_gain_tab[block->fgaincod[ch]];
        }
    }

    count_frame_bits(tctx);
}

/** estimated number of bits used for a mantissa, indexed by bap value. */
static FLOAT mant_est_tab[16] = {
    FCONST( 0.000), FCONST( 1.667),
    FCONST( 2.333), FCONST( 3.000),
    FCONST( 3.500), FCONST( 4.000),
    FCONST( 5.000), FCONST( 6.000),
    FCONST( 7.000), FCONST( 8.000),
    FCONST( 9.000), FCONST(10.000),
    FCONST(11.000), FCONST(12.000),
    FCONST(14.000), FCONST(16.000)
};

/**
 * Variable bandwidth bit allocation
 * This estimates the bandwidth code which will give quality around 240.
 */
void
vbw_bit_allocation(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    FLOAT mant_bits;
    int blk, ch, bw, nc;
    int avail_bits, bits;
    int wmin, wmax, ncmin, ncmax;

    start_bit_allocation(tctx);
    avail_bits = (16 * frame->frame_size) - frame->frame_bits;

    bit_alloc_prepare(tctx);
    bit_alloc(tctx, 240);

    // deduct any LFE exponent and mantissa bits
    if (ctx->lfe) {
        FLOAT lfe_bits = FCONST(0.0);
        ch = ctx->lfe_channel;
        lfe_bits += expstr_set_bits[frame->expstr_set[ch]][7];
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
            uint8_t *bap = frame->blocks[blk].bap[ch];
            for (nc = 0; nc < 7; nc++)
                lfe_bits += mant_est_tab[bap[nc]];
        }
        avail_bits -= (int)lfe_bits;
    }

    // set limits
    wmin = ctx->params.min_bwcode;
    wmax = ctx->params.max_bwcode;
    ncmin = wmin * 3 + 73;
    ncmax = wmax * 3 + 73;

    // sum up mantissa bits up to bin 72
    mant_bits = FCONST(0.0);
    for (ch = 0; ch < ctx->n_channels; ch++) {
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
            uint8_t *bap = frame->blocks[blk].bap[ch];
            for (nc = 0; nc < ncmin; nc++)
                mant_bits += mant_est_tab[bap[nc]];
        }
    }

    // add bins while estimated bits fit in the frame
    for (nc = ncmin; nc <= ncmax; nc++) {
        bw = (nc - 73) / 3;
        bits = 0;
        for (ch = 0; ch < ctx->n_channels; ch++) {
            bits += expstr_set_bits[frame->expstr_set[ch]][nc];
            for (blk = 0; blk < A52_NUM_BLOCKS; blk++)
                mant_bits += mant_est_tab[frame->blocks[blk].bap[ch][nc]];
        }
        if ((bits + (int)mant_bits) > avail_bits)
            break;
    }

    // set frame bandwidth parameters
    bw = CLIP((nc - 73) / 3, 0, 60);
    nc = bw * 3 + 73;
    frame->bwcode = bw;
    for (ch = 0; ch < ctx->n_channels; ch++)
        frame->ncoefs[ch] = nc;
}

/**
 * Run the bit allocation encoding routine.
 * Runs the bit allocation in either CBR or VBR mode, depending on the mode
 * selected by the user.
 */
int
compute_bit_allocation(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;

    start_bit_allocation(tctx);
    if (ctx->params.encoding_mode == AFTEN_ENC_MODE_VBR) {
        if (vbr_bit_allocation(tctx))
            return -1;
    } else if(ctx->params.encoding_mode == AFTEN_ENC_MODE_CBR) {
        if (cbr_bit_allocation(tctx, 1))
            return -1;
    } else {
        return -1;
    }
    return 0;
}
