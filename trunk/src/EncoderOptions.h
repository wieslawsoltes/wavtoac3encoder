//
// WAV to AC3 Encoder
// Copyright (C) 2007-2013 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#pragma once

// define max number of input files
#define NUM_MAX_INPUT_FILES 6

// current version of presets configuration (format: #.###)
#define CURRENT_PRESETS_VERSION _T("1.1.0.0")

// number of valid CBR mode bitrates
const int nNumValidCbrBitrates = 20;

extern const int nValidCbrBitrates[nNumValidCbrBitrates];

typedef struct TChannelConfig
{
    int acmod;
    int lfe;
    CString chconfig;
} ChannelConfig;

const int nNumChannelConfigAften = 16;

extern const ChannelConfig ccAften[nNumChannelConfigAften];

// number of valid raw audio input formats
const int nNumRawSampleFormats = 15;

// default values for raw audio input
extern LPTSTR szRawSampleFormats[nNumRawSampleFormats];

// find valid bitrate index
int FindValidBitratePos(const int nBitrate);

typedef struct TEncoderOptions
{
    CString szName;
    CString szOption;
    CString szHelpText;
    CList<CString,CString&> listOptNames;
    CList<int,int&> listOptValues;
    int nDefaultValue;
    int nIgnoreValue;
    CString szGroupName;
    bool bBeginGroup;
} EncoderOptions;

// number of advanced encoder options
const int nNumEncoderOptions = 31;

// number of advanced encoder options groups
const int nNumEncoderOptionsGroups = 6;

// number of SIMD instructions
const int nNumSIMDIntructions = 4;

// advanced options global variable
extern EncoderOptions encOpt[nNumEncoderOptions];

// encoder options groups
extern CString pszGroups[nNumEncoderOptionsGroups];

// option for CBR encoding mode for aften
extern const CString szCbrOption;

// option for VBR encoding mode for aften
extern const CString szVbrOption;

// option for number of threads for aften
extern const CString szThreadsOption;

// option for SIMD instructions for aften
extern const CString szSimdOption;

// option for raw audio input sample format for aften
extern const CString szRawSampleFormatOption;

// option for raw audio input sample rate for aften
extern const CString szRawSampleRateOption;

// option for raw audio input channels for aften
extern const CString szRawChannelsOption;

// encoder preset settings
typedef struct TEncoderPreset
{
    // preset name
    CString szName;
    // encoding mode (CBR or VBR)
    AftenEncMode nMode;
    // bitrate for CBR mode or quality for VBR mode
    int nBitrate;
    int nQuality;
    // raw audio input
    int nRawSampleFormat;
    int nRawSampleRate;
    int nRawChannels;
    // enable specific SIMD instructions: 0=MMX, 1=SSE, 2=SSE2, 3=SSE3
    int nUsedSIMD[nNumSIMDIntructions];
    // number of threads
    int nThreads;
    // currently used encoder engine
    // if set to -1 then no engine is used
    // if set to 0 or above then user engine is used
    int nCurrentEngine;
    // list of encoder settings
    int nSetting[nNumEncoderOptions];
} EncoderPreset;

// list of encoder presets
extern CList<EncoderPreset,EncoderPreset&> encPresets;

// default preset
extern EncoderPreset defaultPreset;

// current preset index
extern int nCurrentPreset;

// get currently selected preset
extern EncoderPreset GetCurrentPreset();

// update currently selected preset
void UpdateCurrentPreset(EncoderPreset updatePreset);

// find option index
int FindOptionIndex(CString szOption);

void ResetEncoderOptionsLists();

// initialize advanced encoder options
void InitEncoderOptions();

// load encoder presets
bool LoadEncoderPresets(CString szFileName);

// save encoder presets
bool SaveEncoderPresets(CString szFileName);

// define supported input file extension
#define NUM_SUPPORTED_INPUT_EXT 8

// define supported output file extension
#define NUM_SUPPORTED_OUTPUT_EXT 1

// supported input file extensions
extern const TCHAR szSupportedInputExt[NUM_SUPPORTED_INPUT_EXT][8];

// supported input file formats
extern const int nSupportedInputFormats[NUM_SUPPORTED_INPUT_EXT];

// supported output file extensions
extern const TCHAR szSupportedOutputExt[NUM_SUPPORTED_OUTPUT_EXT][8];

// check if file extension is supported
bool IsSupportedInputExt(CString &szExt);

// get supported input format
int GetSupportedInputFormat(CString &szExt);

// create input file filter for open file dialogs
CString GetSupportedInputFilesFilter();
