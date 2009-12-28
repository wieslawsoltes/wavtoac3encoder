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
 * file format registration and detection
 */

#include "formats.h"

#define REGISTER_FORMAT(x) { \
    extern PcmFormat x##_format; \
    pcmfile_register_format(&x##_format); }

void
pcmfile_register_all_formats(void)
{
    static int inited;

    if (inited)
        return;
    inited = 1;

    REGISTER_FORMAT(raw);
    REGISTER_FORMAT(wave);
    REGISTER_FORMAT(aiff);
    REGISTER_FORMAT(caff);
}

PcmFormat *first_format = NULL;

void
pcmfile_register_format(PcmFormat *format)
{
    PcmFormat **p;
    p = &first_format;
    while (*p != NULL)
        p = &(*p)->next;
    *p = format;
    format->next = NULL;
}

PcmFormat *
pcmfile_find_format(int format)
{
    PcmFormat *fmt;
    for (fmt = first_format; fmt != NULL; fmt = fmt->next) {
        if (fmt->format == format)
            return fmt;
    }
    return NULL;
}

PcmFormat *
pcmfile_probe_format(uint8_t *data, int size)
{
    PcmFormat *fmt1, *fmt;
    int score, score_max;

    score_max = 0;
    fmt = NULL;
    for (fmt1 = first_format; fmt1 != NULL; fmt1 = fmt1->next) {
        score = 0;
        if (fmt1->probe)
            score = fmt1->probe(data, size);
        if (score > score_max) {
            score_max = score;
            fmt = fmt1;
        }
    }
    return fmt;
}
