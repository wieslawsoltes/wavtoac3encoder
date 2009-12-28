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

/**
 * @file a52tab.c
 * tables taken from ATSC A/52 spec.
 */

#include "a52tab.h"
#include "a52.h"

/**
 * Possible frame sizes in bits
 * derived from ATSC A/52 Table 5.18 Frame Size Code Table.
 */
const uint16_t a52_frame_size_tab[38][3] = {
    { 1024,  1104,  1536},
    { 1024,  1120,  1536},
    { 1280,  1392,  1920},
    { 1280,  1408,  1920},
    { 1536,  1664,  2304},
    { 1536,  1680,  2304},
    { 1792,  1936,  2688},
    { 1792,  1952,  2688},
    { 2048,  2224,  3072},
    { 2048,  2240,  3072},
    { 2560,  2784,  3840},
    { 2560,  2800,  3840},
    { 3072,  3328,  4608},
    { 3072,  3344,  4608},
    { 3584,  3888,  5376},
    { 3584,  3904,  5376},
    { 4096,  4448,  6144},
    { 4096,  4464,  6144},
    { 5120,  5568,  7680},
    { 5120,  5584,  7680},
    { 6144,  6672,  9216},
    { 6144,  6688,  9216},
    { 7168,  7792, 10752},
    { 7168,  7808, 10752},
    { 8192,  8912, 12288},
    { 8192,  8928, 12288},
    {10240, 11136, 15360},
    {10240, 11152, 15360},
    {12288, 13360, 18432},
    {12288, 13376, 18432},
    {14336, 15600, 21504},
    {14336, 15616, 21504},
    {16384, 17824, 24576},
    {16384, 17840, 24576},
    {18432, 20048, 27648},
    {18432, 20064, 27648},
    {20480, 22288, 30720},
    {20480, 22304, 30720}
};

/**
 * Maps audio coding mode (acmod) to number of full-bandwidth channels.
 * from ATSC A/52 Table 5.8 Audio Coding Mode
 */
const uint8_t a52_channels_tab[8] = {
    2, 1, 2, 3, 3, 4, 4, 5
};

/* possible frequencies */
const uint16_t a52_sample_rate_tab[3] = { 48000, 44100, 32000 };

/* possible bitrates */
const uint16_t a52_bitrate_tab[19] = {
    32, 40, 48, 56, 64, 80, 96, 112, 128,
    160, 192, 224, 256, 320, 384, 448, 512, 576, 640
};

/**
 * log addition table
 */
const uint8_t a52_log_add_tab[260]= {
    64, 63, 62, 61, 60, 59, 58, 57, 56, 55,
    54, 53, 52, 52, 51, 50, 49, 48, 47, 47,
    46, 45, 44, 44, 43, 42, 41, 41, 40, 39,
    38, 38, 37, 36, 36, 35, 35, 34, 33, 33,
    32, 32, 31, 30, 30, 29, 29, 28, 28, 27,
    27, 26, 26, 25, 25, 24, 24, 23, 23, 22,
    22, 21, 21, 21, 20, 20, 19, 19, 19, 18,
    18, 18, 17, 17, 17, 16, 16, 16, 15, 15,
    15, 14, 14, 14, 13, 13, 13, 13, 12, 12,
    12, 12, 11, 11, 11, 11, 10, 10, 10, 10,
    10,  9,  9,  9,  9,  9,  8,  8,  8,  8,
     8,  8,  7,  7,  7,  7,  7,  7,  6,  6,
     6,  6,  6,  6,  6,  6,  5,  5,  5,  5,
     5,  5,  5,  5,  4,  4,  4,  4,  4,  4,
     4,  4,  4,  4,  4,  3,  3,  3,  3,  3,
     3,  3,  3,  3,  3,  3,  3,  3,  3,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

/**
 * absolute hearing threshold table
 * values are in log-psd units (128 psd = -6dB)
 * each table entry corresponds to a critical frequency band
 * each entry has 3 values, 1 for each base sample rate
 * { 48kHz, 44.1kHz, 32kHz }
 */
const uint16_t a52_hearing_threshold_tab[50][3]= {
    { 1232, 1264, 1408 },
    { 1232, 1264, 1408 },
    { 1088, 1120, 1200 },
    { 1024, 1040, 1104 },
    {  992,  992, 1056 },
    {  960,  976, 1008 },
    {  944,  960,  992 },
    {  944,  944,  976 },
    {  928,  944,  960 },
    {  928,  928,  944 },
    {  928,  928,  944 },
    {  928,  928,  944 },
    {  928,  928,  928 },
    {  912,  928,  928 },
    {  912,  912,  928 },
    {  912,  912,  928 },
    {  896,  912,  928 },
    {  896,  896,  928 },
    {  880,  896,  928 },
    {  880,  896,  928 },
    {  864,  880,  912 },
    {  864,  880,  912 },
    {  848,  864,  912 },
    {  848,  864,  912 },
    {  832,  848,  896 },
    {  832,  848,  896 },
    {  816,  832,  896 },
    {  800,  832,  880 },
    {  784,  800,  864 },
    {  768,  784,  848 },
    {  752,  768,  832 },
    {  752,  752,  816 },
    {  752,  752,  800 },
    {  752,  752,  784 },
    {  768,  752,  768 },
    {  784,  768,  752 },
    {  832,  800,  752 },
    {  912,  848,  752 },
    {  992,  912,  768 },
    { 1056,  992,  784 },
    { 1120, 1056,  816 },
    { 1168, 1104,  848 },
    { 1184, 1184,  960 },
    { 1120, 1168, 1040 },
    { 1088, 1120, 1136 },
    { 1088, 1088, 1184 },
    { 1312, 1152, 1120 },
    { 2048, 1584, 1088 },
    { 2112, 2112, 1104 },
    { 2112, 2112, 1248 },
};

/**
 * bit allocation pointer table
 */
const uint8_t a52_bap_tab[64]= {
    0, 1, 1, 1, 1, 1, 2, 2, 3, 3,
    3, 4, 4, 5, 5, 6, 6, 6, 6, 7,
    7, 7, 7, 8, 8, 8, 8, 9, 9, 9,
    9, 10, 10, 10, 10, 11, 11, 11, 11, 12,
    12, 12, 12, 13, 13, 13, 13, 14, 14, 14,
    14, 14, 14, 14, 14, 15, 15, 15, 15, 15,
    15, 15, 15, 15,
};

/**
 * slow decay table
 */
const uint8_t a52_slow_decay_tab[4]={
    15, 17, 19, 21
};

/**
 * fast decay table
 */
const uint8_t a52_fast_decay_tab[4]={
    63, 83, 103, 123
};

/**
 * slow gain table
 */
const uint16_t a52_slow_gain_tab[4]= {
    1344, 1240, 1144, 1040
};

/**
 * dB per bit table
 */
const uint16_t a52_db_per_bit_tab[4]= {
    0, 1792, 2304, 2816
};

/**
 * floor table
 */
const int16_t a52_floor_tab[8]= {
    752, 688, 624, 560, 496, 368, 240, -2048
};

/**
 * fast gain table
 */
const uint16_t a52_fast_gain_tab[8]= {
    128, 256, 384, 512, 640, 768, 896, 1024
};

/**
 * band size table (number of bins in each band)
 */
const uint8_t a52_critical_band_size_tab[50]={
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3,
    3, 6, 6, 6, 6, 6, 6, 12, 12, 12, 12, 24, 24, 24, 24, 24
};

/**
 * Pre-defined sets of exponent strategies. A strategy set is selected for
 * each channel in a frame.
 */
const uint8_t a52_expstr_set_tab[32][6] = {
    { EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE, EXP_REUSE, EXP_REUSE },
    { EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE, EXP_REUSE,   EXP_D45 },
    { EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE,   EXP_D25, EXP_REUSE },
    { EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE,   EXP_D45,   EXP_D45 },
    { EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D25, EXP_REUSE, EXP_REUSE },
    { EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D25, EXP_REUSE,   EXP_D45 },
    { EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D45,   EXP_D25, EXP_REUSE },
    { EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D45,   EXP_D45,   EXP_D45 },
    { EXP_D25, EXP_REUSE,   EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE },
    { EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D45 },
    { EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE },
    { EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D45 },
    { EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D25, EXP_REUSE, EXP_REUSE },
    { EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D45 },
    { EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE },
    { EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D45,   EXP_D45,   EXP_D45 },
    { EXP_D45,   EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE, EXP_REUSE },
    { EXP_D45,   EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE,   EXP_D45 },
    { EXP_D45,   EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D25, EXP_REUSE },
    { EXP_D45,   EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D45,   EXP_D45 },
    { EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE, EXP_REUSE },
    { EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE,   EXP_D45 },
    { EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D25, EXP_REUSE },
    { EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D45,   EXP_D45 },
    { EXP_D45,   EXP_D45,   EXP_D15, EXP_REUSE, EXP_REUSE, EXP_REUSE },
    { EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE, EXP_REUSE,   EXP_D45 },
    { EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D25, EXP_REUSE },
    { EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D45,   EXP_D45 },
    { EXP_D45,   EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE, EXP_REUSE },
    { EXP_D45,   EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE,   EXP_D45 },
    { EXP_D45,   EXP_D45,   EXP_D45,   EXP_D45,   EXP_D25, EXP_REUSE },
    { EXP_D45,   EXP_D45,   EXP_D45,   EXP_D45,   EXP_D45,   EXP_D45 }
};
