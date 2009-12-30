//
// WAV to AC3 Encoder
// Copyright (C) 2007, 2008, 2009 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

/* $Id$ */

#include "stdafx.h"
#include "EncoderOptions.h"
#include "ProgramConfig.h"
#include "Utilities.h"
#include "MyFile.h"

// valid CBR mode bitrates
const int nValidCbrBitrates[nNumValidCbrBitrates] = 
{
       0,  32,  40,  48,  56,  64,  80,  96, 112, 128,
     160, 192, 224, 256, 320, 384, 448, 512, 576, 640
};

const ChannelConfig ccAften[nNumChannelConfigAften] = 
{
    { 0, 0, _T("1+1") },
    { 1, 0, _T("1/0") },
    { 2, 0, _T("2/0") },
    { 3, 0, _T("3/0") },
    { 4, 0, _T("2/1") },
    { 5, 0, _T("3/1") },
    { 6, 0, _T("2/2") },
    { 7, 0, _T("3/2") },
    { 0, 1, _T("1+1") },
    { 1, 1, _T("1/0") },
    { 2, 1, _T("2/0") },
    { 3, 1, _T("3/0") },
    { 4, 1, _T("2/1") },
    { 5, 1, _T("3/1") },
    { 6, 1, _T("2/2") },
    { 7, 1, _T("3/2") }
};

// default values for raw audio input
const LPTSTR szRawSampleFormats[nNumRawSampleFormats] =
{
    DEFAULT_TEXT_IGNORED,
    _T("u8"),
    _T("s8"),
    _T("s16_le"),
    _T("s16_be"),
    _T("s20_le"),
    _T("s20_be"),
    _T("s24_le"),
    _T("s24_be"),
    _T("s32_le"),
    _T("s32_be"),
    _T("float_le"),
    _T("float_be"),
    _T("double_le"),
    _T("double_be")
};

// advanced options global variable
EncoderOptions encOpt[nNumEncoderOptions];

// encoder options groups
const LPWSTR pszGroups[nNumEncoderOptionsGroups] = 
{
	L"Encoding options",
	L"Bitstream info metadata",
	L"Dynamic range compression and dialog normalization",
	L"Input options",
	L"Input filters",
	L"Alternate bit stream syntax"
};

// option for CBR encoding mode for aften
/*
[-b #]         CBR bitrate in kbps (default: about 96kbps per channel)
*/
const CString szCbrOption = _T("-b");

// option for VBR encoding mode for aften
/*
[-q #]         VBR quality [0 - 1023] (default: 240)
*/
const CString szVbrOption = _T("-q");

// option for number of threads for aften
/*
[-threads #]   Number of parallel threads to use
                   0 = detect number of CPUs (default)
*/
const CString szThreadsOption = _T("-threads");

// option for SIMD instructions for aften
/*
[-nosimd X]    Comma-separated list of SIMD instruction sets not to use
                   Available sets are mmx, sse, sse2, sse3 and altivec.
                   No spaces are allowed between the sets and the commas.
*/
const CString szSimdOption = _T("-nosimd");

// option for raw audio input sample format for aften
/*
[-raw_fmt X]   Raw audio input sample format (default: s16_le)
                   One of the pre-defined sample formats:
                   u8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,
                   s32_le, s32_be, float_le, float_be, double_le, double_be
*/
const CString szRawSampleFormatOption = _T("-raw_fmt");

// option for raw audio input sample rate for aften
/*
[-raw_sr #]    Raw audio input sample rate (default: 48000)
*/
const CString szRawSampleRateOption = _T("-raw_sr");


// option for raw audio input channels for aften
/*
[-raw_ch #]    Raw audio input channels (default: 2)
*/
const CString szRawChannelsOption = _T("-raw_ch");

// list of encoder presets
CList<EncoderPreset,EncoderPreset&> encPresets;

// default preset
EncoderPreset defaultPreset;

// current preset index
int nCurrentPreset = 0;

// supported input file extensions
const TCHAR szSupportedInputExt[NUM_SUPPORTED_INPUT_EXT][8] = 
{
    _T("wav"),
    _T("pcm"),
    _T("raw"),
    _T("bin"),
    _T("aiff"),
    _T("aif"),
    _T("aifc"),
#ifndef DISABLE_AVISYNTH
    _T("avs")
#endif
};

// supported input file formats
const int nSupportedInputFormats[NUM_SUPPORTED_INPUT_EXT] = 
{
    PCM_FORMAT_WAVE,
    PCM_FORMAT_RAW,
    PCM_FORMAT_RAW,
    PCM_FORMAT_RAW,
    PCM_FORMAT_AIFF,
    PCM_FORMAT_AIFF,
    PCM_FORMAT_CAFF,

};

// supported output file extensions
const TCHAR szSupportedOutputExt[NUM_SUPPORTED_OUTPUT_EXT][8] = 
{
    _T("ac3")
};

EncoderPreset GetCurrentPreset()
{
    return ::encPresets.GetAt(::encPresets.FindIndex(::nCurrentPreset));
}

void UpdateCurrentPreset(EncoderPreset updatePreset)
{
    ::encPresets.SetAt(::encPresets.FindIndex(::nCurrentPreset), updatePreset);
}

int FindValidBitratePos(const int nBitrate)
{
    for(int i = 0; i < nNumValidCbrBitrates; i++)
    {
        if(nValidCbrBitrates[i] == nBitrate)
            return i;
    }

    return 0;
}

int FindOptionIndex(CString szOption)
{
    for(int i = 0; i < nNumEncoderOptions; i++)
    {
        CString szBuffer = encOpt[i].szOption;
        if(szOption.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            return i;
        }
    }

    return 0;
}

void InitEncoderOptions()
{
    // tooltip text is based on Aften command-line help text (aften -longhelp)
    int nCurOpt = -1;
    CString szName = _T("");
    int nValue = -1;

    #define SetEncoderOption(name, option, tooltip, defval, ignval, group, begin) \
        nCurOpt++; \
        encOpt[nCurOpt].szName = name; \
        encOpt[nCurOpt].szOption = option; \
        encOpt[nCurOpt].szHelpText = tooltip; \
        encOpt[nCurOpt].nDefaultValue = defval; \
        encOpt[nCurOpt].nIgnoreValue = ignval; \
        encOpt[nCurOpt].szGroupName = group; \
        encOpt[nCurOpt].bBeginGroup = begin;

    #define AddEncoderOptionValue(name, value) \
        szName = name; \
        encOpt[nCurOpt].listOptNames.AddTail(szName); \
        nValue = value; \
        encOpt[nCurOpt].listOptValues.AddTail(nValue);

    // (1) Encoding options

    /*
    [-fba #]       Fast bit allocation (default: 0)
                       0 = more accurate encoding
                       1 = faster encoding
    */
    SetEncoderOption(_T("Fast bit allocation"),
        _T("-fba"),
        _T("Fast bit allocation is a less-accurate search method\n")
        _T("for CBR bit allocation. It only narrows down the SNR\n")
        _T("value to within 16 of the optimal value. The result\n")
        _T("is lower overall quality, but faster encoding. This\n")
        _T("may not give the same results each time when using\n")
        _T("parallel encoding."),
        0,
        -1,
        _T("Encoding options"),
        true);

    AddEncoderOptionValue(_T("More accurate encoding (default)"), 0);
    AddEncoderOptionValue(_T("Faster encoding"), 1);

    /*
    [-exps #]      Exponent strategy search size (default: 8)
                       1 to 32 (lower is faster, higher is better quality)
    */
    SetEncoderOption(_T("Exponent strategy search size"),
        _T("-exps"),
        _T("The encoder determines the best combination of\n")
        _T("exponent strategies for a frame by searching through\n")
        _T("a list of pre-defined exponent strategies. This option\n")
        _T("controls the size of the list to be searched. The\n")
        _T("value can range from 1 (lower quality but faster) to\n")
        _T("32 (higher quality but slower). The default value is 8."),
        7, // counted from 0 (=1) to 31 (=32)
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("1 (lower quality but faster)"), 1);

    for(int i = 2; i <= 7; i++)
    {
        CString szTmpBuffer;
        szTmpBuffer.Format(_T("%d"), i);
        AddEncoderOptionValue(szTmpBuffer, i);
    }

    AddEncoderOptionValue(_T("8 (default)"), 8);

    for(int i = 9; i <= 31; i++)
    {
        CString szTmpBuffer;
        szTmpBuffer.Format(_T("%d"), i);
        AddEncoderOptionValue(szTmpBuffer, i);
    }

    AddEncoderOptionValue(_T("32 (higher quality but slower)"), 32);

    /*
    [-pad #]       Start-of-stream padding
                       0 = no padding
                       1 = 256 samples of padding (default)
    */
    SetEncoderOption(_T("Start-of-stream padding"),
        _T("-pad"),
        _T("The AC-3 format uses an overlap/add cycle for encoding\n")
        _T("each block. By default, Aften pads the delay buffer\n")
        _T("with a block of silence to avoid inaccurate encoding\n")
        _T("of the first frame of audio. If this behavior is not\n")
        _T("wanted, it can be disabled. The pad value can be a\n")
        _T("1 (default) to use padding or 0 to not use padding."),
        1,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("No padding"), 0);
    AddEncoderOptionValue(_T("256 samples of padding (default)"), 1);

    /*
    [-w #]         Bandwidth
                       0 to 60 = fixed bandwidth (28%-99% of full bandwidth)
                      -1 = fixed adaptive bandwidth (default)
                      -2 = variable adaptive bandwidth
    */
    SetEncoderOption(_T("Bandwidth"),
        _T("-w"),
        _T("The bandwidth setting corresponds to the high-frequency\n")
        _T("cutoff. Specifically, it sets the highest frequency bin\n")
        _T("which is encoded. The AC-3 format uses a 512-point MDCT\n")
        _T("which gives 256 frequency levels from 0 to 1/2 of the\n")
        _T("samplerate. The formula to give the number of coded\n")
        _T("frequency bins from bandwidth setting is:\n")
        _T("(w * 3) + 73, which gives a range of 73 to 253\n")
        _T("There are 2 special values, -1 and -2.\n")
        _T("When -1 is used, Aften automatically selects what\n")
        _T("it thinks is an appropriate bandwidth. This is the\n")
        _T("default setting.\n")
        _T("When -2 is used, a bandwidth is chosen for each frame\n")
        _T("based on CBR frame size and a target quality of 240.\n")
        _T("Variable bandwidth cannot be used with VBR mode."),
        0,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("Fixed adaptive bandwidth (default)"), -1);
    AddEncoderOptionValue(_T("Variable adaptive bandwidth"), -2);
    AddEncoderOptionValue(_T("0 (28% of full bandwidth)"), 0);

    for(int i = 1; i <= 59; i++)
    {
        CString szTmpBuffer;
        szTmpBuffer.Format(_T("%d"), i);
        AddEncoderOptionValue(szTmpBuffer, i);
    }

    AddEncoderOptionValue(_T("60 (99% of full bandwidth)"), 60);

    /*
    [-wmin #]      Minimum bandwidth [0 - 60] (default: 0)
    */
    SetEncoderOption(_T("Minimum bandwidth"),
        _T("-wmin"),
        _T("For variable bandwidth mode (-2), this option sets the\n")
        _T("minimum value for the bandwidth code. This allows the\n")
        _T("user to avoid a harsh cutoff frequency by sacrificing\n")
        _T("general audio quality. The default value is 0."),
        0,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("0 (default)"), 0);

    for(int i = 1; i <= 59; i++)
    {
        CString szTmpBuffer;
        szTmpBuffer.Format(_T("%d"), i);
        AddEncoderOptionValue(szTmpBuffer, i);
    }

    AddEncoderOptionValue(_T("60"), 60);

    /*
    [-wmax #]      Maximum bandwidth [0 - 60] (default: 60)
    */
    SetEncoderOption(_T("Maximum bandwidth"),
        _T("-wmax"),
        _T("For variable bandwidth mode (-2), this option sets the\n")
        _T("maximum value for the bandwidth code. This can be used\n")
        _T("to speed up encoding by using a lower value than 60,\n")
        _T("which is the default."),
        60,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("0"), 0);

    for(int i = 1; i <= 59; i++)
    {
        CString szTmpBuffer;
        szTmpBuffer.Format(_T("%d"), i);
        AddEncoderOptionValue(szTmpBuffer, i);
    }

    AddEncoderOptionValue(_T("60 (default)"), 60);

    /*
    [-m #]         Stereo rematrixing
                       0 = independent L+R channels
                       1 = mid/side rematrixing (default)
    */
    SetEncoderOption(_T("Stereo rematrixing"),
        _T("-m"),
        _T("Using stereo rematrixing can increase quality by\n")
        _T("removing redundant information between the left and\n")
        _T("right channels. This technique is common in audio\n")
        _T("encoding, and is sometimes called mid/side encoding.\n")
        _T("When this setting is turned on, Aften adaptively turns\n")
        _T("rematrixing on or off for each of 4 frequency bands for\n")
        _T("each block. When this setting is turned off,\n")
        _T("rematrixing is not used for any blocks. The default\n")
        _T("value is 1."),
        1,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("Independent L+R channels"), 0);
    AddEncoderOptionValue(_T("Mid/side rematrixing (default)"), 1);

    /*
    [-s #]         Block switching
                       0 = use only 512-point MDCT (default)
                       1 = selectively use 256-point MDCT
    */
    SetEncoderOption(_T("Block switching"),
        _T("-s"),
        _T("The AC-3 format allows for 2 different types of MDCT\n")
        _T("transformations to translate from time-domain to\n")
        _T("frequency-domain. The default is a 512-point transform,\n")
        _T("which gives better frequency resolution. There is also\n")
        _T("a 256-point transform, which gives better time\n")
        _T("resolution. The specification gives a suggested method\n")
        _T("for determining when to use the 256-point transform.\n")
        _T("When block switching is turned on, Aften uses the spec\n")
        _T("method for selecting the 256-point MDCT. When it is\n")
        _T("turned off, only the 512-point MDCT is used, which is\n")
        _T("faster. Block switching is turned off by default."),
        0,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("Use only 512-point MDCT (default)"), 0);
    AddEncoderOptionValue(_T("Selectively use 256-point MDCT"), 1);

    // (2) Bitstream info metadata

    /*
    [-cmix #]      Center mix level
                       0 = -3.0 dB (default)
                       1 = -4.5 dB
                       2 = -6.0 dB
    */
    SetEncoderOption(_T("Center mix level"),
        _T("-cmix"),
        _T("When three front channels are in use, this code\n")
        _T("indicates the nominal down mix level of the center\n")
        _T("channel with respect to the left and right channels.\n")
        _T("0 = -3.0 dB (default)\n")
        _T("1 = -4.5 dB\n")
        _T("2 = -6.0 dB"),
        0,
        -1,
        _T("Bitstream info metadata"),
        true);

    AddEncoderOptionValue(_T("-3.0 dB (default)"), 0);
    AddEncoderOptionValue(_T("-4.5 dB"), 1);
    AddEncoderOptionValue(_T("-6.0 dB"), 2);

    /*
    [-smix #]      Surround mix level
                       0 = -3 dB (default)
                       1 = -6 dB
                       2 = 0
    */
    SetEncoderOption(_T("Surround mix level"),
        _T("-smix"),
        _T("If surround channels are in use, this code indicates\n")
        _T("the nominal down mix level of the surround channels.\n")
        _T("0 = -3 dB (default)\n")
        _T("1 = -6 dB\n")
        _T("2 = 0"),
        0,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("-3 dB (default)"), 0);
    AddEncoderOptionValue(_T("-6 dB"), 1);
    AddEncoderOptionValue(_T("0"), 2);

    /*
    [-dsur #]      Dolby Surround mode
                       0 = not indicated (default)
                       1 = not Dolby surround encoded
                       2 = Dolby surround encoded
    */
    SetEncoderOption(_T("Dolby Surround mode"),
        _T("-dsur"),
        _T("When operating in the two channel mode, this code\n")
        _T("indicates whether or not the program has been encoded in\n")
        _T("Dolby Surround. This information is not used by the\n")
        _T("AC-3 decoder, but may be used by other portions of the\n")
        _T("audio reproduction equipment.\n")
        _T("0 = not indicated (default)\n")
        _T("1 = not Dolby surround encoded\n")
        _T("2 = Dolby surround encoded"),
        0,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("Not indicated (default)"), 0);
    AddEncoderOptionValue(_T("Not Dolby surround encoded"), 1);
    AddEncoderOptionValue(_T("Dolby surround encoded"), 2);

    // (3) Dynamic range compression and dialog normalization

    /*
    [-dnorm #]     Dialog normalization [0 - 31] (default: 31)
    */
    SetEncoderOption(_T("Dialog normalization"),
        _T("-dnorm"),
        _T("The dialog normalization value sets the average dialog\n")
        _T("level. The value is typically constant for a particular\n")
        _T("audio program. The decoder has a target output dialog\n")
        _T("level of -31dB, so if the dialog level is specified as\n")
        _T("being -31dB already (default), the output volume is not\n")
        _T("altered. Otherwise, the overall output volume is\n")
        _T("decreased so that the dialog level is adjusted down to\n")
        _T("-31dB."),
        31,
        -1,
        _T("Dynamic range compression and dialog normalization"),
        true);

    for(int i = 0; i <= 30; i++)
    {
        CString szTmpBuffer;
        szTmpBuffer.Format(_T("%d"), i);
        AddEncoderOptionValue(szTmpBuffer, i);
    }

    AddEncoderOptionValue(_T("31 (default)"), 31);

    /*
    [-dynrng #]    Dynamic Range Compression profile
                       0 = Film Light
                       1 = Film Standard
                       2 = Music Light
                       3 = Music Standard
                       4 = Speech
                       5 = None (default)
    */
    SetEncoderOption(_T("Dynamic Range Compression profile"),
        _T("-dynrng"),
        _T("Dynamic Range Compression allows for the final output\n")
        _T("dynamic range to be limited without sacrificing quality.\n")
        _T("The full dynamic range audio is still encoded, but a\n")
        _T("code is given for each block which tells the decoder to\n")
        _T("adjust the output volume for that block. The encoder\n")
        _T("must analyze the input audio to determine the best way\n")
        _T("to compress the dynamic range based on the loudness and\n")
        _T("type of input (film, music, speech).\n")
        _T("0 = Film Light\n")
        _T("1 = Film Standard\n")
        _T("2 = Music Light\n")
        _T("3 = Music Standard\n")
        _T("4 = Speech\n")
        _T("5 = None (default)"),
        5,
        -1,
        _T(""),
        false);

    AddEncoderOptionValue(_T("Film Light"), 0);
    AddEncoderOptionValue(_T("Film Standard"), 1);
    AddEncoderOptionValue(_T("Music Light"), 2);
    AddEncoderOptionValue(_T("Music Standard"), 3);
    AddEncoderOptionValue(_T("Speech"), 4);
    AddEncoderOptionValue(_T("None (default)"), 5);

    // (4) Input options

    /*
    [-acmod #]     Audio coding mode (overrides wav header)
                       0 = 1+1 (Ch1,Ch2)
                       1 = 1/0 (C)
                       2 = 2/0 (L,R)
                       3 = 3/0 (L,R,C)
                       4 = 2/1 (L,R,S)
                       5 = 3/1 (L,R,C,S)
                       6 = 2/2 (L,R,SL,SR)
                       7 = 3/2 (L,R,C,SL,SR)
    */
    SetEncoderOption(_T("Audio coding mode (overrides wav header)"),
        _T("-acmod"),
        _T("The acmod and lfe options allow the user to explicitly select the\n")
        _T("desired channel layout. This only controls the interpretation\n")
        _T("of the input, so no downmixing or upmixing is done.\n") 
        _T("0 = 1+1 (Ch1,Ch2)\n")
        _T("1 = 1/0 (C)\n")
        _T("2 = 2/0 (L,R)\n")
        _T("3 = 3/0 (L,R,C)\n")
        _T("4 = 2/1 (L,R,S)\n")
        _T("5 = 3/1 (L,R,C,S)\n")
        _T("6 = 2/2 (L,R,SL,SR)\n")
        _T("7 = 3/2 (L,R,C,SL,SR)"),
        8,
        8,
        _T("Input options"),
        true);

    AddEncoderOptionValue(_T("1+1 (Ch1,Ch2)"), 0);
    AddEncoderOptionValue(_T("1/0 (C)"), 1);
    AddEncoderOptionValue(_T("2/0 (L,R)"), 2);
    AddEncoderOptionValue(_T("3/0 (L,R,C)"), 3);
    AddEncoderOptionValue(_T("2/1 (L,R,S)"), 4);
    AddEncoderOptionValue(_T("3/1 (L,R,C,S)"), 5);
    AddEncoderOptionValue(_T("2/2 (L,R,SL,SR)"), 6);
    AddEncoderOptionValue(_T("3/2 (L,R,C,SL,SR)"), 7);
    AddEncoderOptionValue(DEFAULT_TEXT_IGNORED, 0);

    /*
    [-lfe #]       Specify use of LFE channel (overrides wav header)
                       0 = LFE channel is not present
                       1 = LFE channel is present
    */
    SetEncoderOption(_T("Specify use of LFE channel (overrides wav header)"),
        _T("-lfe"),
        _T("The acmod and lfe options allow the user to explicitly select the\n")
        _T("desired channel layout. This only controls the interpretation\n")
        _T("of the input, so no downmixing or upmixing is done.\n") 
        _T("0 = LFE channel is not present\n")
        _T("1 = LFE channel is present"),
        2,
        2,
        _T(""),
        false);

    AddEncoderOptionValue(_T("LFE channel is not present"), 0);
    AddEncoderOptionValue(_T("LFE channel is present"), 1);
    AddEncoderOptionValue(DEFAULT_TEXT_IGNORED, 0);

    /*
    [-chconfig X]  Specify channel configuration (overrides wav header)
                       1+1 = (Ch1,Ch2)
                       1/0 = (C)
                       2/0 = (L,R)
                       3/0 = (L,R,C)
                       2/1 = (L,R,S)
                       3/1 = (L,R,C,S)
                       2/2 = (L,R,SL,SR)
                       3/2 = (L,R,C,SL,SR)
                       adding "+LFE" indicates use of the LFE channel
    */
    SetEncoderOption(_T("Specify channel configuration (overrides wav header)"),
        _T("-chconfig"),
        _T("The chconfig option allow the user to explicitly select the\n")
        _T("desired channel layout. This only controls the interpretation\n")
        _T("of the input, so no downmixing or upmixing is done.\n") 
        _T("1+1 = (Ch1,Ch2)\n")
        _T("1/0 = (C)\n")
        _T("2/0 = (L,R)\n")
        _T("3/0 = (L,R,C)\n")
        _T("2/1 = (L,R,S)\n")
        _T("3/1 = (L,R,C,S)\n")
        _T("2/2 = (L,R,SL,SR)\n")
        _T("3/2 = (L,R,C,SL,SR)\n")
        _T("Adding +LFE indicates use of the LFE channel"),
        16,
        16,
        _T(""),
        false);

    AddEncoderOptionValue(_T("1+1 = (Ch1,Ch2)"), 0);
    AddEncoderOptionValue(_T("1/0 = (C)"), 1);
    AddEncoderOptionValue(_T("2/0 = (L,R)"), 2);
    AddEncoderOptionValue(_T("3/0 = (L,R,C)"), 3);
    AddEncoderOptionValue(_T("2/1 = (L,R,S)"), 4);
    AddEncoderOptionValue(_T("3/1 = (L,R,C,S)"), 5);
    AddEncoderOptionValue(_T("2/2 = (L,R,SL,SR)"), 6);
    AddEncoderOptionValue(_T("3/2 = (L,R,C,SL,SR)"), 7);
    AddEncoderOptionValue(_T("1+1+LFE = (Ch1,Ch2) + LFE"), 8);
    AddEncoderOptionValue(_T("1/0+LFE = (C) + LFE"), 9);
    AddEncoderOptionValue(_T("2/0+LFE = (L,R) + LFE"), 10);
    AddEncoderOptionValue(_T("3/0+LFE = (L,R,C) + LFE"), 11);
    AddEncoderOptionValue(_T("2/1+LFE = (L,R,S) + LFE"), 12);
    AddEncoderOptionValue(_T("3/1+LFE = (L,R,C,S) + LFE"), 13);
    AddEncoderOptionValue(_T("2/2+LFE = (L,R,SL,SR) + LFE"), 14);
    AddEncoderOptionValue(_T("3/2+LFE = (L,R,C,SL,SR) + LFE"), 15);
    AddEncoderOptionValue(DEFAULT_TEXT_IGNORED, 0);

    /*
    [-chmap #]     Channel mapping order of input audio
                       0 = WAVE mapping (default)
                       1 = AC-3 mapping
                       2 = MPEG mapping
    */
	SetEncoderOption(_T("Channel mapping order of input audio"),
		_T("-chmap"),
		_T("Some programs create WAVE files which use a channel\n")
		_T("mapping other than the standard WAVE mapping. This\n")
		_T("option allows the user to specify if the input file\n")
		_T("uses WAVE, AC-3, or MPEG channel mapping. The MPEG\n")
		_T("channel mapping is used by DTS and by MPEG-2/4 formats\n")
		_T("such as MP2 and AAC.\n")
		_T("0 = WAVE mapping (default)\n")
		_T("1 = AC-3 mapping\n")
		_T("2 = MPEG mapping"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("WAV mapping (default)"), 0);
	AddEncoderOptionValue(_T("AC-3 mapping"), 1);
	AddEncoderOptionValue(_T("MPEG mapping"), 2);

    /*
    [-readtoeof #] Read input WAVE audio data until the end-of-file
                       0 = use data size in header (default)
                       1 = read data until end-of-file
    */
	SetEncoderOption(_T("Read input WAVE audio data until the end-of-file"),
		_T("-readtoeof"),
		_T("This overrides the data size in the WAVE header, and\n")
		_T("can be useful for streaming input or files larger than\n")
		_T("4 GB.\n")
		_T("0 = use data size in header (default)\n")
		_T("1 = read data until end-of-file"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Use data size in header (default)"), 0);
	AddEncoderOptionValue(_T("Read data until end-of-file"), 1);

    // (5) Input filters

    /*
    [-bwfilter #]  Specify use of the bandwidth low-pass filter
                       0 = do not apply filter (default)
                       1 = apply filter
    */
	SetEncoderOption(_T("Specify use of the bandwidth low-pass filter"),
		_T("-bwfilter"),
		_T("The bandwidth low-pass filter pre-filters the input\n")
		_T("audio before converting to frequency-domain. This\n")
		_T("smooths the cutoff frequency transition for slightly\n")
		_T("better quality.\n")
		_T("0 = do not apply filter (default)\n")
		_T("1 = apply filter"),
		0,
		-1,
        _T("Input filters"),
        true);

	AddEncoderOptionValue(_T("Do not apply filter (default)"), 0);
	AddEncoderOptionValue(_T("Apply filter"), 1);

    /*
    [-dcfilter #]  Specify use of the DC high-pass filter
                       0 = do not apply filter (default)
                       1 = apply filter
    */
	SetEncoderOption(_T("Specify use of the DC high-pass filter"),
		_T("-dcfilter"),
		_T("The DC high-pass filter is listed as optional by the\n")
		_T("AC-3 specification. The implementation, as suggested,\n")
		_T("is a single pole filter at 3 Hz.\n")
		_T("0 = do not apply filter (default)\n")
		_T("1 = apply filter"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Do not apply filter (default)"), 0);
	AddEncoderOptionValue(_T("Apply filter"), 1);

    /*
    [-lfefilter #] Specify use of the LFE low-pass filter
                       0 = do not apply filter (default)
                       1 = apply filter
    */
	SetEncoderOption(_T("Specify use of the LFE low-pass filter"),
		_T("-lfefilter"),
		_T("The LFE low-pass filter is recommended by the AC-3\n")
		_T("specification. The cutoff is 120 Hz. The specification\n")
		_T("recommends an 8th order elliptic filter, but instead,\n")
		_T("Aften uses a Butterworth 2nd order cascaded direct\n")
		_T("form II filter.\n")
		_T("0 = do not apply filter (default)\n")
		_T("1 = apply filter"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Do not apply filter (default)"), 0);
	AddEncoderOptionValue(_T("Apply filter"), 1);

    // (6) Alternate bit stream syntax

    /*
    [-xbsi1 #]     Specify use of extended bitstream info 1
                       0 = do not write xbsi1
                       1 = write xbsi1
    */
	SetEncoderOption(_T("Specify use of extended bitstream info 1"),
		_T("-xbsi1"),
		_T("Extended bitstream info 1 contains the dmixmod,\n")
		_T("ltrtcmix, ltrtsmix, lorocmix, and lorosmix fields. If\n")
		_T("this option is turned on, all these values are written\n")
		_T("to the output stream.\n")
		_T("0 = do not write xbsi1\n")
		_T("1 = write xbsi1"),
		2,
		2,
        _T("Alternate bit stream syntax"),
        true);

	AddEncoderOptionValue(_T("Do not write xbsi1"), 0);
	AddEncoderOptionValue(_T("Write xbsi1"), 1);
	AddEncoderOptionValue(DEFAULT_TEXT_IGNORED, 0);

    /*
    [-dmixmod #]   Preferred stereo downmix mode
                       0 = not indicated (default)
                       1 = Lt/Rt downmix preferred
                       2 = Lo/Ro downmix preferred
    */
	SetEncoderOption(_T("Preferred stereo downmix mode"),
		_T("-dmixmod"),
		_T("This code indicates the type of stereo downmix preferred\n")
		_T("by the mastering engineer, and can be optionally used,\n")
		_T("overridden, or ignored by the decoder.\n")
		_T("0 = not indicated (default)\n")
		_T("1 = Lt/Rt downmix preferred\n")
		_T("2 = Lo/Ro downmix preferred"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Not indicated (default)"), 0);
	AddEncoderOptionValue(_T("Lt/Rt downmix preferred"), 1);
	AddEncoderOptionValue(_T("Lo/Ro downmix preferred"), 2);

    /*
    [-ltrtcmix #]  Lt/Rt center mix level
                       0 = +3.0 dB
                       1 = +1.5 dB
                       2 =  0.0 dB
                       3 = -1.5 dB
                       4 = -3.0 dB (default)
                       5 = -4.5 dB
                       6 = -6.0 dB
                       7 = -inf dB
    */
	SetEncoderOption(_T("Lt/Rt center mix level"),
		_T("-ltrtcmix"),
		_T("This code indicates the nominal down mix level of the\n")
		_T("center channel with respect to the left and right\n")
		_T("channels in an Lt/Rt downmix.\n")
		_T("0 = +3.0 dB\n")
		_T("1 = +1.5 dB\n")
		_T("2 =  0.0 dB\n")
		_T("3 = -1.5 dB\n")
		_T("4 = -3.0 dB (default)\n")
		_T("5 = -4.5 dB\n")
		_T("6 = -6.0 dB\n")
		_T("7 = -inf dB"),
		4,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("+3.0 dB"), 0);
	AddEncoderOptionValue(_T("+1.5 dB"), 1);
	AddEncoderOptionValue(_T("0.0 dB"), 2);
	AddEncoderOptionValue(_T("-1.5 dB"), 3);
	AddEncoderOptionValue(_T("-3.0 dB (default)"), 4);
	AddEncoderOptionValue(_T("-4.5 dB"), 5);
	AddEncoderOptionValue(_T("-6.0 dB"), 6);
	AddEncoderOptionValue(_T("-inf dB"), 7);

    /*
    [-ltrtsmix #]  Lt/Rt surround mix level
                       0 = +3.0 dB
                       1 = +1.5 dB
                       2 =  0.0 dB
                       3 = -1.5 dB
                       4 = -3.0 dB (default)
                       5 = -4.5 dB
                       6 = -6.0 dB
                       7 = -inf dB
    */
	SetEncoderOption(_T("Lt/Rt surround mix level"),
		_T("-ltrtsmix"),
		_T("This code indicates the nominal down mix level of the\n")
		_T("surround channels with respect to the left and right\n")
		_T("channels in an Lt/Rt downmix.\n")
		_T("0 = +3.0 dB\n")
		_T("1 = +1.5 dB\n")
		_T("2 =  0.0 dB\n")
		_T("3 = -1.5 dB\n")
		_T("4 = -3.0 dB (default)\n")
		_T("5 = -4.5 dB\n")
		_T("6 = -6.0 dB\n")
		_T("7 = -inf dB"),
		4,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("+3.0 dB"), 0);
	AddEncoderOptionValue(_T("+1.5 dB"), 1);
	AddEncoderOptionValue(_T("0.0 dB"), 2);
	AddEncoderOptionValue(_T("-1.5 dB"), 3);
	AddEncoderOptionValue(_T("-3.0 dB (default)"), 4);
	AddEncoderOptionValue(_T("-4.5 dB"), 5);
	AddEncoderOptionValue(_T("-6.0 dB"), 6);
	AddEncoderOptionValue(_T("-inf dB"), 7);

    /*
    [-lorocmix #]  Lo/Ro center mix level
                       0 = +3.0 dB
                       1 = +1.5 dB
                       2 =  0.0 dB
                       3 = -1.5 dB
                       4 = -3.0 dB (default)
                       5 = -4.5 dB
                       6 = -6.0 dB
                       7 = -inf dB
    */
	SetEncoderOption(_T("Lo/Ro center mix level"),
		_T("-lorocmix"),
		_T("This code indicates the nominal down mix level of the\n")
		_T("center channel with respect to the left and right\n")
		_T("channels in an Lo/Ro downmix.\n")
		_T("0 = +3.0 dB\n")
		_T("1 = +1.5 dB\n")
		_T("2 =  0.0 dB\n")
		_T("3 = -1.5 dB\n")
		_T("4 = -3.0 dB (default)\n")
		_T("5 = -4.5 dB\n")
		_T("6 = -6.0 dB\n")
		_T("7 = -inf dB"),
		4,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("+3.0 dB"), 0);
	AddEncoderOptionValue(_T("+1.5 dB"), 1);
	AddEncoderOptionValue(_T("0.0 dB"), 2);
	AddEncoderOptionValue(_T("-1.5 dB"), 3);
	AddEncoderOptionValue(_T("-3.0 dB (default)"), 4);
	AddEncoderOptionValue(_T("-4.5 dB"), 5);
	AddEncoderOptionValue(_T("-6.0 dB"), 6);
	AddEncoderOptionValue(_T("-inf dB"), 7);

    /*
    [-lorosmix #]  Lo/Ro surround mix level
                       0 = +3.0 dB
                       1 = +1.5 dB
                       2 =  0.0 dB
                       3 = -1.5 dB
                       4 = -3.0 dB (default)
                       5 = -4.5 dB
                       6 = -6.0 dB
                       7 = -inf dB
    */
	SetEncoderOption(_T("Lo/Ro surround mix level"),
		_T("-lorosmix"),
		_T("This code indicates the nominal down mix level of the\n")
		_T("surround channels with respect to the left and right\n")
		_T("channels in an Lo/Ro downmix.\n")
		_T("0 = +3.0 dB\n")
		_T("1 = +1.5 dB\n")
		_T("2 =  0.0 dB\n")
		_T("3 = -1.5 dB\n")
		_T("4 = -3.0 dB (default)\n")
		_T("5 = -4.5 dB\n")
		_T("6 = -6.0 dB\n")
		_T("7 = -inf dB"),
		4,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("+3.0 dB"), 0);
	AddEncoderOptionValue(_T("+1.5 dB"), 1);
	AddEncoderOptionValue(_T("0.0 dB"), 2);
	AddEncoderOptionValue(_T("-1.5 dB"), 3);
	AddEncoderOptionValue(_T("-3.0 dB (default)"), 4);
	AddEncoderOptionValue(_T("-4.5 dB"), 5);
	AddEncoderOptionValue(_T("-6.0 dB"), 6);
	AddEncoderOptionValue(_T("-inf dB"), 7);

    /*
    [-xbsi2 #]     Specify use of extended bitstream info 2
                       0 = do not write xbsi2
                       1 = write xbsi2
    */
	SetEncoderOption(_T("Specify use of extended bitstream info 2"),
		_T("-xbsi2"),
		_T("Extended bitstream info 2 contains the dsurexmod,\n")
		_T("dheadphon, and adconvtyp fields. If this option is\n")
		_T("turned on, all these values are written to the output\n")
		_T("stream. These options are not used by the AC-3 decoder,\n")
		_T("but may be used by other portions of the audio\n")
		_T("reproduction equipment.\n")
		_T("0 = do not write xbsi2\n")
		_T("1 = write xbsi2"),
		2,
		2,
        _T(""),
        false);

    AddEncoderOptionValue(_T("Do not write xbsi2"), 0);
    AddEncoderOptionValue(_T("Write xbsi2"), 1);
    AddEncoderOptionValue(DEFAULT_TEXT_IGNORED, 0);

    /*
    [-dsurexmod #] Dolby Surround EX mode
                       0 = not indicated (default)
                       1 = Not Dolby Surround EX encoded
                       2 = Dolby Surround EX encoded
    */
	SetEncoderOption(_T("Dolby Surround EX mode"),
		_T("-dsurexmod"),
		_T("This code indicates whether or not the program has been\n")
		_T("encoded in Dolby Surround EX.\n")
		_T("0 = not indicated (default)\n")
		_T("1 = Not Dolby Surround EX encoded\n")
		_T("2 = Dolby Surround EX encoded"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Not indicated (default)"), 0);
	AddEncoderOptionValue(_T("Not Dolby Surround EX encoded"), 1);
	AddEncoderOptionValue(_T("Dolby Surround EX encoded"), 2);

    /*
    [-dheadphon #] Dolby Headphone mode
                       0 = not indicated (default)
                       1 = Not Dolby Headphone encoded
                       2 = Dolby Headphone encoded
    */
	SetEncoderOption(_T("Dolby Headphone mode"),
		_T("-dheadphon"),
		_T("This code indicates whether or not the program has been\n")
		_T("Dolby Headphone-encoded.\n")
		_T("0 = not indicated (default)\n")
		_T("1 = Not Dolby Headphone encoded\n")
		_T("2 = Dolby Headphone encoded"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Not indicated (default)"), 0);
	AddEncoderOptionValue(_T("Not Dolby Headphone encoded"), 1);
	AddEncoderOptionValue(_T("Dolby Headphone encoded"), 2);

    /*
    [-adconvtyp #] A/D converter type
                       0 = Standard (default)
                       1 = HDCD
    */
	SetEncoderOption(_T("A/D converter type"),
		_T("-adconvtyp"),
		_T("This code indicates the type of A/D converter technology\n")
		_T("used to capture the PCM audio.\n")
		_T("0 = Standard (default)\n")
		_T("1 = HDCD"),
		0,
		-1,
        _T(""),
        false);

	AddEncoderOptionValue(_T("Standard (default)"), 0);
	AddEncoderOptionValue(_T("HDCD"), 1);
}

void ParseEncoderPreset(EncoderPreset &preset, ConfigList_t &clTmp)
{
    POSITION pos = clTmp.GetHeadPosition();
    while(pos)
    {
        CString szBuffer;
        ConfigEntry ce = clTmp.GetNext(pos);

        // encoder engine number
        if(ce.szKey.Compare(_T("engine")) == 0)
        {
            preset.nCurrentEngine = _tstoi(ce.szData);
            continue;
        }

        // number of threads
        szBuffer = szThreadsOption;
        if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            preset.nThreads = _tstoi(ce.szData);
            continue;
        }

        // SIMD instructions: MMX
        if(ce.szKey.Compare(_T("mmx")) == 0)
        {
            preset.nUsedSIMD[0] = _tstoi(ce.szData);
            continue;
        }

        // SIMD instructions: SSE
        if(ce.szKey.Compare(_T("sse")) == 0)
        {
            preset.nUsedSIMD[1] = _tstoi(ce.szData);
            continue;
        }

        // SIMD instructions: SSE2
        if(ce.szKey.Compare(_T("sse2")) == 0)
        {
            preset.nUsedSIMD[2] = _tstoi(ce.szData);
            continue;
        }

        // SIMD instructions: SSE3
        if(ce.szKey.Compare(_T("sse3")) == 0)
        {
            preset.nUsedSIMD[3] = _tstoi(ce.szData);
            continue;
        }

        // mode
        if(ce.szKey.Compare(_T("mode")) == 0)
        {
            preset.nMode = (AftenEncMode) _tstoi(ce.szData);
            continue;
        }

        // bitrate
        szBuffer = szCbrOption;
        if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            preset.nBitrate = _tstoi(ce.szData);
            continue;
        }

        // quality
        szBuffer = szVbrOption;
        if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            preset.nQuality = _tstoi(ce.szData);
            continue;
        }

        // raw input audio format
        szBuffer = szRawSampleFormatOption;
        if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            preset.nRawSampleFormat = _tstoi(ce.szData);
            continue;
        }

        // raw input audio sample rate
        szBuffer = szRawSampleRateOption;
        if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            preset.nRawSampleRate = _tstoi(ce.szData);
            continue;
        }

        // raw input audio channels
        szBuffer = szRawChannelsOption;
        if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
        {
            preset.nRawChannels = _tstoi(ce.szData);
            continue;
        }

        // check all other Aften options
        for(int i = 0; i < nNumEncoderOptions; i++)
        {
            szBuffer = encOpt[i].szOption;
            if(ce.szKey.Compare(szBuffer.TrimLeft(_T("-"))) == 0)
            {
                preset.nSetting[i] = _tstoi(ce.szData);
                break;
            }
        }
    }
}

// load encoder presets
bool LoadEncoderPresets(CString szFileName)
{
    try
    {
        CMyFile fp;
        if(fp.FOpen(szFileName, false) == false)
            return false;

        EncoderPreset preset;
        ConfigList_t clTmp;
        ConfigEntry ceTmp;

        bool bHaveVersion = false;
        bool bHavePreset = false;

        TCHAR Buffer;
        const ULONGLONG nLength = fp.FSize();
        CString szBuffer = _T("");

        while(fp.FRead(Buffer) == true)
        {
            if((Buffer != '\r') && (Buffer != '\n'))
                szBuffer += Buffer;

            if(Buffer == '\n')
            {
                szBuffer += _T("\0");

                if(bHaveVersion == false)
                {
                    // check presets version
                    if(szBuffer.Compare(CURRENT_PRESETS_VERSION) != 0)
                    {
                        // invalid presets version
                        fp.FClose();
                        return false;
                    }

                    bHaveVersion = true;
                }
                else
                {
                    if((szBuffer.Left(1) == _T("[")) && (szBuffer.Right(1) == _T("]")))
                    {
                        // check if we have preset to save
                        if(bHavePreset == true)
                        {
                            // parse all options in clTmp list
                            ParseEncoderPreset(preset, clTmp);

                            // add to presets list
                            encPresets.AddTail(preset);

                            // clear temp list
                            clTmp.RemoveAll();
                        }
                        else
                        {
                            // remove old presets because we have valid one preset
                            encPresets.RemoveAll();
                        }

                        // set defaults for new preset
                        preset = defaultPreset;

                        // get next preset name
                        preset.szName = szBuffer.Mid(1, szBuffer.GetLength() - 2);

                        bHavePreset = true;
                    }
                    else
                    {
                        int nPos = szBuffer.Find('=', 0);
                        if(nPos != -1)
                        {
                            ceTmp.szKey = szBuffer.Mid(0, nPos);

                            // remove 'spaces' and 'tabs'
                            while(ceTmp.szKey.Left(1) == _T(" ") || ceTmp.szKey.Left(1) == _T("\t"))
                            {
                                ceTmp.szKey.TrimLeft(_T(" "));
                                ceTmp.szKey.TrimLeft(_T("\t"));
                            }

                            while(ceTmp.szKey.Right(1) == _T(" ") || ceTmp.szKey.Right(1) == _T("\t"))
                            {
                                ceTmp.szKey.TrimRight(_T(" "));
                                ceTmp.szKey.TrimRight(_T("\t"));
                            }

                            ceTmp.szData = szBuffer.Mid(nPos + 1, szBuffer.GetLength() - 1);

                            // remove 'spaces' and 'tabs'
                            while(ceTmp.szData.Left(1) == _T(" ") || ceTmp.szData.Left(1) == _T("\t"))
                            {
                                ceTmp.szData.TrimLeft(_T(" "));
                                ceTmp.szData.TrimLeft(_T("\t"));
                            }

                            while(ceTmp.szData.Right(1) == _T(" ") || ceTmp.szData.Right(1) == _T("\t"))
                            {
                                ceTmp.szData.TrimRight(_T(" "));
                                ceTmp.szData.TrimRight(_T("\t"));
                            }

                            // add to the list (data will be parsed later)
                            clTmp.AddTail(ceTmp);
                        }
                        else
                        {
                            // this is not a valid option skip the line
                        }
                    }
                }

                szBuffer = _T("");
            }

            // save last preset if any
            if(fp.FPos() == nLength)
            {
                // check if we have preset to save
                if(bHavePreset == true)
                {
                    // parse all options in clTmp list
                    ParseEncoderPreset(preset, clTmp);

                    // add to presets list
                    encPresets.AddTail(preset);

                    // clear temp list
                    clTmp.RemoveAll();
                }
            }
        }

        fp.FClose();

        return true;
    }
    catch(...)
    {
        return false;
    }

    return true;
}

// save encoder presets
bool SaveEncoderPresets(CString szFileName)
{
    const int nSize = (const int) encPresets.GetSize();
    try
    {
        CMyFile fp;
        if(fp.FOpen(szFileName, true) == false)
            return false;

        CString szBuffer;
        CString szTmpBuffer;
        EncoderPreset preset;

        #define WriteBufferToFile() \
            fp.FWriteString(szBuffer.GetBuffer(), szBuffer.GetLength()); \
            szBuffer.ReleaseBuffer();

        // write current presets version
        szBuffer.Format(_T("%s\r\n"), CURRENT_PRESETS_VERSION);
        WriteBufferToFile();

        // write all presets
        for(int i = 0; i < nSize; i++)
        {
            preset = encPresets.GetAt(encPresets.FindIndex(i));

            // save name
            szBuffer.Format(_T("[%s]\r\n"), preset.szName);
            WriteBufferToFile();

            // save only values different than default and ignore

            // save encoder engine number
            if(preset.nCurrentEngine != defaultPreset.nCurrentEngine)
            {
                szBuffer.Format(_T("engine=%d\r\n"), preset.nCurrentEngine);
                WriteBufferToFile();
            }

            // save number of threads
            if(preset.nThreads != defaultPreset.nThreads)
            {
                szTmpBuffer = szThreadsOption;
                szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nThreads);
                WriteBufferToFile();
            }

            // save SIMD instructions: MMX
            if(preset.nUsedSIMD[0] != defaultPreset.nUsedSIMD[0])
            {
                szBuffer.Format(_T("mmx=%d\r\n"), preset.nUsedSIMD[0]);
                WriteBufferToFile();
            }

            // save SIMD instructions: SSE
            if(preset.nUsedSIMD[1] != defaultPreset.nUsedSIMD[1])
            {
                szBuffer.Format(_T("sse=%d\r\n"), preset.nUsedSIMD[1]);
                WriteBufferToFile();
            }

            // save SIMD instructions: SSE2
            if(preset.nUsedSIMD[2] != defaultPreset.nUsedSIMD[2])
            {
                szBuffer.Format(_T("sse2=%d\r\n"), preset.nUsedSIMD[2]);
                WriteBufferToFile();
            }

            // save SIMD instructions: SSE3
            if(preset.nUsedSIMD[3] != defaultPreset.nUsedSIMD[3])
            {
                szBuffer.Format(_T("sse3=%d\r\n"), preset.nUsedSIMD[3]);
                WriteBufferToFile();
            }

            // save mode
            if(preset.nMode != defaultPreset.nMode)
            {
                szBuffer.Format(_T("mode=%d\r\n"), preset.nMode);
                WriteBufferToFile();
            }

            // save bitrate
            if(preset.nBitrate != defaultPreset.nBitrate)
            {
                szTmpBuffer = szCbrOption;
                szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nBitrate);
                WriteBufferToFile();
            }

            // save quality
            if(preset.nQuality != defaultPreset.nQuality)
            {
                szTmpBuffer = szVbrOption;
                szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nQuality);
                WriteBufferToFile();
            }

            // save raw input audio format
            if(preset.nRawSampleFormat != defaultPreset.nRawSampleFormat)
            {
                szTmpBuffer = szRawSampleFormatOption;
                szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nRawSampleFormat);
                WriteBufferToFile();
            }

            // save raw input audio sample rate
            if(preset.nRawSampleRate != defaultPreset.nRawSampleRate)
            {
                szTmpBuffer = szRawSampleRateOption;
                szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nRawSampleRate);
                WriteBufferToFile();
            }

            // save raw input audio channels
            if(preset.nRawChannels != defaultPreset.nRawChannels)
            {
                szTmpBuffer = szRawChannelsOption;
                szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nRawChannels);
                WriteBufferToFile();
            }

            // save all other settings
            for(int j = 0; j < nNumEncoderOptions; j++)
            {
                if((encOpt[j].nIgnoreValue != preset.nSetting[j]) && (encOpt[j].nDefaultValue != preset.nSetting[j]))
                {
                    szTmpBuffer = encOpt[j].szOption;
                    szBuffer.Format(_T("%s=%d\r\n"), szTmpBuffer.TrimLeft(_T("-")), preset.nSetting[j]);
                    WriteBufferToFile();
                }
            }
        }

        fp.FClose();
    }
    catch(...)
    {

        return false;
    }

    return true;
}

bool IsSupportedInputExt(CString &szExt)
{
    for(int i = 0; i < NUM_SUPPORTED_INPUT_EXT; i++)
    {
        if(szExt.CompareNoCase(szSupportedInputExt[i]) == 0)
            return true;
    }

    return false;
}

int GetSupportedInputFormat(CString &szExt)
{
    for(int i = 0; i < NUM_SUPPORTED_INPUT_EXT; i++)
    {
        if(szExt.CompareNoCase(szSupportedInputExt[i]) == 0)
        {
            return nSupportedInputFormats[i];
        }
    }

    return PCM_FORMAT_UNKNOWN;
}

CString GetSupportedInputFilesFilter()
{
    CString szFilter = _T("");
    CString szExtL = _T("");
    CString szExtU = _T("");
    CString szBuff = _T("");

    for(int i = 0; i < NUM_SUPPORTED_INPUT_EXT; i++)
    {
        szExtL = szSupportedInputExt[i];
        szBuff = _T("*.") + szExtL.MakeLower();
        szBuff += (i < NUM_SUPPORTED_INPUT_EXT - 1) ? _T(";") : _T("");
        szFilter += szBuff;
    }

    szFilter = _T("Supported Files (") + szFilter + _T(")|") + szFilter + _T("|");

    for(int i = 0; i < NUM_SUPPORTED_INPUT_EXT; i++)
    {
        szExtL = szExtU = szSupportedInputExt[i];
        szExtU.MakeUpper();
        szExtL.MakeLower();
        szBuff.Format(_T("%s Files (*.%s)|*.%s|"), szExtU, szExtL, szExtL);
        szFilter += szBuff;
    }

    szFilter += _T("All Files (*.*)|*.*||");
    return szFilter;
}
