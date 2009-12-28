/**
 * Aften: A/52 audio encoder
 *
 * x86 exponent functions header
 * Copyright (c) 2007 Prakash Punnoor <prakash@punnoor.de>
 *               2006 Justin Ruggles
 *
 * Based on "The simplest AC3 encoder" from FFmpeg
 * Copyright (c) 2000 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation;
 * version 2 of the License
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

/**
 * @file exponent.h
 * A/52 x86 exponent header
 */

#ifndef X86_EXPONENT_H
#define X86_EXPONENT_H

#include "common.h"

#ifdef HAVE_SSE2
extern void exponent_min_sse2(uint8_t *exp, uint8_t *exp1, int n);
extern void encode_exp_blk_ch_sse2(uint8_t *exp, int ncoefs, int exp_strategy);
extern int exponent_sum_square_error_sse2(uint8_t *exp0, uint8_t *exp1, int ncoefs);
#endif
#ifdef HAVE_MMX
extern void exponent_min_mmx(uint8_t *exp, uint8_t *exp1, int n);
extern void encode_exp_blk_ch_mmx(uint8_t *exp, int ncoefs, int exp_strategy);
extern int exponent_sum_square_error_mmx(uint8_t *exp0, uint8_t *exp1, int ncoefs);
#endif

#endif /* X86_EXPONENT_H */
