/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2007 Justin Ruggles
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
 * @file opts.h
 * Commandline options header
 */

#ifndef OPTS_H
#define OPTS_H

#include <stdio.h>

#include "aften.h"
#include "pcm.h"

#define A52_NUM_SPEAKERS 9

typedef struct {
    int chmap;
    int num_input_files;
    char *infile[A52_NUM_SPEAKERS];
    char *outfile;
    AftenContext *s;
    int pad_start;
    int read_to_eof;
    int raw_input;
    enum PcmSampleFormat raw_fmt;
    int raw_order;
    int raw_sr;
    int raw_ch;
} CommandOptions;

extern int parse_commandline(int argc, char **argv, CommandOptions *opts);

#endif /* OPTS_H */
