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
#include "aftenxx.h"

namespace Aften
{
#include "aften.h"

/// Constructor: Initializes the encoder with specified context
FrameEncoder::FrameEncoder(AftenContext &context)
{
    m_context = context;
    int nReturnValue = aften_encode_init(&m_context);
    if (nReturnValue > 0)
        throw nReturnValue;
}

/// Destructor: Deinitalizes the encoder
FrameEncoder::~FrameEncoder()
{
    aften_encode_close(&m_context);
}

/// Encodes PCM samples to an A/52 frame
int FrameEncoder::Encode(unsigned char *frameBuffer, const void *samples, int count)
{
    return aften_encode_frame(&m_context, frameBuffer, samples, count);
}

/// Gets a context with default values
AftenContext FrameEncoder::GetDefaultsContext()
{
    AftenContext context;
    aften_set_defaults(&context);
    return context;
}

/// Determines the proper A/52 acmod and lfe parameters based on the
/// number of channels and the WAVE_FORMAT_EXTENSIBLE channel mask.  If the
/// channelMask value has the high bit set to 1 (e.g. 0xFFFFFFFF), then the default
/// plain WAVE channel selection is assumed.
/// On error, an execption is thrown. On success, the acmod and lfe params are set
/// to appropriate values.
void Utility::WaveChannelsToAcmod(int channels, unsigned int channelMask, int &acmod, bool &hasLfe)
{
    int nLfe;
    if (aften_wav_channels_to_acmod(channels, channelMask, &acmod, &nLfe) != 0)
        throw -1;
    hasLfe = (nLfe != 0);
}

/// overload for convenience
void Utility::WaveChannelsToAcmod(int channels, unsigned int channelMask, int &acmod, int &hasLfe)
{
    if (aften_wav_channels_to_acmod(channels, channelMask, &acmod, &hasLfe) != 0)
        throw -1;
}

/// Takes a channel-interleaved array of audio samples, where the channel order
/// is the default Wave order. The samples are rearranged to the proper A/52
/// channel order based on the acmod and lfe parameters.
void Utility::RemapWaveToA52(void *samples, int samplesCount, int channels,
                             A52SampleFormat format, int acmod)
{
    aften_remap_wav_to_a52(samples, samplesCount, channels, format, acmod);
}

/// Takes a channel-interleaved array of audio samples, where the channels are
/// in MPEG order. The samples are rearranged to the proper A/52 channel order
/// based on the acmod parameter.
void Utility::RemapMpegToA52(void *samples, int samplesCount, int channels,
                             A52SampleFormat format, int acmod)
{
    aften_remap_mpeg_to_a52(samples, samplesCount, channels, format, acmod);
}

const char* Utility::GetVersion()
{
    return aften_get_version();
}

/// Tells whether libaften was configured to use floats or doubles
FloatType Utility::GetFloatType()
{
    return aften_get_float_type();
}
}
