#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <wchar.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include "logger\Log.h"
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

    class CEngine
    {
    public:
        std::wstring szName;
        std::wstring szPath;
        int nThreads;
        std::map<int, int> nUsedSIMD;
    public:
        CEngine() { }
        CEngine(std::wstring name, std::wstring path) : szName(name), szPath(path)
        {
            this->nThreads = 0;
            this->nUsedSIMD = {
                { 0, 1 },
                { 1, 1 },
                { 2, 1 },
                { 3, 1 }
            };
        }
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
        static const int nDefaultRawChannels = 0;
        static const int nDefaultRawSampleFormat = 0;
        static const int nDefaultRawSampleRate = 0;
    public:
        int nRawSampleFormat;
        int nRawSampleRate;
        int nRawChannels;
    };

    class CPreset
    {
    public:
        static const AftenEncMode nDefaultMode = AFTEN_ENC_MODE_CBR;
        static const int nDefaultBitrate = 0;
        static const int nDefaultQuality = 240;
    public:
        std::wstring szName;
        AftenEncMode nMode;
        int nBitrate;
        int nQuality;
        CRawInput m_RawInput;
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

    class CFile
    {
    public:
        std::wstring szPath;
        unsigned __int64 nSize;
        bool bStatus;
    };

    class CConfiguration
    {
    public:
        std::unique_ptr<logger::ILog> Log;
        std::map<int, std::wstring> * pStrings;
        std::vector<CLanguage> m_Languages;
        std::wstring m_szLangFileName;
        int m_nLangId;
        bool m_bIsPortable;
        std::wstring szLogFilePath;
        std::wstring szPresetsFilePath;
        std::wstring szConfigFilePath;
        std::wstring szEnginesFilePath;
        std::wstring szFilesListFilePath;
        std::wstring szLangFilePath;
        std::vector<CEngine> m_Engines;
        int nCurrentEngine;
        std::vector<config::CPreset> m_Presets;
        config::CPreset m_DefaultPreset;
        int nCurrentPreset;
        std::vector<CFile> m_Files;
        std::wstring szOutputPath;
        std::wstring szOutputFile;
        bool bUseOutputPath;
        bool bMultiMonoInput;
        bool bDisableAllWarnings;
        bool bSaveConfig;
        CEncoderOptions m_EncoderOptions;
    public:
        bool LoadEntries(std::wstring &szFileName, std::vector<Entry> &entries);
        bool SaveEntries(std::wstring &szFileName, std::vector<Entry> &entries);
        bool LoadFiles(std::wstring &szFileName, std::vector<std::wstring>& files);
        bool SaveFiles(std::wstring &szFileName, std::vector<std::wstring>& files, int nFormat);
        bool FindLanguages(std::wstring szPath, const bool bRecurse, std::vector<CLanguage>& languages);
        bool LoadStrings(std::wstring &szFileName, std::map<int, std::wstring> &strings);
        bool LoadLanguagePath(std::wstring &szFileName);
        bool SaveLanguagePath(std::wstring &szFileName);
        void LoadLLanguages(std::wstring szLangPath);
        std::wstring CConfiguration::GetString(const int nKey);
        void ParseEngineEntries(CEngine &engine, std::vector<Entry> &entries);
        bool LoadEngines(std::vector<CEngine>& engines, std::wstring& szFileName);
        bool SaveEngines(std::vector<CEngine>& engines, std::wstring& szFileName);
        void ParsePresetEntries(CPreset &preset, std::vector<Entry> &entries);
        bool LoadPresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        bool SavePresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset);
        int FindValidBitrateIndex(const int nBitrate);
        int FindOptionIndex(std::wstring szOption);
        bool IsSupportedInputExt(std::wstring &szExt);
        int GetSupportedInputFormat(std::wstring &szExt);
        std::wstring GetSupportedInputFilesFilter();
        CPreset& GetCurrentPreset();
        CEngine& GetCurrentEngine();
        void SetEncoderOptions();
    };
}
