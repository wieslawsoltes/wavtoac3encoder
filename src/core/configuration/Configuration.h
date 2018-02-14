#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <wchar.h>
#include <iostream>
#include <sstream>
#include <cstdio>
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
        std::map<int, int> nOptions;
    };

    class CEncoderOptions
    {
    public:
        std::vector<int> nValidCbrBitrates;
        std::vector<CChannelConfig> m_ChannelConfig;
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
    };

    class CConfiguration
    {
    public:
        std::map<int, std::wstring> * pStrings;
        std::vector<CLanguage> m_LangLst;
        std::wstring m_szLangFileName;
        int m_nLangId;
    public:
        bool m_bIsPortable;
        std::wstring m_szPresetsFilePath;
        std::wstring m_szConfigFilePath;
        std::wstring m_szEnginesFilePath;
        std::wstring m_szFilesListFilePath;
        std::wstring m_szLangFilePath;
    public:
        std::vector<config::Entry> m_Engines;
        std::vector<config::CPreset> m_Presets;
        config::CPreset m_DefaultPreset;
        int nCurrentPreset;
        std::wstring szOutputPath;
        std::wstring szOutputFile;
        bool bMultipleMonoInput;
        bool bDisableAllWarnings;
        bool bSaveConfig;
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
        int FindValidBitratePos(const int nBitrate);
        int FindOptionIndex(std::wstring szOption);
        void ParsePreset(CPreset &preset, std::vector<Entry> &cl);
        bool LoadPresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        bool SavePresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        bool IsSupportedInputExt(std::wstring &szExt);
        int GetSupportedInputFormat(std::wstring &szExt);
    public:
        void SetEncoderOptions();
        std::wstring GetSupportedInputFilesFilter();
    };
}
