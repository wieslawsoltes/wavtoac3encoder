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
 * @file helptext.h
 * string constants for commandline help and longhelp
 */

#ifndef HELPTEXT_H
#define HELPTEXT_H

static const char *usage_heading = "usage: aften [options] <input.wav> <output.ac3>\n";

#define HELP_OPTIONS_COUNT 43

static const char *help_options[HELP_OPTIONS_COUNT] = {
"    [-h]           Print out list of commandline options\n",

"    [-longhelp]    Print out commandline option details\n",

"    [-version]     Print out the build version\n",

"    [-v #]         Verbosity (controls output to console)\n"
"                       0 = quiet mode\n"
"                       1 = show average stats (default)\n"
"                       2 = show each frame's stats\n",

"    [-threads #]   Number of parallel threads to use\n"
"                       0 = detect number of CPUs (default)\n",

"    [-nosimd X]    Comma-separated list of SIMD instruction sets not to use\n"
"                       Available sets are mmx, sse, sse2, sse3 and altivec.\n"
"                       No spaces are allowed between the sets and the commas.\n",

"    [-b #]         CBR bitrate in kbps (default: about 96kbps per channel)\n",

"    [-q #]         VBR quality [0 - 1023] (default: 240)\n",

"    [-fba #]       Fast bit allocation (default: 0)\n"
"                       0 = more accurate encoding\n"
"                       1 = faster encoding\n",

"    [-exps #]      Exponent strategy search size (default: 8)\n"
"                       1 to 32 (lower is faster, higher is better quality)\n",

"    [-pad #]       Start-of-stream padding\n"
"                       0 = no padding\n"
"                       1 = 256 samples of padding (default)\n",

"    [-w #]         Bandwidth\n"
"                       0 to 60 = fixed bandwidth (28\45-99\45 of full bandwidth)\n"
"                      -1 = fixed adaptive bandwidth (default)\n"
"                      -2 = variable adaptive bandwidth\n",

"    [-wmin #]      Minimum bandwidth [0 - 60] (default: 0)\n",

"    [-wmax #]      Maximum bandwidth [0 - 60] (default: 60)\n",

"    [-m #]         Stereo rematrixing\n"
"                       0 = independent L+R channels\n"
"                       1 = mid/side rematrixing (default)\n",

"    [-s #]         Block switching\n"
"                       0 = use only 512-point MDCT (default)\n"
"                       1 = selectively use 256-point MDCT\n",

"    [-cmix #]      Center mix level\n"
"                       0 = -3.0 dB (default)\n"
"                       1 = -4.5 dB\n"
"                       2 = -6.0 dB\n",

"    [-smix #]      Surround mix level\n"
"                       0 = -3 dB (default)\n"
"                       1 = -6 dB\n"
"                       2 = 0\n",

"    [-dsur #]      Dolby Surround mode\n"
"                       0 = not indicated (default)\n"
"                       1 = not Dolby surround encoded\n"
"                       2 = Dolby surround encoded\n",

"    [-dnorm #]     Dialog normalization [0 - 31] (default: 31)\n",

"    [-dynrng #]    Dynamic Range Compression profile\n"
"                       0 = Film Light\n"
"                       1 = Film Standard\n"
"                       2 = Music Light\n"
"                       3 = Music Standard\n"
"                       4 = Speech\n"
"                       5 = None (default)\n",

"    [-acmod #]     Audio coding mode (overrides wav header)\n"
"                       0 = 1+1 (Ch1,Ch2)\n"
"                       1 = 1/0 (C)\n"
"                       2 = 2/0 (L,R)\n"
"                       3 = 3/0 (L,R,C)\n"
"                       4 = 2/1 (L,R,S)\n"
"                       5 = 3/1 (L,R,C,S)\n"
"                       6 = 2/2 (L,R,SL,SR)\n"
"                       7 = 3/2 (L,R,C,SL,SR)\n",

"    [-lfe #]       Specify use of LFE channel (overrides wav header)\n"
"                       0 = LFE channel is not present\n"
"                       1 = LFE channel is present\n",

"    [-chconfig X]  Specify channel configuration (overrides wav header)\n"
"                       1+1 = (Ch1,Ch2)\n"
"                       1/0 = (C)\n"
"                       2/0 = (L,R)\n"
"                       3/0 = (L,R,C)\n"
"                       2/1 = (L,R,S)\n"
"                       3/1 = (L,R,C,S)\n"
"                       2/2 = (L,R,SL,SR)\n"
"                       3/2 = (L,R,C,SL,SR)\n"
"                       adding \"+LFE\" indicates use of the LFE channel\n",

"    [-ch_X file]   Add a mono file to the input list as the channel specified\n"
"                       ch_fl  = Front Left\n"
"                       ch_fc  = Front Center\n"
"                       ch_fr  = Front Right\n"
"                       ch_sl  = Surround Left\n"
"                       ch_s   = Surround\n"
"                       ch_sr  = Surround Right\n"
"                       ch_m1  = Dual Mono Channel 1\n"
"                       ch_m2  = Dual Mono Channel 2\n"
"                       ch_lfe = LFE\n",

"    [-raw_fmt X]   Raw audio input sample format (default: s16_le)\n"
"                       One of the pre-defined sample formats:\n"
"                       u8, s8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,\n"
"                       s32_le, s32_be, float_le, float_be, double_le, double_be\n",

"    [-raw_sr #]    Raw audio input sample rate (default: 48000)\n",

"    [-raw_ch #]    Raw audio input channels (default: 1)\n",

"    [-chmap #]     Channel mapping order of input audio\n"
"                       0 = WAVE mapping (default)\n"
"                       1 = AC-3 mapping\n"
"                       2 = MPEG mapping\n",

"    [-readtoeof #] Read input WAVE audio data until the end-of-file\n"
"                       0 = use data size in header (default)\n"
"                       1 = read data until end-of-file\n",

"    [-bwfilter #]  Specify use of the bandwidth low-pass filter\n"
"                       0 = do not apply filter (default)\n"
"                       1 = apply filter\n",

"    [-dcfilter #]  Specify use of the DC high-pass filter\n"
"                       0 = do not apply filter (default)\n"
"                       1 = apply filter\n",

"    [-lfefilter #] Specify use of the LFE low-pass filter\n"
"                       0 = do not apply filter (default)\n"
"                       1 = apply filter\n",

"    [-xbsi1 #]     Specify use of extended bitstream info 1\n"
"                       0 = do not write xbsi1\n"
"                       1 = write xbsi1\n",

"    [-dmixmod #]   Preferred stereo downmix mode\n"
"                       0 = not indicated (default)\n"
"                       1 = Lt/Rt downmix preferred\n"
"                       2 = Lo/Ro downmix preferred\n",

"    [-ltrtcmix #]  Lt/Rt center mix level\n",
"    [-ltrtsmix #]  Lt/Rt surround mix level\n",
"    [-lorocmix #]  Lo/Ro center mix level\n",
"    [-lorosmix #]  Lo/Ro surround mix level\n"
"                       0 = +3.0 dB\n"
"                       1 = +1.5 dB\n"
"                       2 =  0.0 dB\n"
"                       3 = -1.5 dB\n"
"                       4 = -3.0 dB (default)\n"
"                       5 = -4.5 dB\n"
"                       6 = -6.0 dB\n"
"                       7 = -inf dB\n",

"    [-xbsi2 #]     Specify use of extended bitstream info 2\n"
"                       0 = do not write xbsi2\n"
"                       1 = write xbsi2\n",

"    [-dsurexmod #] Dolby Surround EX mode\n"
"                       0 = not indicated (default)\n"
"                       1 = Not Dolby Surround EX encoded\n"
"                       2 = Dolby Surround EX encoded\n",

"    [-dheadphon #] Dolby Headphone mode\n"
"                       0 = not indicated (default)\n"
"                       1 = Not Dolby Headphone encoded\n"
"                       2 = Dolby Headphone encoded\n",

"    [-adconvtyp #] A/D converter type\n"
"                       0 = Standard (default)\n"
"                       1 = HDCD\n"
};


/* longhelp string constants */

#define CONSOLE_OPTIONS_COUNT 4

static const char console_heading[24] = "CONSOLE OUTPUT OPTIONS\n";
static const char *console_options[CONSOLE_OPTIONS_COUNT] = {
"    [-h]           Print out list of commandline options.\n",

"    [-longhelp]    Print out commandline option details.\n",

"    [-version]     Print out the build version.\n",

"    [-v #]         Verbosity\n"
"                       This controls the level of console output to stderr.\n"
"                       0 - Quiet Mode. No output to stderr.\n"
"                       1 - Shows a running average of encoding statistics.\n"
"                           This is the default setting.\n"
"                       2 - Shows the statistics for each frame.\n"
};

#define ENCODING_OPTIONS_COUNT 12

static const char encoding_heading[18] = "ENCODING OPTIONS\n";
static const char *encoding_options[ENCODING_OPTIONS_COUNT] = {
"    [-threads #]   Number of threads\n"
"                       Aften can use multiple threads to speed up encoding.\n"
"                       By default, Aften uses one thread for each logical CPU\n"
"                       your system has, but you can override this value.  A\n"
"                       value of 0 is the default and indicates that Aften\n"
"                       should try to detect the number of CPUs.\n",

"    [-nosimd X]    Comma-separated list of SIMD instruction sets not to use\n"
"                       Aften will auto-detect available SIMD instruction sets\n"
"                       for your CPU, so you shouldn't need to disable sets\n"
"                       explicitly - unless for speed or debugging reasons.\n"
"                       Available sets are mmx, sse, sse2, sse3 and altivec.\n"
"                       No spaces are allowed between the sets and the commas.\n"
"                       Example: -nosimd sse2,sse3\n",

"    [-b #]         CBR bitrate in kbps\n"
"                       CBR mode is selected by default. This option allows for\n"
"                       setting the fixed bitrate. The default bitrate depends\n"
"                       on the number of channels (not including LFE).\n"
"                       mono      =  96 kbps\n"
"                       stereo    = 192 kbps\n"
"                       3-channel = 256 kbps\n"
"                       4-channel = 384 kbps\n"
"                       5-channel = 448 kbps\n",

"    [-q #]         VBR quality\n"
"                       A value 0 to 1023 which corresponds to SNR offset, where\n"
"                       q=240 equates to an SNR offset of 0.  240 is the default\n"
"                       value.  This scale will most likely be replaced in the\n"
"                       future with a better quality measurement.\n",

"    [-fba #]      Fast bit allocation\n"
"                       Fast bit allocation is a less-accurate search method\n"
"                       for CBR bit allocation.  It only narrows down the SNR\n"
"                       value to within 16 of the optimal value.  The result\n"
"                       is lower overall quality, but faster encoding.  This\n"
"                       may not give the same results each time when using\n"
"                       parallel encoding.\n",

"    [-exps #]     Exponent strategy search size\n"
"                       The encoder determines the best combination of\n"
"                       exponent strategies for a frame by searching through\n"
"                       a list of pre-defined exponent strategies.  This option\n"
"                       controls the size of the list to be searched.  The\n"
"                       value can range from 1 (lower quality but faster) to\n"
"                       32 (higher quality but slower).  The default value is 8.\n",

"    [-pad #]      Start-of-stream padding\n"
"                       The AC-3 format uses an overlap/add cycle for encoding\n"
"                       each block.  By default, Aften pads the delay buffer\n"
"                       with a block of silence to avoid inaccurate encoding\n"
"                       of the first frame of audio.  If this behavior is not\n"
"                       wanted, it can be disabled.  The pad value can be a\n"
"                       1 (default) to use padding or 0 to not use padding.\n",

"    [-w #]         Bandwidth\n"
"                       The bandwidth setting corresponds to the high-frequency\n"
"                       cutoff.  Specifically, it sets the highest frequency bin\n"
"                       which is encoded.  The AC-3 format uses a 512-point MDCT\n"
"                       which gives 256 frequency levels from 0 to 1/2 of the\n"
"                       samplerate.  The formula to give the number of coded\n"
"                       frequency bins from bandwidth setting is:\n"
"                       (w * 3) + 73, which gives a range of 73 to 253\n"
"                       I hope to replace this setting with one where the user\n"
"                       specifies the actual cutoff frequency rather than the\n"
"                       bandwidth code.\n"
"                       There are 2 special values, -1 and -2.\n"
"                       When -1 is used, Aften automatically selects what\n"
"                       it thinks is an appropriate bandwidth.  This is the\n"
"                       default setting.\n"
"                       When -2 is used, a bandwidth is chosen for each frame\n"
"                       based on CBR frame size and a target quality of 240.\n"
"                       Variable bandwidth cannot be used with VBR mode.\n",

"    [-wmin #]      Minimum bandwidth\n"
"                       For variable bandwidth mode (-2), this option sets the\n"
"                       minimum value for the bandwidth code.  This allows the\n"
"                       user to avoid a harsh cutoff frequency by sacrificing\n"
"                       general audio quality.  The default value is 0.\n",

"    [-wmax #]      Maximum bandwidth\n"
"                       For variable bandwidth mode (-2), this option sets the\n"
"                       maximum value for the bandwidth code.  This can be used\n"
"                       to speed up encoding by using a lower value than 60,\n"
"                       which is the default.\n",

"    [-m #]         Stereo rematrixing\n"
"                       Using stereo rematrixing can increase quality by\n"
"                       removing redundant information between the left and\n"
"                       right channels.  This technique is common in audio\n"
"                       encoding, and is sometimes called mid/side encoding.\n"
"                       When this setting is turned on, Aften adaptively turns\n"
"                       rematrixing on or off for each of 4 frequency bands for\n"
"                       each block.  When this setting is turned off,\n"
"                       rematrixing is not used for any blocks. The default\n"
"                       value is 1.\n",

"    [-s #]         Block switching\n"
"                       The AC-3 format allows for 2 different types of MDCT\n"
"                       transformations to translate from time-domain to\n"
"                       frequency-domain.  The default is a 512-point transform,\n"
"                       which gives better frequency resolution.  There is also\n"
"                       a 256-point transform, which gives better time\n"
"                       resolution.  The specification gives a suggested method\n"
"                       for determining when to use the 256-point transform.\n"
"                       When block switching is turned on, Aften uses the spec\n"
"                       method for selecting the 256-point MDCT.  When it is\n"
"                       turned off, only the 512-point MDCT is used, which is\n"
"                       faster.  Block switching is turned off by default.\n",
};

#define BSI_OPTIONS_COUNT 3

static const char bsi_heading[25] = "BITSTREAM INFO METADATA\n";
static const char *bsi_options[BSI_OPTIONS_COUNT] = {
"    [-cmix #]      Center mix level\n"
"                       When three front channels are in use, this code\n"
"                       indicates the nominal down mix level of the center\n"
"                       channel with respect to the left and right channels.\n"
"                       0 = -3.0 dB (default)\n"
"                       1 = -4.5 dB\n"
"                       2 = -6.0 dB\n",

"    [-smix #]      Surround mix level\n"
"                       If surround channels are in use, this code indicates\n"
"                       the nominal down mix level of the surround channels.\n"
"                       0 = -3 dB (default)\n"
"                       1 = -6 dB\n"
"                       2 = 0\n",

"    [-dsur #]      Dolby Surround mode\n"
"                       When operating in the two channel mode, this code\n"
"                       indicates whether or not the program has been encoded in\n"
"                       Dolby Surround.  This information is not used by the\n"
"                       AC-3 decoder, but may be used by other portions of the\n"
"                       audio reproduction equipment.\n"
"                       0 = not indicated (default)\n"
"                       1 = not Dolby surround encoded\n"
"                       2 = Dolby surround encoded\n"
};

#define DRC_OPTIONS_COUNT 2

static const char drc_heading[52] = "DYNAMIC RANGE COMPRESSION AND DIALOG NORMALIZATION\n";
static const char *drc_options[DRC_OPTIONS_COUNT] = {
"    [-dynrng #]    Dynamic Range Compression profile\n"
"                       Dynamic Range Compression allows for the final output\n"
"                       dynamic range to be limited without sacrificing quality.\n"
"                       The full dynamic range audio is still encoded, but a\n"
"                       code is given for each block which tells the decoder to\n"
"                       adjust the output volume for that block.  The encoder\n"
"                       must analyze the input audio to determine the best way\n"
"                       to compress the dynamic range based on the loudness and\n"
"                       type of input (film, music, speech).\n"
"                       0 = Film Light\n"
"                       1 = Film Standard\n"
"                       2 = Music Light\n"
"                       3 = Music Standard\n"
"                       4 = Speech\n"
"                       5 = None (default)\n",

"    [-dnorm #]     Dialog normalization [0 - 31] (default: 31)\n"
"                       The dialog normalization value sets the average dialog\n"
"                       level.  The value is typically constant for a particular\n"
"                       audio program.  The decoder has a target output dialog\n"
"                       level of -31dB, so if the dialog level is specified as\n"
"                       being -31dB already (default), the output volume is not\n"
"                       altered.  Otherwise, the overall output volume is\n"
"                       decreased so that the dialog level is adjusted down to\n"
"                       -31dB.\n"
};

#define INPUT_OPTIONS_COUNT 10

static const char input_heading[17] = "INPUT OPTIONS\n";
static const char *input_options[INPUT_OPTIONS_COUNT] = {
"    By default, information about the input file, such as the channel\n"
"    configuration and data size, is determined by the input file wav header.\n"
"    However, the basic WAVE format is limited in that it can only support\n"
"    up to 4 GB of data and cannot specify all channel layouts possible in the\n"
"    AC-3 format.  The acmod, lfe, and chconfig options allow the user to\n"
"    explicitly select the desired channel layout.  This only controls the\n"
"    interpretation of the input, so no downmixing or upmixing is done.  The\n"
"    readtoeof option overrides the header and lets the user specify that Aften\n"
"    should keep reading data until the end-of-file.\n",

"    [-acmod #]     Audio coding mode (overrides wav header)\n"
"                       0 = 1+1 (Ch1,Ch2)\n"
"                       1 = 1/0 (C)\n"
"                       2 = 2/0 (L,R)\n"
"                       3 = 3/0 (L,R,C)\n"
"                       4 = 2/1 (L,R,S)\n"
"                       5 = 3/1 (L,R,C,S)\n"
"                       6 = 2/2 (L,R,SL,SR)\n"
"                       7 = 3/2 (L,R,C,SL,SR)\n",

"    [-lfe #]       Specify use of LFE channel (overrides wav header)\n"
"                       0 = LFE channel is not present\n"
"                       1 = LFE channel is present\n",

"    [-chconfig X]  Specify channel configuration (overrides wav header)\n"
"                       1+1 = (Ch1,Ch2)\n"
"                       1/0 = (C)\n"
"                       2/0 = (L,R)\n"
"                       3/0 = (L,R,C)\n"
"                       2/1 = (L,R,S)\n"
"                       3/1 = (L,R,C,S)\n"
"                       2/2 = (L,R,SL,SR)\n"
"                       3/2 = (L,R,C,SL,SR)\n"
"                       adding \"+LFE\" indicates use of the LFE channel\n",

"    [-ch_X file]   Add a mono file to the input list as the channel specified\n"
"                       These parameters are used to specify multiple mono\n"
"                       source files instead of a single multi-channel source\n"
"                       file.  Only valid AC-3 combinations are allowed.  The\n"
"                       acmod, lfe, chconfig, and chmap parameters are all\n"
"                       ignored if multi-mono inputs are used.\n"
"                       ch_fl  = Front Left\n"
"                       ch_fc  = Front Center\n"
"                       ch_fr  = Front Right\n"
"                       ch_sl  = Surround Left\n"
"                       ch_s   = Surround\n"
"                       ch_sr  = Surround Right\n"
"                       ch_m1  = Dual Mono Channel 1\n"
"                       ch_m2  = Dual Mono Channel 2\n"
"                       ch_lfe = LFE\n",

"    [-raw_fmt X]   Raw audio input sample format (default: s16_le)\n"
"                       This options specifies the sample format when using\n"
"                       raw audio input.  Using this option forces Aften to\n"
"                       treat the input as raw audio.  The choices for the\n"
"                       pre-defined sample formats are:\n"
"                       u8, s8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,\n"
"                       s32_le, s32_be, float_le, float_be, double_le, double_be\n",

"    [-raw_sr #]    Raw audio input sample rate (default: 48000)\n"
"                       Using this option forces Aften to treat the input as\n"
"                       raw audio.\n",

"    [-raw_ch #]    Raw audio input channels (default: 1)\n"
"                       Using this option forces Aften to treat the input as\n"
"                       raw audio.\n",

"    [-chmap #]     Channel mapping order of input audio\n"
"                       Some programs create WAVE files which use a channel\n"
"                       mapping other than the standard WAVE mapping.  This\n"
"                       option allows the user to specify if the input file\n"
"                       uses WAVE, AC-3, or MPEG channel mapping.  The MPEG\n"
"                       channel mapping is used by DTS and by MPEG-2/4 formats\n"
"                       such as MP2 and AAC.\n"
"                       0 = WAVE mapping (default)\n"
"                       1 = AC-3 mapping\n"
"                       2 = MPEG mapping\n",

"    [-readtoeof #] Read input WAVE audio data until the end-of-file.\n"
"                       This overrides the data size in the WAVE header, and\n"
"                       can be useful for streaming input or files larger than\n"
"                       4 GB.\n"
"                       0 = use data size in header (default)\n"
"                       1 = read data until end-of-file\n"
};

#define FILTER_OPTIONS_COUNT 3

static const char filter_heading[15] = "INPUT FILTERS\n";
static const char *filter_options[FILTER_OPTIONS_COUNT] = {
"    [-bwfilter #]  Specify use of the bandwidth low-pass filter\n"
"                       The bandwidth low-pass filter pre-filters the input\n"
"                       audio before converting to frequency-domain.  This\n"
"                       smooths the cutoff frequency transition for slightly\n"
"                       better quality.  When used with variable bandwidth\n"
"                       mode, the maximum bandwidth setting used as the cutoff\n"
"                       frequency.\n"
"                       0 = do not apply filter (default)\n"
"                       1 = apply filter\n",

"    [-dcfilter #]  Specify use of the DC high-pass filter\n"
"                       The DC high-pass filter is listed as optional by the\n"
"                       AC-3 specification.  The implementation, as suggested,\n"
"                       is a single pole filter at 3 Hz.\n"
"                       0 = do not apply filter (default)\n"
"                       1 = apply filter\n",

"    [-lfefilter #] Specify use of the LFE low-pass filter\n"
"                       The LFE low-pass filter is recommended by the AC-3\n"
"                       specification.  The cutoff is 120 Hz.  The specification\n"
"                       recommends an 8th order elliptic filter, but instead,\n"
"                       Aften uses a Butterworth 2nd order cascaded direct\n"
"                       form II filter.\n"
"                       0 = do not apply filter (default)\n"
"                       1 = apply filter\n"
};

#define ALT_OPTIONS_COUNT 12

static const char alt_heading[29] = "ALTERNATE BIT STREAM SYNTAX\n";
static const char *alt_options[ALT_OPTIONS_COUNT] = {
"    The alternate bit stream syntax allows for encoding additional metadata by\n"
"    adding 1 or 2 extended bitstream info sections to each frame.\n",

"    [-xbsi1 #]     Specify use of extended bitstream info 1\n"
"                       Extended bitstream info 1 contains the dmixmod,\n"
"                       ltrtcmix, ltrtsmix, lorocmix, and lorosmix fields.  If\n"
"                       this option is turned on, all these values are written\n"
"                       to the output stream.\n"
"                       0 = do not write xbsi1\n"
"                       1 = write xbsi1\n",

"    [-dmixmod #]   Preferred stereo downmix mode\n"
"                       This code indicates the type of stereo downmix preferred\n"
"                       by the mastering engineer, and can be optionally used,\n"
"                       overridden, or ignored by the decoder.\n"
"                       0 = not indicated (default)\n"
"                       1 = Lt/Rt downmix preferred\n"
"                       2 = Lo/Ro downmix preferred\n",

"    The 4 mix level options below all use the same code table:\n"
"                       0 = +3.0 dB\n"
"                       1 = +1.5 dB\n"
"                       2 =  0.0 dB\n"
"                       3 = -1.5 dB\n"
"                       4 = -3.0 dB (default)\n"
"                       5 = -4.5 dB\n"
"                       6 = -6.0 dB\n"
"                       7 = -inf dB\n",

"    [-ltrtcmix #]  Lt/Rt center mix level\n"
"                       This code indicates the nominal down mix level of the\n"
"                       center channel with respect to the left and right\n"
"                       channels in an Lt/Rt downmix.\n",

"    [-ltrtsmix #]  Lt/Rt surround mix level\n"
"                       This code indicates the nominal down mix level of the\n"
"                       surround channels with respect to the left and right\n"
"                       channels in an Lt/Rt downmix.\n",

"    [-lorocmix #]  Lo/Ro center mix level\n"
"                       This code indicates the nominal down mix level of the\n"
"                       center channel with respect to the left and right\n"
"                       channels in an Lo/Ro downmix.\n",

"    [-lorosmix #]  Lo/Ro surround mix level\n"
"                       This code indicates the nominal down mix level of the\n"
"                       surround channels with respect to the left and right\n"
"                       channels in an Lo/Ro downmix.\n",

"    [-xbsi2 #]     Specify use of extended bitstream info 2\n"
"                       Extended bitstream info 2 contains the dsurexmod,\n"
"                       dheadphon, and adconvtyp fields.  If this option is\n"
"                       turned on, all these values are written to the output\n"
"                       stream.  These options are not used by the AC-3 decoder,\n"
"                       but may be used by other portions of the audio\n"
"                       reproduction equipment.\n"
"                       0 = do not write xbsi2\n"
"                       1 = write xbsi2\n",

"    [-dsurexmod #] Dolby Surround EX mode\n"
"                       This code indicates whether or not the program has been\n"
"                       encoded in Dolby Surround EX.\n"
"                       0 = not indicated (default)\n"
"                       1 = Not Dolby Surround EX encoded\n"
"                       2 = Dolby Surround EX encoded\n",

"    [-dheadphon #] Dolby Headphone mode\n"
"                       This code indicates whether or not the program has been\n"
"                       Dolby Headphone-encoded.\n"
"                       0 = not indicated (default)\n"
"                       1 = Not Dolby Headphone encoded\n"
"                       2 = Dolby Headphone encoded\n",

"    [-adconvtyp #] A/D converter type\n"
"                       This code indicates the type of A/D converter technology\n"
"                       used to capture the PCM audio.\n"
"                       0 = Standard (default)\n"
"                       1 = HDCD\n"
};

typedef struct {
    int section_count;
    const char *section_heading;
    const char **section_options;
} LongHelpSection;

#define LONG_HELP_SECTIONS_COUNT 7

static const LongHelpSection long_help_sections[LONG_HELP_SECTIONS_COUNT] = {
    {   CONSOLE_OPTIONS_COUNT,
        console_heading,
        console_options },
    {   ENCODING_OPTIONS_COUNT,
        encoding_heading,
        encoding_options },
    {   BSI_OPTIONS_COUNT,
        bsi_heading,
        bsi_options },
    {   DRC_OPTIONS_COUNT,
        drc_heading,
        drc_options },
    {   INPUT_OPTIONS_COUNT,
        input_heading,
        input_options },
    {   FILTER_OPTIONS_COUNT,
        filter_heading,
        filter_options },
    {   ALT_OPTIONS_COUNT,
        alt_heading,
        alt_options }
};

static void
print_usage(FILE *out)
{
    fprintf(out, "%s", usage_heading);
    fprintf(out, "type 'aften -h' for more details.\n\n");
}

static void
print_long_help(FILE *out)
{
    int i, j;

    fprintf(out, "%s", usage_heading);
    fprintf(out, "options:\n\n");

    for (i = 0; i < LONG_HELP_SECTIONS_COUNT; i++) {
        fprintf(out, "%s\n", long_help_sections[i].section_heading);
        for (j = 0; j < long_help_sections[i].section_count; j++) {
            fprintf(out, "%s\n", long_help_sections[i].section_options[j]);
        }
    }
}

static void
print_help(FILE *out)
{
    int i;

    fprintf(out, "%s", usage_heading);
    fprintf(out, "options:\n");

    for (i = 0; i < HELP_OPTIONS_COUNT; i++) {
        fprintf(out, "%s", help_options[i]);
    }
    fprintf(out, "\n");
}

#endif /* HELPTEXT_H */
