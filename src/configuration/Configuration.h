#pragma once

#include "version.h"
#include "utilities\ListT.h"
#include "utilities\MapT.h"
#include "worker\AftenAPI.h"

#define ENCWAVTOAC3_VERSION _T(VER_FILE_VERSION_SHORT_STR)
#define ENCWAVTOAC3_URL_HOME _T("https://github.com/wieslawsoltes/wavtoac3encoder/")

#ifdef _M_X64
#define FILENAME_ENGINES _T("EncWAVtoAC3-x64.engines")
#else
#define FILENAME_ENGINES _T("EncWAVtoAC3-x86.engines")
#endif

#ifdef _M_X64
#define DIRECTORY_CONFIG _T("EncWAVtoAC3-x64")
#else
#define DIRECTORY_CONFIG _T("EncWAVtoAC3-x86")
#endif

#define FILENAME_LANG _T("EncWAVtoAC3.lang")
#define FILENAME_PORTABLE _T("EncWAVtoAC3.portable")
#define FILENAME_CONFIG _T("EncWAVtoAC3.config")
#define FILENAME_PRESETS _T("EncWAVtoAC3.presets")
#define FILENAME_FILES _T("EncWAVtoAC3.files")

#define DEFAULT_PRESET_NAME (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207001) : _T("Default"))
#define DEFAULT_TEXT_AUTO (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207002) : _T("<Auto>"))
#define DEFAULT_TEXT_IGNORED (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207003) : _T("<Ignored>"))
#define DEFAULT_TEXT_OUTPUT_PATH (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207004) : _T("<Same as input file path>"))
#define DEFAULT_TEXT_OUTPUT_FILE (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207005) : _T("<Same as first input file path + output.ac3>"))

class CConfigEntry
{
public:
    CString szKey;
    CString szValue;
public:
    CConfigEntry()
    {
    }
    CConfigEntry(const CConfigEntry &other)
    {
        Copy(other);
    }
    CConfigEntry& operator=(const CConfigEntry &other)
    {
        Copy(other);
        return *this;
    }
    virtual ~CConfigEntry()
    {
    }
public:
    void Copy(const CConfigEntry &other)
    {
        this->szKey = other.szKey;
        this->szValue = other.szValue;
    }
};

class CConfigList : public CListT<CConfigEntry>
{
};

class CLangMap : public CMapT<int, CString>
{
};

class CLang
{
public:
    CString szFileName;
    CString szEnglishName;
    CString szTargetName;
    CLangMap lm;
public:
    CLang()
    {
    }
    CLang(const CLang &other)
    {
        Copy(other);
    }
    CLang& operator=(const CLang &other)
    {
        Copy(other);
        return *this;
    }
    virtual ~CLang()
    {
    }
public:
    void Copy(const CLang &other)
    {
        this->szFileName = other.szFileName;
        this->szEnglishName = other.szEnglishName;
        this->szTargetName = other.szTargetName;
        this->lm = other.lm;
    }
};

class CLangList : public CListT<CLang>
{
};

class CConfiguration
{
public:
    CConfiguration();
    virtual ~CConfiguration();
public:
    constexpr static LPTSTR pszReadMode = _T("rt, ccs=UTF-8");
    constexpr static LPTSTR pszWriteMode = _T("wt, ccs=UTF-8");
public:
    CLangMap * m_Lang;
    CLangList m_LangLst;
    CString m_szLangFileName = _T("");
    BOOL m_bHaveLang = FALSE;
    int m_nLangId = -1;
public:
    bool m_bIsPortable = true;
    CString m_szPresetsFilePath;
    CString m_szConfigFilePath;
    CString m_szEnginesFilePath;
    CString m_szFilesListFilePath;
    CString m_szLangFilePath;
public:
    static bool LoadConfig(CString &szFileName, CConfigList &cl);
    static bool SaveConfig(CString &szFileName, CConfigList &cl);
public:
    bool LoadFiles(CString &szFileName, CListT<CString>& fl);
    bool SaveFiles(CString &szFileName, CListT<CString>& fl, int nFormat);
public:
    void SearchFolderForLang(CString szPath, const bool bRecurse, CLangList& m_LangLst);
    bool LoadLang(CString &szFileName, CLangMap &lm);
public:
    bool LoadLangConfig(CString &szFileName);
    bool SaveLangConfig(CString &szFileName);
public:
    void LoadLangStrings();
    BOOL HaveLangStrings();
    CString GetLangString(int id);
};

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

class CEncoderPresetList : public CListT<CEncoderPreset>
{
};

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
    static LPTSTR szCurrentPresetsVersion;
    static int nValidCbrBitrates[nNumValidCbrBitrates];
    static CChannelConfig ccAften[nNumChannelConfigAften];
    static LPTSTR szRawSampleFormats[nNumRawSampleFormats];
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
    static CEncoderOptions encOpt[CEncoderPreset::nNumEncoderOptions];
public:
    static void InitEncoderOptions();
    static int FindValidBitratePos(const int nBitrate);
    static int FindOptionIndex(CString szOption);
    static void ResetEncoderOptionsLists();
    static void ParseEncoderPreset(CEncoderPreset &preset, CConfigList &cl);
    static bool LoadEncoderPresets(CEncoderPresetList& encPresets, CString szFileName, CEncoderPreset& defaultPreset);
    static bool SaveEncoderPresets(CEncoderPresetList& encPresets, CString szFileName, CEncoderPreset& defaultPreset);
    static bool IsSupportedInputExt(CString &szExt);
    static int GetSupportedInputFormat(CString &szExt);
    static CString GetSupportedInputFilesFilter();
};
