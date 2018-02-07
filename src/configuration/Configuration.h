#pragma once

#include <string>
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

#define DEFAULT_PRESET_NAME (app::m_Config.HaveLangStrings() ? app::m_Config.GetLangString(0x00207001) : _T("Default"))
#define DEFAULT_TEXT_AUTO (app::m_Config.HaveLangStrings() ? app::m_Config.GetLangString(0x00207002) : _T("<Auto>"))
#define DEFAULT_TEXT_IGNORED (app::m_Config.HaveLangStrings() ? app::m_Config.GetLangString(0x00207003) : _T("<Ignored>"))
#define DEFAULT_TEXT_OUTPUT_PATH (app::m_Config.HaveLangStrings() ? app::m_Config.GetLangString(0x00207004) : _T("<Same as input file path>"))
#define DEFAULT_TEXT_OUTPUT_FILE (app::m_Config.HaveLangStrings() ? app::m_Config.GetLangString(0x00207005) : _T("<Same as first input file path + output.ac3>"))

namespace config
{
    class CConfigEntry
    {
    public:
        std::wstring szKey;
        std::wstring szValue;
    };

    class CConfigList : public util::CListT<CConfigEntry>
    {
    };

    class CLangMap : public util::CMapT<int, std::wstring>
    {
    };

    class CLang
    {
    public:
        std::wstring szFileName;
        std::wstring szEnglishName;
        std::wstring szTargetName;
        CLangMap lm;
    };

    class CLangList : public util::CListT<CLang>
    {
    };

    class CConfiguration
    {
    public:
        constexpr static std::wstring pszReadMode = L"rt, ccs=UTF-8";
        constexpr static std::wstring pszWriteMode = L"wt, ccs=UTF-8";
    public:
        CLangMap * m_Lang;
        CLangList m_LangLst;
        std::wstring m_szLangFileName = L"";
        BOOL m_bHaveLang = FALSE;
        int m_nLangId = -1;
    public:
        bool m_bIsPortable = true;
        std::wstring m_szPresetsFilePath;
        std::wstring m_szConfigFilePath;
        std::wstring m_szEnginesFilePath;
        std::wstring m_szFilesListFilePath;
        std::wstring m_szLangFilePath;
    public:
        static bool LoadConfig(std::wstring &szFileName, CConfigList &cl);
        static bool SaveConfig(std::wstring &szFileName, CConfigList &cl);
    public:
        bool LoadFiles(std::wstring &szFileName, util::CListT<std::wstring>& fl);
        bool SaveFiles(std::wstring &szFileName, util::CListT<std::wstring>& fl, int nFormat);
    public:
        void SearchFolderForLang(std::wstring szPath, const bool bRecurse, CLangList& m_LangLst);
        bool LoadLang(std::wstring &szFileName, CLangMap &lm);
    public:
        bool LoadLangConfig(std::wstring &szFileName);
        bool SaveLangConfig(std::wstring &szFileName);
    public:
        void LoadLangStrings();
        BOOL HaveLangStrings();
        std::wstring GetLangString(int id);
    };

    class CChannelConfig
    {
    public:
        int acmod;
        int lfe;
        std::wstring chconfig;
    public:
        CChannelConfig(int acmod, int lfe, std::wstring chconfig)
        {
            this->acmod = acmod;
            this->lfe = lfe;
            this->chconfig = chconfig;
        }
    };

    class CEncoderOptions
    {
    public:
        std::wstring szName;
        std::wstring szOption;
        std::wstring szHelpText;
        util::CListT<std::wstring> listOptNames;
        util::CListT<int> listOptValues;
        int nDefaultValue;
        int nIgnoreValue;
        std::wstring szGroupName;
        bool bBeginGroup;
    };

    class CEncoderPreset
    {
    public:
        const static int nNumEncoderOptions = 31;
        const static int nNumSIMDIntructions = 4;
    public:
        std::wstring szName;
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
    };

    class CEncoderPresetList : public util::CListT<CEncoderPreset>
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
        static std::wstring szCurrentPresetsVersion;
        static int nValidCbrBitrates[nNumValidCbrBitrates];
        static CChannelConfig ccAften[nNumChannelConfigAften];
        static std::wstring szRawSampleFormats[nNumRawSampleFormats];
        static std::wstring pszGroups[nNumEncoderOptionsGroups];
        static std::wstring szCbrOption;
        static std::wstring szVbrOption;
        static std::wstring szThreadsOption;
        static std::wstring szSimdOption;
        static std::wstring szRawSampleFormatOption;
        static std::wstring szRawSampleRateOption;
        static std::wstring szRawChannelsOption;
        static std::wstring szSupportedInputExt[nNumSupportedInputExt];
        static int nSupportedInputFormats[nNumSupportedInputExt];
        static std::wstring szSupportedOutputExt[nNumSupportedOutputExt];
        static CEncoderOptions encOpt[CEncoderPreset::nNumEncoderOptions];
    public:
        static void InitEncoderOptions();
        static int FindValidBitratePos(const int nBitrate);
        static int FindOptionIndex(std::wstring szOption);
        static void ResetEncoderOptionsLists();
        static void ParseEncoderPreset(CEncoderPreset &preset, CConfigList &cl);
        static bool LoadEncoderPresets(CEncoderPresetList& encPresets, std::wstring& szFileName, CEncoderPreset& defaultPreset);
        static bool SaveEncoderPresets(CEncoderPresetList& encPresets, std::wstring& szFileName, CEncoderPreset& defaultPreset);
        static bool IsSupportedInputExt(std::wstring &szExt);
        static int GetSupportedInputFormat(std::wstring &szExt);
        static CString GetSupportedInputFilesFilter();
    };
}
