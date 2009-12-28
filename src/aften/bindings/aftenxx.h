/********************************************************************************
 * Copyright (C) 2007 by Prakash Punnoor                                        *
 * prakash@punnoor.de                                                           *
 *                                                                              *
 * This library is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU Lesser General Public                   *
 * License as published by the Free Software Foundation; either                 *
 * version 2 of the License                                                     *
 *                                                                              *
 * This library is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * Lesser General Public License for more details.                              *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public             *
 * License along with this library; if not, write to the Free Software          *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA *
 ********************************************************************************/
namespace Aften
{
#include "aften-types.h"

#if defined(_WIN32) && !defined(_XBOX)
 #if defined(AFTENXX_BUILD_LIBRARY)
  #define AFTENXX_API __declspec(dllexport)
 #else
  #define AFTENXX_API __declspec(dllimport)
 #endif
#else
 #if defined(AFTENXX_BUILD_LIBRARY) && defined(HAVE_GCC_VISIBILITY)
  #define AFTENXX_API __attribute__((visibility("default")))
 #else
  #define AFTENXX_API
 #endif
#endif

class AFTENXX_API FrameEncoder
{
private:
    AftenContext m_context;

public:
    /// Constructor: Initializes the encoder with specified context
    FrameEncoder(AftenContext &context);

    /// Destructor: Deinitalizes the encoder
    ~FrameEncoder();

    /// Encodes PCM samples to an A/52 frame; returns encoded frame size
    int Encode(unsigned char *frameBuffer, const void *samples, int count);

    /// Gets a context with default values
    static AftenContext GetDefaultsContext();
};

class AFTENXX_API Utility
{
public:
    /// Determines the proper A/52 acmod and lfe parameters based on the
    /// number of channels and the WAVE_FORMAT_EXTENSIBLE channel mask.  If the
    /// channelMask value has the high bit set to 1 (e.g. 0xFFFFFFFF), then the default
    /// plain WAVE channel selection is assumed.
    /// On error, an execption is thrown. On success, the acmod and lfe params are set
    /// to appropriate values.
    static void WaveChannelsToAcmod(int channels, unsigned int channelMask, int &acmod, bool &hasLfe);

    /// overload for convenience
    static void WaveChannelsToAcmod(int channels, unsigned int channelMask, int &acmod, int &hasLfe);

    /// Takes a channel-interleaved array of audio samples, where the channel order
    /// is the default Wave order. The samples are rearranged to the proper A/52
    /// channel order based on the acmod and lfe parameters.
    static void RemapWaveToA52(void *samples, int samplesCount, int channels,
                               A52SampleFormat format, int acmod);

    /// Takes a channel-interleaved array of audio samples, where the channels
    /// are in MPEG order. The samples are rearranged to the proper A/52
    /// channel order based on the acmod parameter.
    static void RemapMpegToA52(void *samples, int samplesCount, int channels,
                               A52SampleFormat format, int acmod);

    /// Gets the libaften version string.
    static const char* GetVersion();

    /// Tells whether libaften was configured to use floats or doubles
    static FloatType GetFloatType();
};

#undef AFTENXX_API
}
