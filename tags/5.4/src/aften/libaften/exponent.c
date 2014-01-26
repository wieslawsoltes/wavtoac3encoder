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
 * @file exponent.c
 * A/52 exponent functions
 */

#include "a52enc.h"
#include "cpu_caps.h"

uint16_t expstr_set_bits[A52_EXPSTR_SETS][256] = {{0}};


/**
 * Search order for the pre-defined strategy sets.
 * The sets are arranged to give a good distribution of time domain vs.
 * frequency domain accuracy with different search sizes.
 */
static const uint8_t expstr_set_search_order_tab[A52_EXPSTR_SETS] = {
     0, 31, 10,  2,  8, 17, 24, 22,  3, 21, 26, 13, 14, 23, 29, 11,
    30, 27, 15, 16,  1,  9, 12,  4,  5,  6,  7, 18, 19, 20, 25, 28
};


/**
 * Determine a good exponent strategy for all blocks of a single channel.
 * A pre-defined set of strategies is chosen based on the SSE between each set
 * and the most accurate strategy set (all blocks EXP_D15).
 */
static int
compute_expstr_ch(A52ExponentFunctions *expf, uint8_t *exp[A52_NUM_BLOCKS],
                  int ncoefs, int search_size)
{
    ALIGN16(uint8_t) exponents[A52_NUM_BLOCKS][256];
    int blk, s, str, i, j, k;
    int min_error, exp_error[A52_EXPSTR_SETS];

    min_error = expstr_set_search_order_tab[0];
    for (s = 0; s < search_size; s++) {
        str = expstr_set_search_order_tab[s];

        // collect exponents
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++)
            memcpy(exponents[blk], exp[blk], 256);

        // encode exponents
        i = 0;
        while (i < A52_NUM_BLOCKS) {
            j = i + 1;
            while (j < A52_NUM_BLOCKS && a52_expstr_set_tab[str][j]==EXP_REUSE) {
                expf->exponent_min(exponents[i], exponents[j], ncoefs);
                j++;
            }
            expf->encode_exp_blk_ch(exponents[i], ncoefs, a52_expstr_set_tab[str][i]);
            for (k = i+1; k < j; k++)
                memcpy(exponents[k], exponents[i], 256);
            i = j;
        }

        // select strategy based on minimum error from unencoded exponents
        exp_error[str] = 0;
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
            exp_error[str] += expf->exponent_sum_square_error(exp[blk],
                                                        exponents[blk],
                                                        ncoefs);
        }
        if (exp_error[str] < exp_error[min_error])
            min_error = str;
    }
    return min_error;
}

/**
 * Runs the per-channel exponent strategy decision function for all channels
 */
static void
compute_exponent_strategy(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *blocks = frame->blocks;
    int *ncoefs = frame->ncoefs;
    uint8_t *exp[A52_MAX_CHANNELS][A52_NUM_BLOCKS];
    int ch, blk, str;

    for (ch = 0; ch < ctx->n_channels; ch++) {
        str = expstr_set_search_order_tab[0];
        if (ctx->params.expstr_search > 1) {
            for (blk = 0; blk < A52_NUM_BLOCKS; blk++)
                exp[ch][blk] = blocks[blk].exp[ch];
            str = compute_expstr_ch(&ctx->expf, exp[ch], ncoefs[ch], ctx->params.expstr_search);
        }
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++)
            blocks[blk].exp_strategy[ch] = a52_expstr_set_tab[str][blk];
        frame->expstr_set[ch] = str;
    }

    // lfe channel
    if (ctx->lfe) {
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++)
            blocks[blk].exp_strategy[ctx->lfe_channel] = !blk ? EXP_D15 : EXP_REUSE;
    }
}

/**
 * Encode exponent groups.  3 exponents are in per 7-bit group.  The number of
 * groups varies depending on exponent strategy and bandwidth
 */
static void
group_exponents(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *block;
    uint8_t *p;
    int delta[3];
    int blk, ch, i, gsize, bits;
    int expstr;
    int exp0, exp1, exp2, exp3;

    bits = 0;
    for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
        block = &frame->blocks[blk];
        for (ch = 0; ch < ctx->n_all_channels; ch++) {
            expstr = block->exp_strategy[ch];
            if (expstr == EXP_REUSE) {
                block->nexpgrps[ch] = 0;
                continue;
            }
            block->nexpgrps[ch] = nexpgrptab[expstr-1][frame->ncoefs[ch]];
            bits += (4 + (block->nexpgrps[ch] * 7));
            gsize = expstr + (expstr == EXP_D45);
            p = block->exp[ch];

            exp1 = *p++;
            block->grp_exp[ch][0] = exp1;

            for (i = 1; i <= block->nexpgrps[ch]; i++) {
                /* merge three delta into one code */
                exp0 = exp1;
                exp1 = p[0];
                p += gsize;
                delta[0] = exp1 - exp0 + 2;

                exp2 = p[0];
                p += gsize;
                delta[1] = exp2 - exp1 + 2;

                exp3 = p[0];
                p += gsize;
                delta[2] = exp3 - exp2 + 2;
                exp1 = exp3;

                block->grp_exp[ch][i] = ((delta[0]*5+delta[1])*5)+delta[2];
            }
        }
    }
    frame->exp_bits = bits;
}

/**
 * Creates final exponents for the entire frame based on exponent strategies.
 * If the strategy for a block & channel is EXP_REUSE, exponents are copied,
 * otherwise they are encoded according to the specific exponent strategy.
 */
static void
encode_exponents(A52ThreadContext *tctx)
{
    A52Context *ctx = tctx->ctx;
    A52Frame *frame = &tctx->frame;
    A52Block *blocks = frame->blocks;
    int *ncoefs = frame->ncoefs;
    int ch, i, j, k;

    for (ch = 0; ch < ctx->n_all_channels; ch++) {
        // compute the exponents as the decoder will see them. The
        // EXP_REUSE case must be handled carefully : we select the
        // min of the exponents
        i = 0;
        while (i < A52_NUM_BLOCKS) {
            j = i + 1;
            while (j < A52_NUM_BLOCKS && blocks[j].exp_strategy[ch]==EXP_REUSE) {
                ctx->expf.exponent_min(blocks[i].exp[ch], blocks[j].exp[ch], ncoefs[ch]);
                j++;
            }
            ctx->expf.encode_exp_blk_ch(blocks[i].exp[ch], ncoefs[ch],
                              blocks[i].exp_strategy[ch]);
            // copy encoded exponents for reuse case
            for (k = i+1; k < j; k++)
                memcpy(blocks[k].exp[ch], blocks[i].exp[ch], ncoefs[ch]);
            i = j;
        }
    }
}

/**
 * Extracts the optimal exponent portion of each MDCT coefficient.
 */
static void
extract_exponents(A52ThreadContext *tctx)
{
    A52Frame *frame = &tctx->frame;
    A52Block *block;
    int all_channels = tctx->ctx->n_all_channels;
    int blk, ch, j;
    uint32_t v1, v2;

    for (ch = 0; ch < all_channels; ch++) {
        for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
            block = &frame->blocks[blk];
            for (j = 0; j < 256; j += 2) {
                v1 = (uint32_t)AFT_FABS(block->mdct_coef[ch][j  ] * FCONST(16777216.0));
                v2 = (uint32_t)AFT_FABS(block->mdct_coef[ch][j+1] * FCONST(16777216.0));
                block->exp[ch][j  ] = (v1 == 0)? 24 : 23 - log2i(v1);
                block->exp[ch][j+1] = (v2 == 0)? 24 : 23 - log2i(v2);
            }
        }
    }
}

static void
exponent_min(uint8_t *exp, uint8_t *exp1, int n)
{
    int i;
    for (i = 0; i < n; i++)
        exp[i] = MIN(exp[i], exp1[i]);
}


static void
encode_exp_blk_ch(uint8_t *exp, int ncoefs, int exp_strategy)
{
    int i, k;
    int ngrps;
    int exp_min1, exp_min2;

    ngrps = nexpgrptab[exp_strategy-1][ncoefs] * 3;

    // constraint for DC exponent
    exp[0] = MIN(exp[0], 15);

    // for each group, compute the minimum exponent
    switch (exp_strategy) {
    case EXP_D25:
        for (i = 1, k = 1; i <= ngrps; i++) {
            exp[i] = MIN(exp[k], exp[k+1]);
            k += 2;
        }
        break;
    case EXP_D45:
        for (i = 1, k = 1; i <= ngrps; i++) {
            exp_min1 = MIN(exp[k  ], exp[k+1]);
            exp_min2 = MIN(exp[k+2], exp[k+3]);
            exp[i]   = MIN(exp_min1, exp_min2);
            k += 4;
        }
        break;
    }

    // Decrease the delta between each groups to within 2
    // so that they can be differentially encoded
    for (i = 1; i <= ngrps; i++)
        exp[i] = MIN(exp[i], exp[i-1]+2);
    for (i = ngrps-1; i >= 0; i--)
        exp[i] = MIN(exp[i], exp[i+1]+2);

    // expand exponent groups to generate final set of exponents
    switch (exp_strategy) {
    case EXP_D25:
        for (i = ngrps, k = ngrps*2; i > 0; i--) {
            exp[k] = exp[k-1] = exp[i];
            k -= 2;
        }
        break;
    case EXP_D45:
        for (i = ngrps, k = ngrps*4; i > 0; i--) {
            exp[k] = exp[k-1] = exp[k-2] = exp[k-3] = exp[i];
            k -= 4;
        }
        break;
    }
}


static int
exponent_sum_square_error(uint8_t *exp0, uint8_t *exp1, int ncoefs)
{
    int i, err;
    int exp_error = 0;

    for (i = 0; i < ncoefs; i++) {
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
    }
    return exp_error;
}


/**
 * Runs all the processes in extracting, analyzing, and encoding exponents
 */
void
a52_process_exponents(A52ThreadContext *tctx)
{
    extract_exponents(tctx);

    compute_exponent_strategy(tctx);

    encode_exponents(tctx);

    group_exponents(tctx);
}


/**
 * Initialize exponent group size table
 */
void
exponent_init(A52ExponentFunctions *expf)
{
    int i, j, grpsize, ngrps, nc, blk;

    for (i = 1; i < 4; i++) {
        for (j = 0; j < 256; j++) {
            grpsize = i + (i == EXP_D45);
            ngrps = 0;
            if (j == 7)
                ngrps = 2;
            else
                ngrps = (j + (grpsize * 3) - 4) / (3 * grpsize);
            nexpgrptab[i-1][j] = ngrps;
        }
    }

    for (i = 0; i < 6; i++) {
        uint16_t *expbits = expstr_set_bits[i];
        for (nc = 0; nc <= 253; nc++) {
            uint16_t bits = 0;
            for (blk = 0; blk < A52_NUM_BLOCKS; blk++) {
                uint8_t es = a52_expstr_set_tab[i][blk];
                if (es != EXP_REUSE)
                    bits += (4 + (nexpgrptab[es-1][nc] * 7));
            }
            expbits[nc] = bits;
        }
    }

    expf->exponent_min = exponent_min;
    expf->encode_exp_blk_ch = encode_exp_blk_ch;
    expf->exponent_sum_square_error = exponent_sum_square_error;
#ifdef HAVE_MMX
    if (cpu_caps_have_mmx()) {
        expf->exponent_min = exponent_min_mmx;
        expf->encode_exp_blk_ch = encode_exp_blk_ch_mmx;
        expf->exponent_sum_square_error = exponent_sum_square_error_mmx;
    }
#endif /* HAVE_MMX */
#ifdef HAVE_SSE2
    if (cpu_caps_have_sse2()) {
        expf->exponent_min = exponent_min_sse2;
        expf->encode_exp_blk_ch = encode_exp_blk_ch_sse2;
        expf->exponent_sum_square_error = exponent_sum_square_error_sse2;
    }
#endif /* HAVE_SSE2 */
}
