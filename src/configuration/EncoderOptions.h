#pragma once

#include "worker\AftenAPI.h"

typedef struct TChannelConfig
{
    int acmod;
    int lfe;
    CString chconfig;
} ChannelConfig;

typedef struct TEncoderOptions
{
    CString szName;
    CString szOption;
    CString szHelpText;
    CList<CString, CString&> listOptNames;
    CList<int, int&> listOptValues;
    int nDefaultValue;
    int nIgnoreValue;
    CString szGroupName;
    bool bBeginGroup;
} EncoderOptions;

// number of advanced encoder options
const int nNumEncoderOptions = 31;

// number of SIMD instructions
const int nNumSIMDIntructions = 4;

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

typedef CList<EncoderPreset, EncoderPreset&> EncoderPresetList_t;

// define max number of input files
const int nNumMaxInputFiles = 6;

// current version of presets configuration (format: #.###)
const LPTSTR szCurrentPresetsVersion = _T("1.1.0.0");

// number of valid CBR mode bitrates
const int nNumValidCbrBitrates = 20;

extern const int nValidCbrBitrates[nNumValidCbrBitrates];

const int nNumChannelConfigAften = 16;

extern const ChannelConfig ccAften[nNumChannelConfigAften];

// number of valid raw audio input formats
const int nNumRawSampleFormats = 15;

// default values for raw audio input
extern LPTSTR szRawSampleFormats[nNumRawSampleFormats];

// number of advanced encoder options groups
const int nNumEncoderOptionsGroups = 6;

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

// define supported input file extension
const int nNumSupportedInputExt = 8;

// define supported output file extension
const int nNumSupportedOutputExt = 1;

// supported input file extensions
extern const TCHAR szSupportedInputExt[nNumSupportedInputExt][8];

// supported input file formats
extern const int nSupportedInputFormats[nNumSupportedInputExt];

// supported output file extensions
extern const TCHAR szSupportedOutputExt[nNumSupportedOutputExt][8];

// find valid bitrate index
int FindValidBitratePos(const int nBitrate);

int FindOptionIndex(CString szOption);

void ResetEncoderOptionsLists();

// initialize advanced encoder options
void InitEncoderOptions();

// load encoder presets
bool LoadEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, EncoderPreset& defaultPreset);

// save encoder presets
bool SaveEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, EncoderPreset& defaultPreset);

// check if file extension is supported
bool IsSupportedInputExt(CString &szExt);

// get supported input format
int GetSupportedInputFormat(CString &szExt);

// create input file filter for open file dialogs
CString GetSupportedInputFilesFilter();
