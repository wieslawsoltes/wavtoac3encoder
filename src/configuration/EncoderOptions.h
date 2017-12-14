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
    CString szName;
    AftenEncMode nMode;
    int nBitrate;
    int nQuality;
    int nRawSampleFormat;
    int nRawSampleRate;
    int nRawChannels;
    int nUsedSIMD[nNumSIMDIntructions];
    int nThreads;
    int nCurrentEngine;
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

class CEncoderDefaults
{
private:
    CEncoderDefaults() { }
public:
    const static int nNumMaxInputFiles = 6;
    const static int nNumValidCbrBitrates = 20;
    const static int nNumChannelConfigAften = 16;
    const static int nNumRawSampleFormats = 15;
    const static int nNumEncoderOptionsGroups = 6;
    const static int nNumSupportedInputExt = 8;
    const static int nNumSupportedOutputExt = 1;
public:
    static LPTSTR szCurrentPresetsVersion = _T("1.1.0.0");
    static int nValidCbrBitrates[nNumValidCbrBitrates];
    static CChannelConfig ccAften[nNumChannelConfigAften];
    static LPTSTR szRawSampleFormats[nNumRawSampleFormats];
    static CEncoderOptions encOpt[CEncoderPreset::nNumEncoderOptions];
    static CString pszGroups[nNumEncoderOptionsGroups];
    static CString szCbrOption;
    static CString szVbrOption;
    static CString szThreadsOption;
    static CString szSimdOption;
    static CString szRawSampleFormatOption;
    static CString szRawSampleRateOption;
    static CString szRawChannelsOption;
    static TCHAR szSupportedInputExt[nNumSupportedInputExt][8];
    static int nSupportedInputFormats[nNumSupportedInputExt];
    static TCHAR szSupportedOutputExt[nNumSupportedOutputExt][8];
    static int FindValidBitratePos(const int nBitrate);
public:
    static int FindOptionIndex(CString szOption);
    static void ResetEncoderOptionsLists();
    static void InitEncoderOptions();
    static bool LoadEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, CEncoderPreset& defaultPreset);
    static bool SaveEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, CEncoderPreset& defaultPreset);
    static bool IsSupportedInputExt(CString &szExt);
    static int GetSupportedInputFormat(CString &szExt);
    static CString GetSupportedInputFilesFilter();
};
