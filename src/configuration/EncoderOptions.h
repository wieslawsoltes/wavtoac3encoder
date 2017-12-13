#pragma once

#include "utilities\ListT.h"
#include "worker\AftenAPI.h"

class CChannelConfig
{
public:
    int acmod;
    int lfe;
    CString chconfig;
public:
    CChannelConfig()
    {
    }
    CChannelConfig(int acmod, int lfe, CString chconfig)
    {
        this->acmod = acmod;
        this->lfe = lfe;
        this->chconfig = chconfig;
    }
    CChannelConfig(const CChannelConfig &other)
    {
        Copy(other);
    }
    CChannelConfig& operator=(const CChannelConfig &other)
    {
        Copy(other);
        return *this;
    }
    virtual ~CChannelConfig()
    {
    }
public:
    void Copy(const CChannelConfig &other)
    {
        this->acmod = other.acmod;
        this->lfe = other.lfe;
        this->chconfig = other.chconfig;
    }
};

class CEncoderOptions
{
public:
    CString szName;
    CString szOption;
    CString szHelpText;
    CListT<CString> listOptNames;
    CListT<int> listOptValues;
    int nDefaultValue;
    int nIgnoreValue;
    CString szGroupName;
    bool bBeginGroup;
public:
    CEncoderOptions()
    {
    }
    CEncoderOptions(const CEncoderOptions &other)
    {
        Copy(other);
    }
    CEncoderOptions& operator=(const CEncoderOptions &other)
    {
        Copy(other);
        return *this;
    }
    virtual ~CEncoderOptions()
    {
    }
public:
    void Copy(const CEncoderOptions &other)
    {
        this->szName = other.szName;
        this->szOption = other.szOption;
        this->szHelpText = other.szHelpText;
        this->listOptNames = other.listOptNames;
        this->listOptValues = other.listOptValues;
        this->nDefaultValue = other.nDefaultValue;
        this->nIgnoreValue = other.nIgnoreValue;
        this->szGroupName = other.szGroupName;
        this->bBeginGroup = other.bBeginGroup;
    }
};

class CEncoderPreset
{
public:
    const static int nNumEncoderOptions = 31;
    const static int nNumSIMDIntructions = 4;
public:
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
public:
    CEncoderPreset()
    {
    }
    CEncoderPreset(const CEncoderPreset &other)
    {
        Copy(other);
    }
    CEncoderPreset& operator=(const CEncoderPreset &other)
    {
        Copy(other);
        return *this;
    }
    virtual ~CEncoderPreset()
    {
    }
public:
    void Copy(const CEncoderPreset &other)
    {
        this->szName = other.szName;
        this->nMode = other.nMode;
        this->nBitrate = other.nBitrate;
        this->nQuality = other.nQuality;
        this->nRawSampleFormat = other.nRawSampleFormat;
        this->nRawSampleRate = other.nRawSampleRate;
        this->nRawChannels = other.nRawChannels;

        for (int i = 0; i < nNumSIMDIntructions; i++)
        {
            this->nUsedSIMD[i] = other.nUsedSIMD[i];
        }

        this->nThreads = other.nThreads;
        this->nCurrentEngine = other.nCurrentEngine;

        for (int i = 0; i < nNumEncoderOptions; i++)
        {
            this->nSetting[i] = other.nSetting[i];
        }
    }
};

typedef CListT<CEncoderPreset> EncoderPresetList_t;

const static int nNumMaxInputFiles = 6;
const static int nNumValidCbrBitrates = 20;
const static int nNumChannelConfigAften = 16;
const static int nNumRawSampleFormats = 15;
const static int nNumEncoderOptionsGroups = 6;
const static int nNumSupportedInputExt = 8;
const static int nNumSupportedOutputExt = 1;

// current version of presets configuration (format: #.###)
const LPTSTR szCurrentPresetsVersion = _T("1.1.0.0");

extern const int nValidCbrBitrates[nNumValidCbrBitrates];

extern const CChannelConfig ccAften[nNumChannelConfigAften];

// default values for raw audio input
extern LPTSTR szRawSampleFormats[nNumRawSampleFormats];

// advanced options global variable
extern CEncoderOptions encOpt[CEncoderPreset::nNumEncoderOptions];

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
bool LoadEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, CEncoderPreset& defaultPreset);

// save encoder presets
bool SaveEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, CEncoderPreset& defaultPreset);

// check if file extension is supported
bool IsSupportedInputExt(CString &szExt);

// get supported input format
int GetSupportedInputFormat(CString &szExt);

// create input file filter for open file dialogs
CString GetSupportedInputFilesFilter();
