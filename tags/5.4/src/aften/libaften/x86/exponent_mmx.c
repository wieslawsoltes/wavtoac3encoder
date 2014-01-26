/**
 * Aften: A/52 audio encoder
 *
 * MMX exponent functions
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
 * @file x86_mmx_exponent.c
 * A/52 mmx optimized exponent functions
 */

#include "a52enc.h"
#include "x86/simd_support.h"


void
exponent_min_mmx(uint8_t *exp, uint8_t *exp1, int n)
{
    int i;

    for (i = 0; i < (n & ~7); i += 8) {
        __m64 vexp = *(__m64*)&exp[i];
        __m64 vexp1 = *(__m64*)&exp1[i];
        __m64 vmask = _mm_cmpgt_pi8(vexp, vexp1);
        vexp = _mm_andnot_si64(vmask, vexp);
        vexp1 = _mm_and_si64(vmask, vexp1);
        vexp = _mm_or_si64(vexp, vexp1);
        *(__m64*)&exp[i] = vexp;
    }
    switch (n & 7) {
    case 7:
        exp[i] = MIN(exp[i], exp1[i]);
        ++i;
    case 6:
        exp[i] = MIN(exp[i], exp1[i]);
        ++i;
    case 5:
        exp[i] = MIN(exp[i], exp1[i]);
        ++i;
    case 4:
        exp[i] = MIN(exp[i], exp1[i]);
        ++i;
    case 3:
        exp[i] = MIN(exp[i], exp1[i]);
        ++i;
    case 2:
        exp[i] = MIN(exp[i], exp1[i]);
        ++i;
    case 1:
        exp[i] = MIN(exp[i], exp1[i]);
    case 0:
        ;
    }
    _mm_empty();
}


void
encode_exp_blk_ch_mmx(uint8_t *exp, int ncoefs, int exp_strategy)
{
    int grpsize, ngrps, i, k, exp_min1, exp_min2;
    uint8_t v;

    ngrps = nexpgrptab[exp_strategy-1][ncoefs] * 3;
    grpsize = exp_strategy + (exp_strategy == EXP_D45);

    // for D15 strategy, there is no need to group/ungroup exponents
    switch (grpsize) {
    case 1: {
        // constraint for DC exponent
        exp[0] = MIN(exp[0], 15);

        // Decrease the delta between each groups to within 2
        // so that they can be differentially encoded
        for (i = 1; i <= ngrps; i++)
            exp[i] = MIN(exp[i], exp[i-1]+2);
        for (i = ngrps-1; i >= 0; i--)
            exp[i] = MIN(exp[i], exp[i+1]+2);

        break;
    }
    // for each group, compute the minimum exponent
    case 2: {
        ALIGN16(uint16_t) exp1[256];
        ALIGN16(const union __m64ui) vmask = {{0x00ff00ff, 0x00ff00ff}};

        i=0; k=1;
        for (; i < (ngrps & ~3); i += 4, k += 8) {
            __m64 v1 = *(__m64*)&exp[k];
            __m64 v2 = _mm_srli_si64(v1, 8);
            __m64 vcmask;
            v1 = _mm_and_si64(v1, vmask.v);
            //v1 = _mm_min_pi8(v1, v2);
            vcmask = _mm_cmpgt_pi8(v1, v2);
            v1 = _mm_andnot_si64(vcmask, v1);
            v2 = _mm_and_si64(vcmask, v2);
            v1 = _mm_or_si64(v1, v2);

            *(__m64*)&exp1[i] = v1;
        }
        switch (ngrps & 3) {
        case 3:
            exp1[i] = MIN(exp[k], exp[k+1]);
            ++i;
            k += 2;
        case 2:
            exp1[i] = MIN(exp[k], exp[k+1]);
            ++i;
            k += 2;
        case 1:
            exp1[i] = MIN(exp[k], exp[k+1]);
        case 0:
            ;
        }
        // constraint for DC exponent
        exp[0] = MIN(exp[0], 15);
        // Decrease the delta between each groups to within 2
        // so that they can be differentially encoded
        exp1[0] = MIN(exp1[0], (uint16_t)exp[0]+2);
        for (i = 1; i < ngrps; i++)
            exp1[i] = MIN(exp1[i], exp1[i-1]+2);
        for(i = ngrps-2; i >= 0; i--)
            exp1[i] = MIN(exp1[i], exp1[i+1]+2);
       // now we have the exponent values the decoder will see
        exp[0] = MIN(exp[0], exp1[0]+2); // DC exponent is handled separately

        i=0; k=1;
        for(; i < (ngrps & ~3); i += 4, k += 8) {
            __m64 v1 = *(__m64*)&exp1[i];
            __m64 v2 = _mm_slli_si64(v1, 8);
            v1 = _mm_or_si64(v1, v2);
            *(__m64*)&exp[k] = v1;
        }
        switch (ngrps & 3) {
        case 3:
            v = (uint8_t)exp1[i];
            exp[k] = v;
            exp[k+1] = v;
            ++i;
            k += 2;
        case 2:
            v = (uint8_t)exp1[i];
            exp[k] = v;
            exp[k+1] = v;
            ++i;
            k += 2;
        case 1:
            v = (uint8_t)exp1[i];
            exp[k] = v;
            exp[k+1] = v;
        case 0:
            ;
        }
        break;
        }
    default: {
        ALIGN16(uint32_t) exp1[256];
        ALIGN16(const union __m64ui) vmask2 = {{0x000000ff, 0x000000ff}};

        i=0;k=1;
        for (; i < (ngrps & ~1); i += 2, k += 8) {
            __m64 v1 = *(__m64*)&exp[k];
            __m64 v2 = _mm_srli_si64(v1, 8);
            //v1 = _mm_min_pi8(v1, v2);
            __m64 vcmask = _mm_cmpgt_pi8(v1, v2);
            v1 = _mm_andnot_si64(vcmask, v1);
            v2 = _mm_and_si64(vcmask, v2);
            v1 = _mm_or_si64(v1, v2);

            v2 = _mm_srli_si64(v1, 16);
            //v1 = _mm_min_pi8(v1, v2);
            vcmask = _mm_cmpgt_pi8(v1, v2);
            v1 = _mm_andnot_si64(vcmask, v1);
            v2 = _mm_and_si64(vcmask, v2);
            v1 = _mm_or_si64(v1, v2);

            v1 = _mm_and_si64(v1, vmask2.v);
            *(__m64*)&exp1[i] = v1;
        }
        if (ngrps & 1) {
            exp_min1 = MIN(exp[k  ], exp[k+1]);
            exp_min2 = MIN(exp[k+2], exp[k+3]);
            exp1[i]  = MIN(exp_min1, exp_min2);
        }
        // constraint for DC exponent
        exp[0] = MIN(exp[0], 15);
        // Decrease the delta between each groups to within 2
        // so that they can be differentially encoded
        exp1[0] = MIN(exp1[0], (uint32_t)exp[0]+2);
        for (i = 1; i < ngrps; i++)
            exp1[i] = MIN(exp1[i], exp1[i-1]+2);
        for (i = ngrps-2; i >= 0; i--)
            exp1[i] = MIN(exp1[i], exp1[i+1]+2);
       // now we have the exponent values the decoder will see
        exp[0] = MIN(exp[0], exp1[0]+2); // DC exponent is handled separately

        i=0; k=1;
        for (; i < (ngrps & ~1); i += 2, k += 8) {
            __m64 v1 = *(__m64*)&exp1[i];
            __m64 v2 = _mm_slli_si64(v1, 8);
            v1 = _mm_or_si64(v1, v2);
            v2 = _mm_slli_si64(v1, 16);
            v1 = _mm_or_si64(v1, v2);
            *(__m64*)&exp[k] = v1;
        }
        if (ngrps & 1) {
            v = exp1[i];
            exp[k] = v;
            exp[k+1] = v;
            exp[k+2] = v;
            exp[k+3] = v;
        }
        break;
    }
    }
    _mm_empty();
}


int
exponent_sum_square_error_mmx(uint8_t *exp0, uint8_t *exp1, int ncoefs)
{
    int i, err;
    int exp_error = 0;
    union {
        __m64 v;
        int32_t res[2];
    } ures;
    __m64 vzero = _mm_setzero_si64();
    __m64 vres = vzero;

    for (i = 0; i < (ncoefs & ~7); i += 8) {
        __m64 vexp = *(__m64*)&exp0[i];
        __m64 vexp2 = *(__m64*)&exp1[i];
#if 0
        //safer but needed?
        __m64 vexphi = _mm_unpackhi_pi8(vexp, vzero);
        __m64 vexp2hi = _mm_unpackhi_pi8(vexp2, vzero);
        __m64 vexplo = _mm_unpacklo_pi8(vexp, vzero);
        __m64 vexp2lo = _mm_unpacklo_pi8(vexp2, vzero);
        __m64 verrhi = _mm_sub_pi16(vexphi, vexp2hi);
        __m64 verrlo = _mm_sub_pi16(vexplo, vexp2lo);
#else
        __m64 verr = _mm_sub_pi8(vexp, vexp2);
        __m64 vsign = _mm_cmpgt_pi8(vzero, verr);
        __m64 verrhi = _mm_unpackhi_pi8(verr, vsign);
        __m64 verrlo = _mm_unpacklo_pi8(verr, vsign);
#endif
        verrhi = _mm_madd_pi16(verrhi, verrhi);
        verrlo = _mm_madd_pi16(verrlo, verrlo);
        verrhi = _mm_add_pi32(verrhi, verrlo);
        vres = _mm_add_pi32(vres, verrhi);
    }
    ures.v = vres;
    exp_error += ures.res[0]+ures.res[1];
    switch (ncoefs & 7) {
    case 7:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
        ++i;
    case 6:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
        ++i;
    case 5:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
        ++i;
    case 4:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
        ++i;
    case 3:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
        ++i;
    case 2:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
        ++i;
    case 1:
        err = exp0[i] - exp1[i];
        exp_error += (err * err);
    case 0:
        ;
    }
    _mm_empty();
    return exp_error;
}
