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
 * @file exponent.h
 * A/52 exponent header
 */

#ifndef EXPONENT_H
#define EXPONENT_H

#include "common.h"

#if defined(HAVE_MMX) || defined(HAVE_SSE)
#include "x86/exponent.h"
#endif

struct A52ThreadContext;

#define A52_EXPSTR_SETS 32

extern int nexpgrptab[3][256];
extern uint16_t expstr_set_bits[A52_EXPSTR_SETS][256];

typedef struct A52ExponentFunctions {

    /** Set exp[i] to min(exp[i], exp1[i]) */
    void (*exponent_min)(uint8_t *exp, uint8_t *exp1, int n);

    /**
     * Update the exponents so that they are the ones the decoder will decode.
     * Constrain DC exponent, group exponents based on strategy, constrain delta
     * between adjacent exponents to +2/-2.
     */
    void (*encode_exp_blk_ch)(uint8_t *exp, int ncoefs, int exp_strategy);

    /**
     * Calculate sum of squared error between 2 sets of exponents.
     */
    int (*exponent_sum_square_error)(uint8_t *exp0, uint8_t *exp1, int ncoefs);

} A52ExponentFunctions;

extern void exponent_init(A52ExponentFunctions *expf);

extern void a52_process_exponents(struct A52ThreadContext *tctx);

#endif /* EXPONENT_H */
