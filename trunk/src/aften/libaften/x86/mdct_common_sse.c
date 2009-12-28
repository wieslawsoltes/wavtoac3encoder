/**
 * Aften: A/52 audio encoder
 *
 * SSE MDCT functions
 * This file is derived from libvorbis lancer patch
 * Copyright (c) 2006-2007 prakash@punnoor.de
 * Copyright (c) 2006, blacksword8192@hotmail.com
 * Copyright (c) 2002, Xiph.org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file x86/mdct_common_sse.c
 * MDCT, common functions for the SSE and SSE3 instruction sets
 */

#include "a52enc.h"
#include "x86/simd_support.h"
#include "mdct_common_sse.h"


static const union __m128ui PCS_NNRR = {{0x80000000, 0x80000000, 0x00000000, 0x00000000}};
static const union __m128ui PCS_RNRN = {{0x00000000, 0x80000000, 0x00000000, 0x80000000}};
static const union __m128ui PCS_RRNN = {{0x00000000, 0x00000000, 0x80000000, 0x80000000}};
static const union __m128ui PCS_RNNR = {{0x80000000, 0x00000000, 0x00000000, 0x80000000}};
static const union __m128ui PCS_RRRR = {{0x80000000, 0x80000000, 0x80000000, 0x80000000}};
static const union __m128ui PCS_NRRN = {{0x00000000, 0x80000000, 0x80000000, 0x00000000}};
static const union __m128f PFV_0P5 = {{0.5f, 0.5f, 0.5f, 0.5f}};


/** 8 point butterfly */
static inline void
mdct_butterfly_8_sse(FLOAT *x) {
    __m128  XMM0, XMM1, XMM2, XMM3;
    XMM0     = _mm_load_ps(x+4);
    XMM1     = _mm_load_ps(x  );
    XMM2     = XMM0;
    XMM0     = _mm_sub_ps(XMM0, XMM1);
    XMM2     = _mm_add_ps(XMM2, XMM1);

    XMM1     = XMM0;
    XMM3     = XMM2;

    XMM0     = _mm_shuffle_ps(XMM0, XMM0, _MM_SHUFFLE(3,2,3,2));
    XMM1     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(0,1,0,1));
    XMM2     = _mm_shuffle_ps(XMM2, XMM2, _MM_SHUFFLE(3,2,3,2));
    XMM3     = _mm_shuffle_ps(XMM3, XMM3, _MM_SHUFFLE(1,0,1,0));

    XMM1     = _mm_xor_ps(XMM1, PCS_NRRN.v);
    XMM3     = _mm_xor_ps(XMM3, PCS_NNRR.v);

    XMM0     = _mm_add_ps(XMM0, XMM1);
    XMM2     = _mm_add_ps(XMM2, XMM3);

    _mm_store_ps(x  , XMM0);
    _mm_store_ps(x+4, XMM2);
}

/** 16 point butterfly */
void
mdct_butterfly_16_sse(FLOAT *x) {
    static _MM_ALIGN16 const float PFV0[4] = { AFT_PI2_8,  AFT_PI2_8, 1.f, -1.f};
    static _MM_ALIGN16 const float PFV1[4] = { AFT_PI2_8, -AFT_PI2_8, 0.f,  0.f};
    static _MM_ALIGN16 const float PFV2[4] = { AFT_PI2_8,  AFT_PI2_8, 1.f,  1.f};
    static _MM_ALIGN16 const float PFV3[4] = {-AFT_PI2_8,  AFT_PI2_8, 0.f,  0.f};
    __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7;

    XMM3     = _mm_load_ps(x+12);
    XMM0     = _mm_load_ps(x   );
    XMM1     = _mm_load_ps(x+ 4);
    XMM2     = _mm_load_ps(x+ 8);
    XMM4     = XMM3;
    XMM5     = XMM0;
    XMM0     = _mm_sub_ps(XMM0, XMM2);
    XMM3     = _mm_sub_ps(XMM3, XMM1);
    XMM2     = _mm_add_ps(XMM2, XMM5);
    XMM4     = _mm_add_ps(XMM4, XMM1);
    XMM1     = XMM0;
    XMM5     = XMM3;
    _mm_store_ps(x+ 8, XMM2);
    _mm_store_ps(x+12, XMM4);
    XMM0     = _mm_shuffle_ps(XMM0, XMM0, _MM_SHUFFLE(2,3,1,1));
    XMM2     = _mm_load_ps(PFV0);
    XMM1     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(2,3,0,0));
    XMM4     = _mm_load_ps(PFV1);
    XMM3     = _mm_shuffle_ps(XMM3, XMM3, _MM_SHUFFLE(3,2,0,0));
    XMM6     = _mm_load_ps(PFV2);
    XMM5     = _mm_shuffle_ps(XMM5, XMM5, _MM_SHUFFLE(3,2,1,1));
    XMM7     = _mm_load_ps(PFV3);
    XMM0     = _mm_mul_ps(XMM0, XMM2);
    XMM1     = _mm_mul_ps(XMM1, XMM4);
    XMM3     = _mm_mul_ps(XMM3, XMM6);
    XMM5     = _mm_mul_ps(XMM5, XMM7);
    XMM0     = _mm_add_ps(XMM0, XMM1);
    XMM3     = _mm_add_ps(XMM3, XMM5);
    _mm_store_ps(x   , XMM0);
    _mm_store_ps(x+ 4, XMM3);

    mdct_butterfly_8_sse(x);
    mdct_butterfly_8_sse(x+8);
}

static void
mdct_butterflies_sse(MDCTContext *mdct, FLOAT *x, int points)
{
    FLOAT *trig = mdct->trig_butterfly_first;
    int stages = mdct->log2n-5;
    int i, j;

    if (--stages > 0)
        mdct->mdct_butterfly_first(trig, x, points);

    for (i = 1; --stages > 0; i++)
        for (j = 0; j < (1<<i); j++)
            mdct->mdct_butterfly_generic(mdct, x+(points>>i)*j, points>>i, 4<<i);

    for (j = 0; j < points; j += 32)
        mdct->mdct_butterfly_32(x+j);
}

static void
mdct_sse(MDCTThreadContext *tmdct, FLOAT *out, FLOAT *in)
{
    MDCTContext *mdct = tmdct->mdct;
    int n = mdct->n;
    int n2 = n>>1;
    int n4 = n>>2;
    int n8 = n>>3;
    FLOAT *w = tmdct->buffer;
    FLOAT *w2 = w+n2;
    float *x0    = in+n2+n4-8;
    float *x1    = in+n2+n4;
    float *T     = mdct->trig_forward;

    int i, j;

#ifdef __INTEL_COMPILER
#pragma warning(disable : 592)
#endif
    for (i = 0, j = n2-2; i < n8; i += 4, j -= 4) {
        __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7;
        XMM0     = _mm_load_ps(x0    + 4);
        XMM4     = _mm_load_ps(x0       );
        XMM1     = _mm_load_ps(x0+i*4+ 8);
        XMM5     = _mm_load_ps(x0+i*4+12);
        XMM2     = _mm_load_ps(T   );
        XMM3     = _mm_load_ps(T+ 4);
        XMM6     = _mm_load_ps(T+ 8);
        XMM7     = _mm_load_ps(T+12);
        XMM0     = _mm_shuffle_ps(XMM0, XMM0, _MM_SHUFFLE(0,1,2,3));
        XMM4     = _mm_shuffle_ps(XMM4, XMM4, _MM_SHUFFLE(0,1,2,3));
        XMM0     = _mm_add_ps(XMM0, XMM1);
        XMM4     = _mm_add_ps(XMM4, XMM5);
        XMM1     = XMM0;
        XMM5     = XMM4;
        XMM0     = _mm_shuffle_ps(XMM0, XMM0, _MM_SHUFFLE(0,0,3,3));
        XMM1     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(2,2,1,1));
        XMM4     = _mm_shuffle_ps(XMM4, XMM4, _MM_SHUFFLE(0,0,3,3));
        XMM5     = _mm_shuffle_ps(XMM5, XMM5, _MM_SHUFFLE(2,2,1,1));
        XMM0     = _mm_mul_ps(XMM0, XMM2);
        XMM1     = _mm_mul_ps(XMM1, XMM3);
        XMM4     = _mm_mul_ps(XMM4, XMM6);
        XMM5     = _mm_mul_ps(XMM5, XMM7);
        XMM0     = _mm_sub_ps(XMM0, XMM1);
        XMM4     = _mm_sub_ps(XMM4, XMM5);
        _mm_storel_pi((__m64*)(w2+i  ), XMM0);
        _mm_storeh_pi((__m64*)(w2+j  ), XMM0);
        _mm_storel_pi((__m64*)(w2+i+2), XMM4);
        _mm_storeh_pi((__m64*)(w2+j-2), XMM4);
        x0  -= 8;
        T   += 16;
    }

    x0   = in;
    x1   = in+n2-8;

    for (; i < n4; i += 4, j -= 4) {
        __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7;
        XMM1     = _mm_load_ps(x1+4);
        XMM5     = _mm_load_ps(x1  );
        XMM0     = _mm_load_ps(x0  );
        XMM4     = _mm_load_ps(x0+4);
        XMM2     = _mm_load_ps(T   );
        XMM3     = _mm_load_ps(T+ 4);
        XMM6     = _mm_load_ps(T+ 8);
        XMM7     = _mm_load_ps(T+12);
        XMM1     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(0,1,2,3));
        XMM5     = _mm_shuffle_ps(XMM5, XMM5, _MM_SHUFFLE(0,1,2,3));
        XMM0     = _mm_sub_ps(XMM0, XMM1);
        XMM4     = _mm_sub_ps(XMM4, XMM5);
        XMM1     = XMM0;
        XMM5     = XMM4;
        XMM0     = _mm_shuffle_ps(XMM0, XMM0, _MM_SHUFFLE(0,0,3,3));
        XMM1     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(2,2,1,1));
        XMM4     = _mm_shuffle_ps(XMM4, XMM4, _MM_SHUFFLE(0,0,3,3));
        XMM5     = _mm_shuffle_ps(XMM5, XMM5, _MM_SHUFFLE(2,2,1,1));
        XMM0     = _mm_mul_ps(XMM0, XMM2);
        XMM1     = _mm_mul_ps(XMM1, XMM3);
        XMM4     = _mm_mul_ps(XMM4, XMM6);
        XMM5     = _mm_mul_ps(XMM5, XMM7);
        XMM0     = _mm_add_ps(XMM0, XMM1);
        XMM4     = _mm_add_ps(XMM4, XMM5);
        _mm_storel_pi((__m64*)(w2+i  ), XMM0);
        _mm_storeh_pi((__m64*)(w2+j  ), XMM0);
        _mm_storel_pi((__m64*)(w2+i+2), XMM4);
        _mm_storeh_pi((__m64*)(w2+j-2), XMM4);
        x0  += 8;
        x1  -= 8;
        T   += 16;
    }
#ifdef __INTEL_COMPILER
#pragma warning(default : 592)
#endif

    mdct_butterflies_sse(mdct, w2, n2);
    mdct->mdct_bitreverse(mdct, w);

    /* roatate + window */

    T    = mdct->trig_forward+n;
    x0    =out +n2;

    for (i = 0; i < n4; i += 4) {
        __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7;
        x0  -= 4;
        XMM0     = _mm_load_ps(w+4);
        XMM4     = _mm_load_ps(w  );
        XMM2     = XMM0;
        XMM1     = _mm_load_ps(T   );
        XMM3     = _mm_load_ps(T+ 4);
        XMM6     = _mm_load_ps(T+ 8);
        XMM7     = _mm_load_ps(T+12);
        XMM0     = _mm_shuffle_ps(XMM0, XMM4,_MM_SHUFFLE(0,2,0,2));
        XMM2     = _mm_shuffle_ps(XMM2, XMM4,_MM_SHUFFLE(1,3,1,3));
        XMM4     = XMM0;
        XMM5     = XMM2;
        XMM0     = _mm_mul_ps(XMM0, XMM1);
        XMM2     = _mm_mul_ps(XMM2, XMM3);
        XMM4     = _mm_shuffle_ps(XMM4, XMM4, _MM_SHUFFLE(0,1,2,3));
        XMM5     = _mm_shuffle_ps(XMM5, XMM5, _MM_SHUFFLE(0,1,2,3));
        XMM4     = _mm_mul_ps(XMM4, XMM6);
        XMM5     = _mm_mul_ps(XMM5, XMM7);
        XMM0     = _mm_sub_ps(XMM0, XMM2);
        XMM4     = _mm_add_ps(XMM4, XMM5);
        _mm_store_ps(x0    , XMM0);
        _mm_store_ps(out +i, XMM4);
        w   += 8;
        T   += 16;
    }
}

void
mdct_512_sse(A52ThreadContext *tctx, FLOAT *out, FLOAT *in)
{
    mdct_sse(&tctx->mdct_tctx_512, out, in);
}

void
mdct_256_sse(A52ThreadContext *tctx, FLOAT *out, FLOAT *in)
{
    FLOAT *coef_a, *coef_b, *xx;
    int i, j;

    coef_a = in;
    coef_b = &in[128];
    xx = tctx->mdct_tctx_256.buffer1;

    memcpy(xx, in+64, 192 * sizeof(FLOAT));
    xx += 192;
    for (i = 0; i < 64; i += 4) {
        __m128 XMM0 = _mm_load_ps(in + i);
        XMM0 = _mm_xor_ps(XMM0, PCS_RRRR.v);
        _mm_store_ps(xx + i, XMM0);
    }
    xx -= 192;

    mdct_sse(&tctx->mdct_tctx_256, coef_a, xx);

    in += 256 + 192;
    for (i = 0; i < 64; i += 4) {
        __m128 XMM0 = _mm_load_ps(in + i);
        XMM0 = _mm_xor_ps(XMM0, PCS_RRRR.v);
        _mm_store_ps(xx + i, XMM0);
    }
    in -= 256 + 192;

    memcpy(xx+64, in+256, 128 * sizeof(FLOAT));
    xx += 192;
    in += 256 + 128;
    for (i = 0; i < 64; i += 4) {
        __m128 XMM0 = _mm_load_ps(in + i);
        XMM0 = _mm_xor_ps(XMM0, PCS_RRRR.v);
        _mm_store_ps(xx + i, XMM0);
    }
    xx -= 192;
    in -= 256 + 128;

    mdct_sse(&tctx->mdct_tctx_256, coef_b, xx);

    for (i = 0, j = 0; i < 128; i += 4, j += 8) {
        __m128 XMM0 = _mm_load_ps(coef_a + i);
        __m128 XMM1 = _mm_load_ps(coef_b + i);
        __m128 XMM2 = _mm_unpacklo_ps(XMM0, XMM1);
        __m128 XMM3 = _mm_unpackhi_ps(XMM0, XMM1);
        _mm_store_ps(out + j  , XMM2);
        _mm_store_ps(out + j+4, XMM3);
    }
}

void
mdct_ctx_init_sse(MDCTContext *mdct, int n)
{
    mdct_ctx_init(mdct, n);
    {
        __m128  pscalem  = _mm_set_ps1(mdct->scale);
        float *T, *S;
        int n2   = n>>1;
        int n4   = n>>2;
        int n8   = n>>3;
        int i, j;
        /*
            for mdct_bitreverse
        */
        T    = aligned_malloc(sizeof(*T)*n2);
        mdct->trig_bitreverse    = T;
        S    = mdct->trig+n;
        for (i = 0; i < n4; i += 8) {
            __m128  XMM0     = _mm_load_ps(S+i   );
            __m128  XMM1     = _mm_load_ps(S+i+ 4);
            __m128  XMM2     = XMM0;
            __m128  XMM3     = XMM1;
            XMM2     = _mm_shuffle_ps(XMM2, XMM2, _MM_SHUFFLE(2,3,0,1));
            XMM3     = _mm_shuffle_ps(XMM3, XMM3, _MM_SHUFFLE(2,3,0,1));
            XMM2     = _mm_xor_ps(XMM2, PCS_RNRN.v);
            XMM3     = _mm_xor_ps(XMM3, PCS_RNRN.v);
            _mm_store_ps(T+i*2   , XMM0);
            _mm_store_ps(T+i*2+ 4, XMM2);
            _mm_store_ps(T+i*2+ 8, XMM1);
            _mm_store_ps(T+i*2+12, XMM3);
        }
        /*
            for mdct_forward part 0
        */
        T    = aligned_malloc(sizeof(*T)*(n*2));
        mdct->trig_forward   = T;
        S    = mdct->trig;
        for (i = 0, j = n2-4; i < n8; i += 4, j -= 4) {
            __m128 XMM0 = _mm_load_ps(S+i);
            __m128 XMM2 = _mm_load_ps(S+j);
            __m128 XMM1 = _mm_shuffle_ps(XMM2, XMM0, _MM_SHUFFLE(1,0,3,2));
            __m128 XMM3 = _mm_shuffle_ps(XMM2, XMM0, _MM_SHUFFLE(3,2,1,0));
            XMM0     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(2,3,0,1));
            XMM2     = _mm_shuffle_ps(XMM3, XMM3, _MM_SHUFFLE(2,3,0,1));
            XMM0     = _mm_xor_ps(XMM0, PCS_RRNN.v);
            XMM1     = _mm_xor_ps(XMM1, PCS_RNNR.v);
            XMM2     = _mm_xor_ps(XMM2, PCS_RRNN.v);
            XMM3     = _mm_xor_ps(XMM3, PCS_RNNR.v);
            _mm_store_ps(T+i*4   , XMM0);
            _mm_store_ps(T+i*4+ 4, XMM1);
            _mm_store_ps(T+i*4+ 8, XMM2);
            _mm_store_ps(T+i*4+12, XMM3);
        }
        for (; i < n4; i += 4, j -= 4) {
            __m128 XMM0 = _mm_load_ps(S+i);
            __m128 XMM2 = _mm_load_ps(S+j);
            __m128 XMM1 = _mm_shuffle_ps(XMM2, XMM0, _MM_SHUFFLE(1,0,3,2));
            __m128 XMM3 = _mm_shuffle_ps(XMM2, XMM0, _MM_SHUFFLE(3,2,1,0));
            XMM0     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(2,3,0,1));
            XMM2     = _mm_shuffle_ps(XMM3, XMM3, _MM_SHUFFLE(2,3,0,1));
            XMM0     = _mm_xor_ps(XMM0, PCS_NNRR.v);
            XMM2     = _mm_xor_ps(XMM2, PCS_NNRR.v);
            XMM1     = _mm_xor_ps(XMM1, PCS_RNNR.v);
            XMM3     = _mm_xor_ps(XMM3, PCS_RNNR.v);
            _mm_store_ps(T+i*4   , XMM0);
            _mm_store_ps(T+i*4+ 4, XMM1);
            _mm_store_ps(T+i*4+ 8, XMM2);
            _mm_store_ps(T+i*4+12, XMM3);
        }
        /*
            for mdct_forward part 1
        */
        T    = mdct->trig_forward+n;
        S    = mdct->trig+n2;
        for (i = 0; i < n4; i += 4) {
            __m128  XMM0, XMM1, XMM2;
            XMM0     = _mm_load_ps(S+4);
            XMM2     = _mm_load_ps(S  );
            XMM1     = XMM0;
            XMM0     = _mm_shuffle_ps(XMM0, XMM2,_MM_SHUFFLE(1,3,1,3));
            XMM1     = _mm_shuffle_ps(XMM1, XMM2,_MM_SHUFFLE(0,2,0,2));
            XMM0     = _mm_mul_ps(XMM0, pscalem);
            XMM1     = _mm_mul_ps(XMM1, pscalem);
            _mm_store_ps(T   , XMM0);
            _mm_store_ps(T+ 4, XMM1);
            XMM1     = _mm_shuffle_ps(XMM1, XMM1, _MM_SHUFFLE(0,1,2,3));
            XMM0     = _mm_shuffle_ps(XMM0, XMM0, _MM_SHUFFLE(0,1,2,3));
            _mm_store_ps(T+ 8, XMM1);
            _mm_store_ps(T+12, XMM0);
            S       += 8;
            T       += 16;
        }
        /*
            for mdct_butterfly_first
        */
        S    = mdct->trig;
        T    = aligned_malloc(sizeof(*T)*n*2);
        mdct->trig_butterfly_first   = T;
        for (i = 0; i < n4; i += 4) {
            __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5;
            XMM2     = _mm_load_ps(S   );
            XMM0     = _mm_load_ps(S+ 4);
            XMM5     = _mm_load_ps(S+ 8);
            XMM3     = _mm_load_ps(S+12);
            XMM1     = XMM0;
            XMM4     = XMM3;
            XMM0     = _mm_shuffle_ps(XMM0, XMM2, _MM_SHUFFLE(0,1,0,1));
            XMM1     = _mm_shuffle_ps(XMM1, XMM2, _MM_SHUFFLE(1,0,1,0));
            XMM3     = _mm_shuffle_ps(XMM3, XMM5, _MM_SHUFFLE(0,1,0,1));
            XMM4     = _mm_shuffle_ps(XMM4, XMM5, _MM_SHUFFLE(1,0,1,0));
            XMM1     = _mm_xor_ps(XMM1, PCS_RNRN.v);
            XMM4     = _mm_xor_ps(XMM4, PCS_RNRN.v);
            _mm_store_ps(T   , XMM1);
            _mm_store_ps(T+ 4, XMM4);
            _mm_store_ps(T+ 8, XMM0);
            _mm_store_ps(T+12, XMM3);
            XMM1     = _mm_xor_ps(XMM1, PCS_RRRR.v);
            XMM4     = _mm_xor_ps(XMM4, PCS_RRRR.v);
            XMM0     = _mm_xor_ps(XMM0, PCS_RRRR.v);
            XMM3     = _mm_xor_ps(XMM3, PCS_RRRR.v);
            _mm_store_ps(T+n   , XMM1);
            _mm_store_ps(T+n+ 4, XMM4);
            _mm_store_ps(T+n+ 8, XMM0);
            _mm_store_ps(T+n+12, XMM3);
            S   += 16;
            T   += 16;
        }
        /*
            for mdct_butterfly_generic(trigint=8)
        */
        S    = mdct->trig;
        T    = aligned_malloc(sizeof(*T)*n2);
        mdct->trig_butterfly_generic8    = T;
        for (i = 0; i < n; i += 32) {
            __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5;

            XMM0     = _mm_load_ps(S+ 24);
            XMM2     = _mm_load_ps(S+ 16);
            XMM3     = _mm_load_ps(S+  8);
            XMM5     = _mm_load_ps(S    );
            XMM1     = XMM0;
            XMM4     = XMM3;
            XMM0     = _mm_shuffle_ps(XMM0, XMM2, _MM_SHUFFLE(0,1,0,1));
            XMM1     = _mm_shuffle_ps(XMM1, XMM2, _MM_SHUFFLE(1,0,1,0));
            XMM3     = _mm_shuffle_ps(XMM3, XMM5, _MM_SHUFFLE(0,1,0,1));
            XMM4     = _mm_shuffle_ps(XMM4, XMM5, _MM_SHUFFLE(1,0,1,0));
            XMM1     = _mm_xor_ps(XMM1, PCS_RNRN.v);
            XMM4     = _mm_xor_ps(XMM4, PCS_RNRN.v);
            _mm_store_ps(T   , XMM0);
            _mm_store_ps(T+ 4, XMM1);
            _mm_store_ps(T+ 8, XMM3);
            _mm_store_ps(T+12, XMM4);
            S   += 32;
            T   += 16;
        }
        /*
            for mdct_butterfly_generic(trigint=16)
        */
        S    = mdct->trig;
        T    = aligned_malloc(sizeof(*T)*n4);
        mdct->trig_butterfly_generic16   = T;
        for (i = 0; i < n; i += 64) {
            __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5;

            XMM0     = _mm_load_ps(S+ 48);
            XMM2     = _mm_load_ps(S+ 32);
            XMM3     = _mm_load_ps(S+ 16);
            XMM5     = _mm_load_ps(S    );
            XMM1     = XMM0;
            XMM4     = XMM3;
            XMM0     = _mm_shuffle_ps(XMM0, XMM2, _MM_SHUFFLE(0,1,0,1));
            XMM1     = _mm_shuffle_ps(XMM1, XMM2, _MM_SHUFFLE(1,0,1,0));
            XMM3     = _mm_shuffle_ps(XMM3, XMM5, _MM_SHUFFLE(0,1,0,1));
            XMM4     = _mm_shuffle_ps(XMM4, XMM5, _MM_SHUFFLE(1,0,1,0));
            XMM1     = _mm_xor_ps(XMM1, PCS_RNRN.v);
            XMM4     = _mm_xor_ps(XMM4, PCS_RNRN.v);
            _mm_store_ps(T   , XMM0);
            _mm_store_ps(T+ 4, XMM1);
            _mm_store_ps(T+ 8, XMM3);
            _mm_store_ps(T+12, XMM4);
            S   += 64;
            T   += 16;
        }
        /*
            for mdct_butterfly_generic(trigint=32)
        */
        if (n < 128) {
            mdct->trig_butterfly_generic32   = NULL;
        } else {
            S    = mdct->trig;
            T    = aligned_malloc(sizeof(*T)*n8);
            mdct->trig_butterfly_generic32   = T;
            for (i = 0; i < n; i += 128) {
                __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5;

                XMM0     = _mm_load_ps(S+ 96);
                XMM2     = _mm_load_ps(S+ 64);
                XMM3     = _mm_load_ps(S+ 32);
                XMM5     = _mm_load_ps(S    );
                XMM1     = XMM0;
                XMM4     = XMM3;
                XMM0     = _mm_shuffle_ps(XMM0, XMM2, _MM_SHUFFLE(0,1,0,1));
                XMM1     = _mm_shuffle_ps(XMM1, XMM2, _MM_SHUFFLE(1,0,1,0));
                XMM3     = _mm_shuffle_ps(XMM3, XMM5, _MM_SHUFFLE(0,1,0,1));
                XMM4     = _mm_shuffle_ps(XMM4, XMM5, _MM_SHUFFLE(1,0,1,0));
                XMM1     = _mm_xor_ps(XMM1, PCS_RNRN.v);
                XMM4     = _mm_xor_ps(XMM4, PCS_RNRN.v);
                _mm_store_ps(T   , XMM0);
                _mm_store_ps(T+ 4, XMM1);
                _mm_store_ps(T+ 8, XMM3);
                _mm_store_ps(T+12, XMM4);
                S   += 128;
                T   += 16;
            }
        }
        /*
            for mdct_butterfly_generic(trigint=64)
        */
        if (n < 256) {
            mdct->trig_butterfly_generic64   = NULL;
        } else {
            S    = mdct->trig;
            T    = aligned_malloc(sizeof(*T)*(n8>>1));
            mdct->trig_butterfly_generic64   = T;
            for (i = 0; i < n; i += 256) {
                __m128  XMM0, XMM1, XMM2, XMM3, XMM4, XMM5;

                XMM0     = _mm_load_ps(S+192);
                XMM2     = _mm_load_ps(S+128);
                XMM3     = _mm_load_ps(S+ 64);
                XMM5     = _mm_load_ps(S    );
                XMM1     = XMM0;
                XMM4     = XMM3;
                XMM0     = _mm_shuffle_ps(XMM0, XMM2, _MM_SHUFFLE(0,1,0,1));
                XMM1     = _mm_shuffle_ps(XMM1, XMM2, _MM_SHUFFLE(1,0,1,0));
                XMM3     = _mm_shuffle_ps(XMM3, XMM5, _MM_SHUFFLE(0,1,0,1));
                XMM4     = _mm_shuffle_ps(XMM4, XMM5, _MM_SHUFFLE(1,0,1,0));
                XMM1     = _mm_xor_ps(XMM1, PCS_RNRN.v);
                XMM4     = _mm_xor_ps(XMM4, PCS_RNRN.v);
                _mm_store_ps(T   , XMM0);
                _mm_store_ps(T+ 4, XMM1);
                _mm_store_ps(T+ 8, XMM3);
                _mm_store_ps(T+12, XMM4);
                S   += 256;
                T   += 16;
            }
        }
    }
}
