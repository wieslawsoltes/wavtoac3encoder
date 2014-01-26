/**
 * Aften: A/52 audio encoder -  Common code between encoder and decoder
 * Copyright (c) 2008 Prakash Punnoor <prakash@punnoor.de>
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
 * @file a52.c
 * Common code between A/52 encoder and decoder.
 */

#include "a52.h"

static uint8_t band_start_tab[51];
static uint8_t bin_to_band_tab[253];


static inline int calc_lowcomp1(int a, int b0, int b1, int c)
{
    if ((b0 + 256) == b1)
        a = c;
    else if (b0 > b1)
        a = MAX(a - 64, 0);
    return a;
}

static inline int calc_lowcomp(int a, int b0, int b1, int bin)
{
    if (bin < 7)
        return calc_lowcomp1(a, b0, b1, 384);
    else if (bin < 20)
        return calc_lowcomp1(a, b0, b1, 320);
    else
        return MAX(a - 128, 0);
}

void a52_bit_alloc_calc_psd(uint8_t *exp, int start, int end, int16_t *psd,
                               int16_t *band_psd)
{
    int bin, band;

    /* exponent mapping to PSD */
    for (bin = start; bin < end; bin++)
        psd[bin] = 3072 - (exp[bin] << 7);

    /* PSD integration */
    bin  = start;
    band = bin_to_band_tab[start];
    do {
        int v = psd[bin++];
        int band_end = MIN(band_start_tab[band+1], end);
        for (; bin < band_end; bin++) {
            /* logadd */
            int adr = MIN(ABS(v - psd[bin]) >> 1, 255);
            v = MAX(v, psd[bin]) + a52_log_add_tab[adr];
        }
        band_psd[band++] = v;
    } while (end > band_start_tab[band]);
}

void a52_bit_alloc_calc_mask(A52BitAllocParams *s, int16_t *band_psd,
                             int start, int end, int fast_gain,
                             int dba_mode, int dba_nsegs, uint8_t *dba_offsets,
                             uint8_t *dba_lengths, uint8_t *dba_values,
                             int16_t *mask)
{
    int16_t excite[50]; /* excitation */
    int band;
    int band_start, band_end, begin, end1;
    int lowcomp, fastleak, slowleak;

    /* excitation function */
    band_start = bin_to_band_tab[start];
    band_end = bin_to_band_tab[end-1] + 1;

    if (band_start == 0) {
        lowcomp = 0;
        lowcomp = calc_lowcomp1(lowcomp, band_psd[0], band_psd[1], 384);
        excite[0] = band_psd[0] - fast_gain - lowcomp;
        lowcomp = calc_lowcomp1(lowcomp, band_psd[1], band_psd[2], 384);
        excite[1] = band_psd[1] - fast_gain - lowcomp;
        begin = 7;
        for (band = 2; band < 7; band++) {
            if (!(band_end == 7 && band == 6))
                lowcomp = calc_lowcomp1(lowcomp, band_psd[band], band_psd[band+1], 384);
            fastleak = band_psd[band] - fast_gain;
            slowleak = band_psd[band] - s->sgain;
            excite[band] = fastleak - lowcomp;
            if (!(band_end == 7 && band == 6)) {
                if (band_psd[band] <= band_psd[band+1]) {
                    begin = band + 1;
                    break;
                }
            }
        }

        end1 = MIN(band_end, 22);

        for (band = begin; band < end1; band++) {
            if (!(band_end == 7 && band == 6))
                lowcomp = calc_lowcomp(lowcomp, band_psd[band], band_psd[band+1], band);

            fastleak = MAX(fastleak - s->fdecay, band_psd[band] - fast_gain);
            slowleak = MAX(slowleak - s->sdecay, band_psd[band] - s->sgain);
            excite[band] = MAX(fastleak - lowcomp, slowleak);
        }
        begin = 22;
    } else {
        /* coupling channel */
        begin = band_start;
        fastleak = (s->cplfleak << 8) + 768;
        slowleak = (s->cplsleak << 8) + 768;
    }

    for (band = begin; band < band_end; band++) {
        fastleak = MAX(fastleak - s->fdecay, band_psd[band] - fast_gain);
        slowleak = MAX(slowleak - s->sdecay, band_psd[band] - s->sgain);
        excite[band] = MAX(fastleak, slowleak);
    }

    /* compute masking curve */

    for (band = band_start; band < band_end; band++) {
        int tmp = s->dbknee - band_psd[band];
        if (tmp > 0)
            excite[band] += tmp >> 2;
        mask[band] = MAX(a52_hearing_threshold_tab[band >> s->halfratecod][s->fscod], excite[band]);
    }

    /* delta bit allocation */
    if (dba_mode == DBA_REUSE || dba_mode == DBA_NEW) {
        int i, band, seg, delta;
        dba_nsegs = MIN(dba_nsegs, 8);
        band = 0;
        for (seg = 0; seg < dba_nsegs; seg++) {
            band += dba_offsets[seg];
            if (dba_values[seg] >= 4)
                delta = (dba_values[seg] - 3) << 7;
            else
                delta = (dba_values[seg] - 4) << 7;
            for (i = 0; i < dba_lengths[seg] && band < 50; i++)
                mask[band++] += delta;
        }
    }
}

/**
 * A52 bit allocation
 * Generate bit allocation pointers for each mantissa, which determines the
 * number of bits allocated for each mantissa.  The fine-grain power-spectral
 * densities and the masking curve have been pre-generated in the preparation
 * step.  They are used along with the given snroffset and floor values to
 * calculate each bap value.
 */
void a52_bit_alloc_calc_bap(int16_t *mask, int16_t *psd, int start, int end,
                            int snr_offset, int floor, uint8_t *bap)
{
    int bin, band;

    // special case, if snr offset is -960, set all bap's to zero
    if (snr_offset == -960) {
        memset(bap, 0, 256);
        return;
    }

    bin = start;
    band = bin_to_band_tab[start];
    do {
        int m = (MAX(mask[band] - snr_offset - floor, 0) & 0x1FE0) + floor;
        int band_end = MIN(band_start_tab[band+1], end);
        if ((band_end - bin) & 1) {
            int address = CLIP((psd[bin] - m) >> 5, 0, 63);
            bap[bin++] = a52_bap_tab[address];
        }
        while (bin < band_end) {
            int address1 = CLIP((psd[bin  ] - m) >> 5, 0, 63);
            int address2 = CLIP((psd[bin+1] - m) >> 5, 0, 63);
            bap[bin  ] = a52_bap_tab[address1];
            bap[bin+1] = a52_bap_tab[address2];
            bin += 2;
        }
    } while (end > band_start_tab[band++]);
}

/**
 * Initializes some tables.
 */
void a52_common_init(void)
{
    /* populate band_start_tab[] and bin_to_band_tab[]
       from a52_critical_band_size_tab[] */
    int bin = 0, band;
    for (band = 0; band < 50; band++) {
        int band_end = bin + a52_critical_band_size_tab[band];
        band_start_tab[band] = bin;
        while (bin < band_end)
            bin_to_band_tab[bin++] = band;
    }
    band_start_tab[50] = bin;
}
