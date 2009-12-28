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
 * @file common.h
 * Common header file
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_POSIX_MEMALIGN
#define _XOPEN_SOURCE 600
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
#if defined(_WIN32) && defined(_MSC_VER)

// integer types
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

// limits
#define INT8_MIN    _I8_MIN
#define INT8_MAX    _I8_MAX
#define UINT8_MAX   _UI8_MAX
#define INT16_MIN   _I16_MIN
#define INT16_MAX   _I16_MAX
#define UINT16_MAX  _UI16_MAX
#define INT32_MIN   _I32_MIN
#define INT32_MAX   _I32_MAX
#define UINT32_MAX  _UI32_MAX
#define INT64_MIN   _I64_MIN
#define INT64_MAX   _I64_MAX
#define UINT64_MAX  _UI64_MAX

// fprintf macros
#define PRId8       "d"
#define PRIi8       "i"
#define PRIo8       "o"
#define PRIu8       "u"
#define PRIx8       "x"
#define PRIX8       "X"
#define PRId16      "hd"
#define PRIi16      "hi"
#define PRIo16      "ho"
#define PRIu16      "hu"
#define PRIx16      "hx"
#define PRIX16      "hX"
#define PRId32      "I32d"
#define PRIi32      "I32i"
#define PRIo32      "I32o"
#define PRIu32      "I32u"
#define PRIx32      "I32x"
#define PRIX32      "I32X"
#define PRId64      "I64d"
#define PRIi64      "I64i"
#define PRIo64      "I64o"
#define PRIu64      "I64u"
#define PRIx64      "I64x"
#define PRIX64      "I64X"

#endif
#endif /* EMULATE_INTTYPES */

#if __GNUC__ && !__INTEL_COMPILER
#define ALIGN16(x) x __attribute__((aligned(16)))
#else
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define ALIGN16(x) __declspec(align(16)) x
#else
#define ALIGN16(x) x
#endif
#endif

#ifdef _WIN32
#define CDECL __cdecl
#ifdef _MSC_VER
#define inline __inline
#endif /* _MSC_VER */
#else
#define CDECL
#endif

#if __GNUC__
#define UNUSED(x) x __attribute__((unused))
#else
#define UNUSED(x) x
#endif

#ifdef CONFIG_DOUBLE
typedef double FLOAT;
#define FCONST(X) (X)
#define AFT_COS cos
#define AFT_SIN sin
#define AFT_TAN tan
#define AFT_LOG10 log10
#define AFT_EXP exp
#define AFT_FABS fabs
#define AFT_SQRT sqrt
#define AFT_EXP exp
#else
typedef float FLOAT;
#define FCONST(X) (X##f)
#define AFT_COS cosf
#define AFT_SIN sinf
#define AFT_TAN tanf
#define AFT_LOG10 log10f
#define AFT_FABS fabsf
#define AFT_SQRT sqrtf
#define AFT_EXP expf
#endif

#define AFT_PI  FCONST(3.14159265358979323846)
#define AFT_SQRT2 FCONST(1.41421356237309504880)
#define AFT_LN10 FCONST(2.30258509299404568402)

#define AFT_EXP10(x) AFT_EXP((x) * AFT_LN10)

#define ABS(a) ((a) >= 0 ? (a) : (-(a)))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define CLIP(x,min,max) MAX(MIN((x), (max)), (min))

#include "bswap.h"

#if 0 /* TIMER USED FOR TESTING */
static inline uint64_t read_time(void)
{
    uint32_t a, d;
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((uint64_t)d << 32) + a;
}

#define START_TIMER \
uint64_t tend;\
uint64_t tstart = read_time();\

#define STOP_TIMER(id) \
tend= read_time();\
{\
    static uint64_t tsum=0;\
    static int tcount=0;\
    static int tskip_count=0;\
    if(tcount<2 || tend - tstart < MAX(8*tsum/tcount, 2000)){\
        tsum+= tend - tstart;\
        tcount++;\
    }else\
        tskip_count++;\
    if(((tcount+tskip_count)&(tcount+tskip_count-1))==0){\
        fprintf(stderr, "%"PRIu64" dezicycles in %s, %d runs, %d skips\n",\
               tsum*10/tcount, id, tcount, tskip_count);\
    }\
}
#endif

#endif /* COMMON_H */
