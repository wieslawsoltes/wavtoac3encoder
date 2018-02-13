#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <wchar.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <atlstr.h>
#include "worker\AftenAPI.h"

namespace config
{
    typedef std::pair<std::wstring, std::wstring> Entry;

    class CLanguage
    {
    public:
        std::wstring szFileName;
        std::wstring szEnglishName;
        std::wstring szTargetName;
        std::map<int, std::wstring> m_Strings;
    };

    class CConfiguration
    {
    public:
        std::map<int, std::wstring> * pStrings;
        std::vector<CLanguage> m_LangLst;
        std::wstring m_szLangFileName = L"";
        int m_nLangId = -1;
    public:
        bool m_bIsPortable = true;
        std::wstring m_szPresetsFilePath;
        std::wstring m_szConfigFilePath;
        std::wstring m_szEnginesFilePath;
        std::wstring m_szFilesListFilePath;
        std::wstring m_szLangFilePath;
    public:
        static bool LoadConfig(std::wstring &szFileName, std::vector<Entry> &cl);
        static bool SaveConfig(std::wstring &szFileName, std::vector<Entry> &cl);
    public:
        bool LoadFiles(std::wstring &szFileName, std::vector<std::wstring>& fl);
        bool SaveFiles(std::wstring &szFileName, std::vector<std::wstring>& fl, int nFormat);
    public:
        bool SearchFolderForLang(std::wstring szPath, const bool bRecurse, std::vector<CLanguage>& m_LangLst);
        bool LoadLang(std::wstring &szFileName, std::map<int, std::wstring> &m_Strings);
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

    class COption
    {
    public:
        std::wstring szName;
        std::wstring szOption;
        std::wstring szHelpText;
        std::vector<std::wstring> m_Names;
        std::vector<int> m_Values;
        int nDefaultValue;
        int nIgnoreValue;
        std::wstring szGroupName;
        bool bBeginGroup;
    };

    class CRawInput
    {
    public:
        int nRawSampleFormat;
        int nRawSampleRate;
        int nRawChannels;
    };

    class CPreset
    {
    public:
        const static int nNumEncoderOptions = 31;
        const static int nNumSIMDIntructions = 4;
    public:
        std::wstring szName;
        AftenEncMode nMode;
        int nBitrate;
        int nQuality;
        CRawInput m_RawInput;
        int nUsedSIMD[nNumSIMDIntructions];
        int nThreads;
        int nCurrentEngine;
        int nOptions[nNumEncoderOptions];
    };

    class CDefaults
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
        static COption encOpt[CPreset::nNumEncoderOptions];
    public:
        static void InitEncoderOptions();
        static int FindValidBitratePos(const int nBitrate);
        static int FindOptionIndex(std::wstring szOption);
        static void ParsePreset(CPreset &preset, std::vector<Entry> &cl);
        static bool LoadPresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        static bool SavePresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        static bool IsSupportedInputExt(std::wstring &szExt);
        static int GetSupportedInputFormat(std::wstring &szExt);
        static CAtlString GetSupportedInputFilesFilter();
    };

    extern CConfiguration m_Config;
}
