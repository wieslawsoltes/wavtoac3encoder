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
 * @file util.c
 * libaften utility functions
 */

#include "aften.h"
#include "a52.h"

static const int8_t ch_to_acmod[6] = {
    A52_ACMOD_MONO,
    A52_ACMOD_STEREO,
    A52_ACMOD_3_0,
    A52_ACMOD_2_2,
    A52_ACMOD_3_2,
    A52_ACMOD_3_2
};

const uint8_t log2tab[256] = {
    0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};


static int
chmask_get_num_channels(unsigned int chmask)
{
    int count;
    for (count = 0; chmask; count++)
        chmask &= chmask-1; // unset lowest set bit
    return count;
}

/**
 * Determines the proper A/52 acmod and lfe parameters based on the
 * number of channels and the WAVE_FORMAT_EXTENSIBLE channel mask.  If the
 * chmask value has the high bit set to 1 (e.g. 0xFFFFFFFF), then the default
 * plain WAVE channel selection is assumed.
 * On error, the function returns -1.  On success, the acmod and lfe params are
 * set to appropriate values and the function returns 0.
 */
int
aften_wav_channels_to_acmod(int ch, unsigned int chmask, int *acmod, int *lfe)
{
    int tmp_lfe, tmp_acmod;

    // check for valid number of channels
    if (ch < 1 || ch > A52_MAX_CHANNELS) {
        fprintf(stderr, "Unsupported # of channels passed to aften_wav_chmask_to_acmod\n");
        return -1;
    }

    chmask &= 0x7FFFFFFF;
    if (!chmask) {
        // set values for plain WAVE format or unknown configuration
        tmp_lfe = (ch == 6);
        if (tmp_lfe)
            ch--;
        tmp_acmod = ch_to_acmod[ch-1];
    } else {
        // check number of channel bits set in chmask and compare to ch
        if (chmask_get_num_channels(chmask) != ch) {
            fprintf(stderr, "channel mask and number of channels do not match\n");
            return -1;
        }

        // read chmask value for LFE channel
        tmp_lfe = !!(chmask & 0x08);
        if (tmp_lfe)
            chmask -= 0x08;

        // check for fbw channel layouts which are compatible with A/52
        switch (chmask) {
        case 0x004: tmp_acmod = A52_ACMOD_MONO;     break;
        case 0x003: tmp_acmod = A52_ACMOD_STEREO;   break;
        case 0x007: tmp_acmod = A52_ACMOD_3_0;      break;
        case 0x103: tmp_acmod = A52_ACMOD_2_1;      break;
        case 0x107: tmp_acmod = A52_ACMOD_3_1;      break;
        case 0x033:
        case 0x603: tmp_acmod = A52_ACMOD_2_2;      break;
        case 0x037:
        case 0x607: tmp_acmod = A52_ACMOD_3_2;      break;
        default:    tmp_acmod = ch_to_acmod[ch-1];  break;
        }
    }

    if (acmod)
        *acmod = tmp_acmod;
    if (lfe)
        *lfe = tmp_lfe;
    return 0;
}

/**
 * WAV to A/52 channel mapping
 * note: thanks to Tebasuna for help in getting this order right.
 */

/**
 * Table to remap channels from WAV order to A/52 order.
 * [acmod][lfe][ch]
 */
static const uint8_t wav_chmap[8][2][6] = {
    { { 0, 1,          }, { 0, 1, 2,         } },
    { { 0,             }, { 0, 1,            } },
    { { 0, 1,          }, { 0, 1, 2,         } },
    { { 0, 2, 1,       }, { 0, 2, 1, 3,      } },
    { { 0, 1, 2,       }, { 0, 1, 3, 2,      } },
    { { 0, 2, 1, 3,    }, { 0, 2, 1, 4, 3,   } },
    { { 0, 1, 2, 3, 4, }, { 0, 1, 3, 4, 2,   } },
    { { 0, 2, 1, 3, 4, }, { 0, 2, 1, 4, 5, 3 } },
};

#define REMAP_WAV_TO_A52_COMMON(DATA_TYPE) \
{ \
    int i, j; \
    DATA_TYPE *smp = samples; \
    DATA_TYPE tmp[6]; \
    int sample_size = sizeof(DATA_TYPE); \
        for (i = 0; i < n*ch; i += ch) { \
            memcpy(tmp, &smp[i], ch*sample_size); \
            for (j = 0; j < ch; j++) \
                smp[i+j] = tmp[wav_chmap[acmod][lfe][j]]; \
        } \
}

void
aften_remap_wav_to_a52(void *samples, int n, int ch, A52SampleFormat fmt,
                       int acmod)
{
    int lfe;

    if (samples == NULL) {
        fprintf(stderr, "NULL parameter passed to aften_remap_wav_to_a52\n");
        return;
    }

    lfe = 0;
    if (ch > a52_channels_tab[acmod])
        lfe = 1;
    if (acmod < 3 || ((acmod == 4 || acmod == 6) && !lfe))
        return;

    switch (fmt) {
        case A52_SAMPLE_FMT_U8:  REMAP_WAV_TO_A52_COMMON(uint8_t)
                                 break;
        case A52_SAMPLE_FMT_S8:  REMAP_WAV_TO_A52_COMMON(int8_t)
                                 break;
        case A52_SAMPLE_FMT_S16: REMAP_WAV_TO_A52_COMMON(int16_t)
                                 break;
        case A52_SAMPLE_FMT_S20:
        case A52_SAMPLE_FMT_S24:
        case A52_SAMPLE_FMT_S32: REMAP_WAV_TO_A52_COMMON(int32_t)
                                 break;
        case A52_SAMPLE_FMT_FLT: REMAP_WAV_TO_A52_COMMON(float)
                                 break;
        case A52_SAMPLE_FMT_DBL: REMAP_WAV_TO_A52_COMMON(double)
                                 break;
    }
}

/**
 * MPEG to A/52 channel mapping
 * Can be used for multi-channel DTS, MP2, or AAC
 */

#define REMAP_MPEG_TO_A52_COMMON(DATA_TYPE) \
{ \
    int i; \
    DATA_TYPE *smp = samples; \
    for (i = 0; i < n*ch; i += ch) { \
        DATA_TYPE tmp = smp[i]; \
        smp[i] = smp[i+1]; \
        smp[i+1] = tmp; \
    } \
}

void
aften_remap_mpeg_to_a52(void *samples, int n, int ch, A52SampleFormat fmt,
                        int acmod)
{
    if (samples == NULL) {
        fprintf(stderr, "NULL parameter passed to aften_remap_mpeg_to_a52\n");
        return;
    }

    if (acmod <= 2 || !(acmod & 1))
        return;

    switch (fmt) {
        case A52_SAMPLE_FMT_U8:  REMAP_MPEG_TO_A52_COMMON(uint8_t)
                                 break;
        case A52_SAMPLE_FMT_S8:  REMAP_MPEG_TO_A52_COMMON(int8_t)
                                 break;
        case A52_SAMPLE_FMT_S16: REMAP_MPEG_TO_A52_COMMON(int16_t)
                                 break;
        case A52_SAMPLE_FMT_S20:
        case A52_SAMPLE_FMT_S24:
        case A52_SAMPLE_FMT_S32: REMAP_MPEG_TO_A52_COMMON(int32_t)
                                 break;
        case A52_SAMPLE_FMT_FLT: REMAP_MPEG_TO_A52_COMMON(float)
                                 break;
        case A52_SAMPLE_FMT_DBL: REMAP_MPEG_TO_A52_COMMON(double)
                                 break;
    }
}

FloatType
aften_get_float_type(void)
{
#ifdef CONFIG_DOUBLE
    return FLOAT_TYPE_DOUBLE;
#else
    return FLOAT_TYPE_FLOAT;
#endif
}
