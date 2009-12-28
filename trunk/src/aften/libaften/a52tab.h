/**
 * Aften: A/52 audio encoder -  A/52 tables
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

#ifndef A52TAB_H
#define A52TAB_H

#include "common.h"

extern const uint16_t a52_frame_size_tab[38][3];
extern const uint8_t  a52_channels_tab[8];
extern const uint16_t a52_sample_rate_tab[3];
extern const uint16_t a52_bitrate_tab[19];
extern const uint8_t  a52_log_add_tab[260];
extern const uint16_t a52_hearing_threshold_tab[50][3];
extern const uint8_t  a52_bap_tab[64];
extern const uint8_t  a52_slow_decay_tab[4];
extern const uint8_t  a52_fast_decay_tab[4];
extern const uint16_t a52_slow_gain_tab[4];
extern const uint16_t a52_db_per_bit_tab[4];
extern const int16_t  a52_floor_tab[8];
extern const uint16_t a52_fast_gain_tab[8];
extern const uint8_t  a52_critical_band_size_tab[50];
extern const uint8_t  a52_expstr_set_tab[32][6];

#endif /* A52TAB_H */
