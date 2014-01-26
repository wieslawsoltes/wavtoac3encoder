/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2007 Justin Ruggles
 *
 * Basic concept and code structure for formats is based on FFmpeg libavformat
 * Copyright (c) 2000-2007 Fabrice Bellard, et al.
 *
 * Aften is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * Aften is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Aften; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file formats.c
 * file format registration and detection header
 */

#ifndef FORMATS_H
#define FORMATS_H

#include "common.h"

/* supported file formats */
enum PcmFileFormat {
    PCM_FORMAT_UNKNOWN = -1,
    PCM_FORMAT_RAW     =  0,
    PCM_FORMAT_WAVE    =  1,
    PCM_FORMAT_AIFF    =  2,
    PCM_FORMAT_CAFF    =  3
};

struct PcmFile;

typedef struct PcmFormat {
    const char *name;
    const char *long_name;
    int format;
    int (*probe)(uint8_t *data, int size);
    int (*init)(struct PcmFile *pf);
    struct PcmFormat *next;
} PcmFormat;

extern void pcmfile_register_all_formats(void);

extern void pcmfile_register_format(PcmFormat *format);

extern PcmFormat *pcmfile_find_format(int format);

extern PcmFormat *pcmfile_probe_format(uint8_t *data, int size);

#endif /* FORMATS_H */
