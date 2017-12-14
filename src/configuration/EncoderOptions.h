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
    static constexpr LPTSTR szCurrentPresetsVersion = _T("1.1.0.0");
    static constexpr int nValidCbrBitrates[nNumValidCbrBitrates]
    {
        0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640
    };
    static CChannelConfig ccAften[nNumChannelConfigAften]
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
    static constexpr LPTSTR szRawSampleFormats[nNumRawSampleFormats]
    {
        (LPTSTR)(LPCTSTR)(DEFAULT_TEXT_IGNORED),
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
    static CEncoderOptions encOpt[CEncoderPreset::nNumEncoderOptions];
    static CString pszGroups[nNumEncoderOptionsGroups]
    {
        _T("Encoding options"),
        _T("Bitstream info metadata"),
        _T("Dynamic range compression and dialog normalization"),
        _T("Input options"),
        _T("Input filters"),
        _T("Alternate bit stream syntax")
    };
    static constexpr CString szCbrOption = _T("-b");
    static constexpr CString szVbrOption = _T("-q");
    static constexpr CString szThreadsOption = _T("-threads");
    static constexpr CString szSimdOption = _T("-nosimd");
    static constexpr CString szRawSampleFormatOption = _T("-raw_fmt");
    static constexpr CString szRawSampleRateOption = _T("-raw_sr");
    static constexpr CString szRawChannelsOption = _T("-raw_ch");
    static constexpr TCHAR szSupportedInputExt[nNumSupportedInputExt][8]
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
    static constexpr int nSupportedInputFormats[nNumSupportedInputExt]
    {
        PCM_FORMAT_WAVE,
        PCM_FORMAT_RAW,
        PCM_FORMAT_RAW,
        PCM_FORMAT_RAW,
        PCM_FORMAT_AIFF,
        PCM_FORMAT_AIFF,
        PCM_FORMAT_CAFF,
    };
    static constexpr TCHAR szSupportedOutputExt[nNumSupportedOutputExt][8]
    {
        _T("ac3")
    };
public:
    static void InitEncoderOptions();
    static int FindValidBitratePos(const int nBitrate);
    static int FindOptionIndex(CString szOption);
    static void ResetEncoderOptionsLists();
    static void ParseEncoderPreset(CEncoderPreset &preset, ConfigList_t &clTmp);
    static bool LoadEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, CEncoderPreset& defaultPreset);
    static bool SaveEncoderPresets(EncoderPresetList_t& encPresets, CString szFileName, CEncoderPreset& defaultPreset);
    static bool IsSupportedInputExt(CString &szExt);
    static int GetSupportedInputFormat(CString &szExt);
    static CString GetSupportedInputFilesFilter();
};
