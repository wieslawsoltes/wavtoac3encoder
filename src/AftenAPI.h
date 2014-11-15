//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wiesław Šoltés <wieslaw.soltes@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "libaften/aften.h"

//
// AFTEN_API const char *aften_get_version(void);
//
typedef const char *(*lpLibAften_aften_get_version)(void);
const LPCSTR pszLibAften_aften_get_version = "aften_get_version";

//
// AFTEN_API void aften_set_defaults(AftenContext *s);
//
typedef void (*lpLibAften_aften_set_defaults)(AftenContext *s);
const LPCSTR pszLibAften_aften_set_defaults = "aften_set_defaults";

//
// AFTEN_API int aften_encode_init(AftenContext *s);
//
typedef int (*lpLibAften_aften_encode_init)(AftenContext *s);
const LPCSTR pszLibAften_aften_encode_init = "aften_encode_init";

//
// AFTEN_API int aften_encode_frame(AftenContext *s, unsigned char *frame_buffer,
//                                  const void *samples, int count);
//
typedef int (*lpLibAften_aften_encode_frame)(AftenContext *s, unsigned char *frame_buffer,
                                           const void *samples, int count);
const LPCSTR pszLibAften_aften_encode_frame = "aften_encode_frame";

//
// AFTEN_API void aften_encode_close(AftenContext *s);
//
typedef void (*lpLibAften_aften_encode_close)(AftenContext *s);
const LPCSTR pszLibAften_aften_encode_close = "aften_encode_close";

//
// AFTEN_API int aften_wav_channels_to_acmod(int ch, unsigned int chmask,
//                                           int *acmod, int *lfe);
//
typedef int (*lpLibAften_aften_wav_channels_to_acmod)(int ch, unsigned int chmask,
                                                    int *acmod, int *lfe);
const LPCSTR pszLibAften_aften_wav_channels_to_acmod = "aften_wav_channels_to_acmod";

//
// AFTEN_API void aften_remap_wav_to_a52(void *samples, int n, int ch,
//                                       A52SampleFormat fmt, int acmod);
//
typedef void (*lpLibAften_aften_remap_wav_to_a52)(void *samples, int n, int ch,
                                                A52SampleFormat fmt, int acmod);
const LPCSTR pszLibAften_aften_remap_wav_to_a52 = "aften_remap_wav_to_a52";

//
// AFTEN_API void aften_remap_mpeg_to_a52(void *samples, int n, int ch,
//                                        A52SampleFormat fmt, int acmod);

typedef void (*lpLibAften_aften_remap_mpeg_to_a52)(void *samples, int n, int ch,
                                                A52SampleFormat fmt, int acmod);
const LPCSTR pszLibAften_aften_remap_mpeg_to_a52 = "aften_remap_mpeg_to_a52";

//
// AFTEN_API FloatType aften_get_float_type(void);
//
typedef FloatType (*lpLibAften_aften_get_float_type)(void);
const LPCSTR pszLibAften_aften_get_float_type = "aften_get_float_type";

//
// structure with currently loaded aften api from dll
//
typedef struct TAftenAPI
{
    // path to currently loaded libaften.dll
    CString szLibPath;
    // handle to currently loaded libaften.dll library
    HMODULE hLibAften;
    // pointers to dynamically loaded aften api functions 
    lpLibAften_aften_get_version LibAften_aften_get_version;
    lpLibAften_aften_set_defaults LibAften_aften_set_defaults;
    lpLibAften_aften_encode_init LibAften_aften_encode_init;
    lpLibAften_aften_encode_frame LibAften_aften_encode_frame;
    lpLibAften_aften_encode_close LibAften_aften_encode_close;
    lpLibAften_aften_wav_channels_to_acmod LibAften_aften_wav_channels_to_acmod;
    lpLibAften_aften_remap_wav_to_a52 LibAften_aften_remap_wav_to_a52;
    lpLibAften_aften_remap_mpeg_to_a52 LibAften_aften_remap_mpeg_to_a52;
    lpLibAften_aften_get_float_type LibAften_aften_get_float_type;
} AftenAPI;

//
// initialize aften api from dll
//
bool OpenAftenAPI(AftenAPI *pAftenApi);

//
// clean-up aften api loaded from dll
//
void CloseAftenAPI(AftenAPI *pAftenApi);
