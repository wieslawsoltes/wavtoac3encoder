#pragma once

#include <atlstr.h>
#include <string>
#include <wchar.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <utility>
#include <vector>
#include "Strings.h"
#include "utilities\ListT.h"
#include "utilities\MapT.h"
#include "worker\AftenAPI.h"

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
        bool SearchFolderForLang(std::wstring szPath, const bool bRecurse, CLangList& m_LangLst);
        bool LoadLang(std::wstring &szFileName, CLangMap &lm);
    public:
        bool LoadLangConfig(std::wstring &szFileName);
        bool SaveLangConfig(std::wstring &szFileName);
    public:
        void LoadLangStrings(std::wstring szLangPath);
    public:
        std::wstring CConfiguration::GetString(const int nKey);
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

    class CEncoderOption
    {
    public:
        std::wstring szName;
        std::wstring szOption;
        std::wstring szHelpText;
        util::CListT<std::wstring> m_Names;
        util::CListT<int> m_Values;
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
    public:
        const static int nNumMaxInputFiles = 6;
        const static int nNumValidCbrBitrates = 20;
        const static int nNumChannelConfigAften = 16;
        const static int nNumRawSampleFormats = 15;
        const static int nNumEncoderOptionsGroups = 6;
        const static int nNumSupportedInputExt = 8;
        const static int nNumSupportedOutputExt = 1;
    public:
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
        static CEncoderOption encOpt[CEncoderPreset::nNumEncoderOptions];
    public:
        static void InitEncoderOptions();
        static int FindValidBitratePos(const int nBitrate);
        static int FindOptionIndex(std::wstring szOption);
        static void ParseEncoderPreset(CEncoderPreset &preset, CConfigList &cl);
        static bool LoadEncoderPresets(CEncoderPresetList& encPresets, std::wstring& szFileName, CEncoderPreset& defaultPreset);
        static bool SaveEncoderPresets(CEncoderPresetList& encPresets, std::wstring& szFileName, CEncoderPreset& defaultPreset);
        static bool IsSupportedInputExt(std::wstring &szExt);
        static int GetSupportedInputFormat(std::wstring &szExt);
        static CAtlString GetSupportedInputFilesFilter();
    };

    extern CConfiguration m_Config;
}
