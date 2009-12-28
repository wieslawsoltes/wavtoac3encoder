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
 * @file pcm.h
 * raw PCM decoder header
 */

#ifndef PCM_H
#define PCM_H

#include "pcmfile.h"

typedef struct PcmContext {
    PcmFile pcm_file[PCM_MAX_CHANNELS]; ///< source files
    int num_files;                      ///< number of source files

    uint64_t samples;   ///< maximum number of samples for all files
    int channels;       ///< number of channels
    int ch_mask;        ///< channel mask. tells which channels are present
    int sample_rate;    ///< sample rate for all files

    int read_to_eof;    ///< indicates that data is to be read until EOF
    int read_format;    ///< sample type to convert to when reading
} PcmContext;

/**
 * Initializes PcmContext structure using the given input file pointers.
 * Examines the header (if present) to get audio information and has the file
 * pointer aligned at start of data when it exits.
 *
 * @param pc           The PcmContext to initialize
 * @param num_files    Number of source files
 * @param fp_list      Array of file pointers to source files
 * @param read_format  Sample format to convert to when reading
 * @param file_format  Source file format, or PCM_FORMAT_UNKNOWN to autodetect
 *
 * @return non-zero value if an error occurs.
 */
extern int pcm_init(PcmContext *pc, int num_files, FILE **fp_list,
                    enum PcmSampleFormat read_format, int file_format);

/**
 * Frees memory from internal buffer.
 */
extern void pcm_close(PcmContext *pc);

/**
 * Sets the source sample format for all files
 */
extern void pcm_set_source_format(PcmContext *pc, enum PcmSampleFormat fmt);

/**
 * Sets source audio information for all files
 */
extern void pcm_set_source_params(PcmContext *pc, int ch, enum PcmSampleFormat fmt, int order, int sr);

/**
 * Sets source audio information for all files
 */
extern void pcm_set_sample_rate(PcmContext *pc, int sample_rate);

/**
 * Sets the read_to_eof parameter for all files
 */
extern void pcm_set_read_to_eof(PcmContext *pc, int read_to_eof);

/**
 * Sets the requested read format
 */
extern void pcm_set_read_format(PcmContext *pc, enum PcmSampleFormat read_format);

/**
 * Prints out a description of the pcm format to the specified
 * output stream.
 */
extern void pcm_print(PcmContext *pc, FILE *st);

/**
 * Reads audio samples to the output buffer.
 * Output is channel-interleaved, native byte order.
 * Only up to PCM_MAX_READ samples can be read in one call.
 * The output sample format depends on the value of pc->read_format.
 * Returns number of samples read or -1 on error.
 */
extern int pcm_read_samples(PcmContext *pc, void *buffer, int num_samples);

#endif /* PCM_H */
