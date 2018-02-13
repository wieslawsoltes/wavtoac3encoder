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
    typedef std::pair<std::wstring, int> Value;

    class CLanguage
    {
    public:
        std::wstring szFileName;
        std::wstring szEnglishName;
        std::wstring szTargetName;
        std::map<int, std::wstring> m_Strings;
    };

    class CChannelConfig
    {
    public:
        int acmod;
        int lfe;
        std::wstring chconfig;
    };

    class COption
    {
    public:
        std::wstring szName;
        std::wstring szOption;
        std::wstring szHelpText;
        std::vector<Value> m_Values;
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
        std::wstring szName;
        AftenEncMode nMode;
        int nBitrate;
        int nQuality;
        CRawInput m_RawInput;
        std::vector<int> nUsedSIMD;
        int nThreads;
        int nCurrentEngine;
        std::vector<int> nOptions;
    };

    class CEncoderOptions
    {
    public:
        std::vector<int> nValidCbrBitrates;
        std::vector<CChannelConfig> ccAften;
        std::vector<std::wstring> szRawSampleFormats;
        std::vector<std::wstring> szGroups;
        std::wstring szCbrOption;
        std::wstring szVbrOption;
        std::wstring szThreadsOption;
        std::wstring szSimdOption;
        std::wstring szRawSampleFormatOption;
        std::wstring szRawSampleRateOption;
        std::wstring szRawChannelsOption;
        std::vector<std::wstring> szSupportedInputExt;
        std::vector<int> nSupportedInputFormats;
        std::vector<std::wstring> szSupportedOutputExt;
        std::vector<COption> m_Options;
    public:
        void InitEncoderOptions();
        int FindValidBitratePos(const int nBitrate);
        int FindOptionIndex(std::wstring szOption);
        void ParsePreset(CPreset &preset, std::vector<Entry> &cl);
        bool LoadPresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        bool SavePresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        bool IsSupportedInputExt(std::wstring &szExt);
        int GetSupportedInputFormat(std::wstring &szExt);
        CAtlString GetSupportedInputFilesFilter();
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
        CEncoderOptions m_EncoderOptions;
    public:
        bool LoadConfig(std::wstring &szFileName, std::vector<Entry> &cl);
        bool SaveConfig(std::wstring &szFileName, std::vector<Entry> &cl);
        bool LoadFiles(std::wstring &szFileName, std::vector<std::wstring>& fl);
        bool SaveFiles(std::wstring &szFileName, std::vector<std::wstring>& fl, int nFormat);
        bool SearchFolderForLang(std::wstring szPath, const bool bRecurse, std::vector<CLanguage>& m_LangLst);
        bool LoadLang(std::wstring &szFileName, std::map<int, std::wstring> &m_Strings);
        bool LoadLangConfig(std::wstring &szFileName);
        bool SaveLangConfig(std::wstring &szFileName);
        void LoadLangStrings(std::wstring szLangPath);
        std::wstring CConfiguration::GetString(const int nKey);
    public:
    };

    extern CConfiguration m_Config;
}
