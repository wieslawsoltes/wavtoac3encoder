#pragma once

#include "StdAfx.h"
#include "utilities\StringHelper.h"
#include "utilities\Utilities.h"
#include "Configuration.h"
#include "Strings.h"

namespace config
{
    const std::wstring szReadMode { L"rt, ccs=UTF-8" };
    const std::wstring szWriteMode { L"wt, ccs=UTF-8" };
    wchar_t Separator { '=' };
    const std::wstring szSeparator = L"=";
    const std::wstring szNewCharVar = L"\\n";
    const std::wstring szNewChar = L"\n";
    const std::wstring szTabCharVar = L"\\t";
    const std::wstring szTabChar = L"\t";

    std::wstring ReadAllText(const std::wstring& szFileName)
    {
        std::wstring buffer;
        FILE *fs;
        errno_t error = _wfopen_s(&fs, szFileName.c_str(), szReadMode.c_str());
        if (error != 0)
            return buffer;

        struct _stat fileinfo;
        _wstat(szFileName.c_str(), &fileinfo);
        size_t size = fileinfo.st_size;

        if (size > 0)
        {
            buffer.resize(size);
            size_t wchars_read = std::fread(&(buffer.front()), sizeof(wchar_t), size, fs);
            buffer.resize(wchars_read);
            buffer.shrink_to_fit();
        }

        fclose(fs);
        return buffer;
    }

    int CConfiguration::GetSupportedInputFormat(std::wstring &szExt)
    {
        for (int i = 0; i < (int)m_EncoderOptions.szSupportedInputExt.size(); i++)
        {
            if (util::StringHelper::CompareNoCase(szExt, m_EncoderOptions.szSupportedInputExt[i]))
                return m_EncoderOptions.nSupportedInputFormats[i];
        }
        return PCM_FORMAT_UNKNOWN;
    }

    bool CConfiguration::LoadConfig(std::wstring &szFileName, std::vector<Entry> &cl)
    {
        try
        {
            std::wstring data = ReadAllText(szFileName);
            if (data.empty())
                return false;

            std::wistringstream stream;
            stream.str(data);
            for (std::wstring szBuffer; std::getline(stream, szBuffer);) 
            {
                auto parts = util::StringHelper::Split(szBuffer.c_str(), Separator);
                if (parts.size() == 2)
                {
                    cl.emplace_back(std::make_pair(parts[0], parts[1]));
                }
            }
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::SaveConfig(std::wstring &szFileName, std::vector<Entry> &cl)
    {
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer;
            int nSize = (int)cl.size();
            for (int i = 0; i < nSize; i++)
            {
                auto& ce = cl[i];
                szBuffer = ce.first + szSeparator + ce.second + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);
            }

            fclose(fs);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::LoadFiles(std::wstring &szFileName, std::vector<std::wstring>& fl)
    {
        try
        {
            std::wstring data = ReadAllText(szFileName);
            if (data.empty())
                return false;

            std::wistringstream stream;
            stream.str(data);
            for (std::wstring szBuffer; std::getline(stream, szBuffer);) 
            {
                if (szBuffer.size() > 0)
                {
                    util::StringHelper::Trim(szBuffer, '"');
                    if (IsSupportedInputExt(util::Utilities::GetFileExtension(szBuffer)) == true)
                    {
                        std::wstring szPath = szBuffer;
                        fl.emplace_back(szPath);
                    }
                }
                szBuffer = L"";
            }

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::SaveFiles(std::wstring &szFileName, std::vector<std::wstring>& fl, int nFormat)
    {
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer;
            int nItems = (int)fl.size();
            for (int i = 0; i < nItems; i++)
            {
                std::wstring &szPath = fl[i];
                szBuffer = (nFormat == 0 ? L"" : L"\"") + szPath + (nFormat == 0 ? L"" : L"\"") + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);
            }

            fclose(fs);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::SearchFolderForLang(std::wstring szPath, const bool bRecurse, std::vector<CLanguage>& m_LangLst)
    {
        try
        {
            std::vector<std::wstring> files;
            bool bResult = util::Utilities::FindFiles(szPath, files, false);
            if (bResult == true)
            {
                for (auto& file : files)
                {
                    std::wstring ext = util::Utilities::GetFileExtension(file);
                    if (util::StringHelper::TowLower(ext) == L"txt")
                    {
                        CLanguage lang;
                        if (this->LoadLang(file, lang.m_Strings) == true)
                        {
                            lang.szFileName = file;
                            lang.szEnglishName = (lang.m_Strings.count(0x00000001) == 1) ? lang.m_Strings[0x00000001] : L"??";
                            lang.szTargetName = (lang.m_Strings.count(0x00000002) == 1) ? lang.m_Strings[0x00000002] : L"??";
                            m_LangLst.emplace_back(std::move(lang));
                        }
                    }
                }
            }
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::LoadLang(std::wstring &szFileName, std::map<int, std::wstring>& m_Strings)
    {
        try
        {
            std::wstring data = ReadAllText(szFileName);
            if (data.empty())
                return false;

            m_Strings.clear();

            std::wstring szBuffer = L"";
            std::wistringstream stream;
            stream.str(data);

            for (std::wstring szBuffer; std::getline(stream, szBuffer);) 
            {
                auto parts = util::StringHelper::Split(szBuffer.c_str(), Separator);
                if (parts.size() == 2)
                {
                    util::StringHelper::Replace(parts[1], szNewCharVar, szNewChar);
                    util::StringHelper::Replace(parts[1], szTabCharVar, szTabChar);
                    m_Strings[util::StringHelper::ToIntFromHex(parts[0])] = parts[1];
                }
                szBuffer = L"";
            }
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::LoadLangConfig(std::wstring &szFileName)
    {
        try
        {
            std::wstring data = ReadAllText(szFileName);
            if (data.empty())
                return false;

            std::wistringstream stream;
            stream.str(data);
            for (std::wstring szBuffer; std::getline(stream, szBuffer);) 
            {
                m_szLangFileName = szBuffer;
                return true;
            }
            return false;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::SaveLangConfig(std::wstring &szFileName)
    {
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer = m_szLangFileName + szNewChar;
            std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

            fclose(fs);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    void CConfiguration::LoadLangStrings(std::wstring szLangPath)
    {
        SearchFolderForLang(szLangPath, false, m_LangLst);

        if (m_LangLst.size() > 0)
        {
            bool haveLang = false;
            for (int i = 0; i < (int)m_LangLst.size(); i++)
            {
                auto& lang = m_LangLst[i];
                std::wstring szNameLang = util::Utilities::GetFileName(lang.szFileName);
                std::wstring szNameConfig = util::Utilities::GetFileName(m_szLangFileName);
                if (szNameLang == szNameConfig)
                {
                    m_nLangId = i;
                    pStrings = &lang.m_Strings;
                    haveLang = true;
                    break;
                }
            }

            if (haveLang == false)
            {
                auto& lang = m_LangLst[0];
                m_nLangId = 0;
                pStrings = &lang.m_Strings;
                m_szLangFileName = lang.szFileName;
            }
        }
        else
        {
            m_nLangId = -1;
        }
    }

    std::wstring CConfiguration::GetString(const int nKey)
    {
        if (pStrings != nullptr)
        {
            if (pStrings->count(nKey) == 1)
                return (*pStrings)[nKey];
        }

        if (m_Strings.count(nKey) == 1)
            return m_Strings.at(nKey);

        return L"??";
    }

    int CConfiguration::FindValidBitratePos(const int nBitrate)
    {
        for (int i = 0; i < (int)m_EncoderOptions.nValidCbrBitrates.size(); i++)
        {
            if (m_EncoderOptions.nValidCbrBitrates[i] == nBitrate)
                return i;
        }
        return 0;
    }

    int CConfiguration::FindOptionIndex(std::wstring szOption)
    {
        for (int i = 0; i < (int)m_EncoderOptions.m_Options.size(); i++)
        {
            std::wstring szBuffer = m_EncoderOptions.m_Options[i].szOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (szOption == szBuffer)
                return i;
        }
        return 0;
    }

    void CConfiguration::ParsePreset(CPreset &preset, std::vector<Entry> &cl)
    {
        for (int i = 0; i < (int)cl.size(); i++)
        {
            std::wstring szBuffer;
            auto& ce = cl[i];

            if (ce.first == L"engine")
            {
                preset.nCurrentEngine = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = m_EncoderOptions.szThreadsOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nThreads = util::StringHelper::ToInt(ce.second);
                continue;
            }

            if (ce.first == L"mmx")
            {
                preset.nUsedSIMD[0] = util::StringHelper::ToInt(ce.second);
                continue;
            }

            if (ce.first == L"sse")
            {
                preset.nUsedSIMD[1] = util::StringHelper::ToInt(ce.second);
                continue;
            }

            if (ce.first == L"sse2")
            {
                preset.nUsedSIMD[2] = util::StringHelper::ToInt(ce.second);
                continue;
            }

            if (ce.first == L"sse3")
            {
                preset.nUsedSIMD[3] = util::StringHelper::ToInt(ce.second);
                continue;
            }

            if (ce.first == L"mode")
            {
                preset.nMode = (AftenEncMode)util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = m_EncoderOptions.szCbrOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nBitrate = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = m_EncoderOptions.szVbrOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nQuality = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = m_EncoderOptions.szRawSampleFormatOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.m_RawInput.nRawSampleFormat = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = m_EncoderOptions.szRawSampleRateOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.m_RawInput.nRawSampleRate = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = m_EncoderOptions.szRawChannelsOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.m_RawInput.nRawChannels = util::StringHelper::ToInt(ce.second);
                continue;
            }

            for (int i = 0; i < (int)m_EncoderOptions.m_Options.size(); i++)
            {
                szBuffer = m_EncoderOptions.m_Options[i].szOption;
                util::StringHelper::TrimLeft(szBuffer, '-');
                if (ce.first == szBuffer)
                {
                    preset.nOptions[i] = util::StringHelper::ToInt(ce.second);
                    break;
                }
            }
        }
    }

    bool CConfiguration::LoadPresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset)
    {
        try
        {
            std::wstring data = ReadAllText(szFileName);
            if (data.empty())
                return false;

            CPreset temp;
            std::vector<Entry> cl;
            bool bHavePreset = false;
            presets.clear();

            std::wistringstream stream;
            stream.str(data);
            for (std::wstring szBuffer; std::getline(stream, szBuffer);) 
            {
                if ((szBuffer[0] == '[') && (szBuffer[szBuffer.size() - 1] == ']'))
                {
                    if (bHavePreset == true)
                    {
                        ParsePreset(temp, cl);
                        auto preset = temp;
                        presets.emplace_back(preset);
                        cl.clear();
                    }

                    temp = defaultPreset;
                    util::StringHelper::TrimLeft(szBuffer, '[');
                    util::StringHelper::TrimRight(szBuffer, ']');
                    temp.szName = szBuffer;
                    bHavePreset = true;
                }
                else
                {
                    auto parts = util::StringHelper::Split(szBuffer.c_str(), Separator);
                    if (parts.size() == 2)
                    {
                        cl.emplace_back(std::make_pair(parts[0], parts[1]));
                    }
                }
            }

            if (bHavePreset == true)
            {
                ParsePreset(temp, cl);
                auto preset = temp;
                presets.emplace_back(preset);
                cl.clear();
            }

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::SavePresets(std::vector<CPreset>& presets, std::wstring& szFileName, CPreset& defaultPreset)
    {
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer;
            std::wstring szTmpBuffer;

            for (int i = 0; i < (const int)presets.size(); i++)
            {
                auto& preset = presets[i];

                szBuffer = L"[" + preset.szName + L"]\n";
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"engine" + szSeparator + std::to_wstring(preset.nCurrentEngine) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = m_EncoderOptions.szThreadsOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nThreads) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"mmx" + szSeparator + std::to_wstring(preset.nUsedSIMD[0]) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"sse" + szSeparator + std::to_wstring(preset.nUsedSIMD[1]) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"sse2" + szSeparator + std::to_wstring(preset.nUsedSIMD[2]) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"sse3" + szSeparator + std::to_wstring(preset.nUsedSIMD[3]) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"mode" + szSeparator + std::to_wstring(preset.nMode) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = m_EncoderOptions.szCbrOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nBitrate) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = m_EncoderOptions.szVbrOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nQuality) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = m_EncoderOptions.szRawSampleFormatOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.m_RawInput.nRawSampleFormat) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = m_EncoderOptions.szRawSampleRateOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.m_RawInput.nRawSampleRate) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = m_EncoderOptions.szRawChannelsOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.m_RawInput.nRawChannels) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                for (int j = 0; j < (int)m_EncoderOptions.m_Options.size(); j++)
                {
                    szTmpBuffer = m_EncoderOptions.m_Options[j].szOption;
                    util::StringHelper::TrimLeft(szTmpBuffer, '-');
                    szBuffer = szTmpBuffer + szSeparator+ std::to_wstring(preset.nOptions[j]) + szNewChar;
                    std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);
                }
            }

            fclose(fs);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::IsSupportedInputExt(std::wstring &szExt)
    {
        for (int i = 0; i < (int)m_EncoderOptions.szSupportedInputExt.size(); i++)
        {
            if (util::StringHelper::CompareNoCase(szExt, m_EncoderOptions.szSupportedInputExt[i]))
                return true;
        }
        return false;
    }

    void CConfiguration::SetEncoderOptions()
    {
        m_EncoderOptions.nValidCbrBitrates = {
            0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640
        };

        m_EncoderOptions.m_ChannelConfig = {
            { 0, 0, L"1+1" },
            { 1, 0, L"1/0" },
            { 2, 0, L"2/0" },
            { 3, 0, L"3/0" },
            { 4, 0, L"2/1" },
            { 5, 0, L"3/1" },
            { 6, 0, L"2/2" },
            { 7, 0, L"3/2" },
            { 0, 1, L"1+1" },
            { 1, 1, L"1/0" },
            { 2, 1, L"2/0" },
            { 3, 1, L"3/0" },
            { 4, 1, L"2/1" },
            { 5, 1, L"3/1" },
            { 6, 1, L"2/2" },
            { 7, 1, L"3/2" }
        };

        m_EncoderOptions.szRawSampleFormats = {
            this->GetString(0x00207003),
            L"u8",
            L"s8",
            L"s16_le",
            L"s16_be",
            L"s20_le",
            L"s20_be",
            L"s24_le",
            L"s24_be",
            L"s32_le",
            L"s32_be",
            L"float_le",
            L"float_be",
            L"double_le",
            L"double_be"
        };

        m_EncoderOptions.szGroups = {
            this->GetString(0x00208001),
            this->GetString(0x00208002),
            this->GetString(0x00208003),
            this->GetString(0x00208004),
            this->GetString(0x00208005),
            this->GetString(0x00208006)
        };

        m_EncoderOptions.szCbrOption = L"-b";
        m_EncoderOptions.szVbrOption = L"-q";
        m_EncoderOptions.szThreadsOption = L"-threads";
        m_EncoderOptions.szSimdOption = L"-nosimd";
        m_EncoderOptions.szRawSampleFormatOption = L"-raw_fmt";
        m_EncoderOptions.szRawSampleRateOption = L"-raw_sr";
        m_EncoderOptions.szRawChannelsOption = L"-raw_ch";

        m_EncoderOptions.szSupportedInputExt = {
            L"wav",
            L"pcm",
            L"raw",
            L"bin",
            L"aiff",
            L"aif",
            L"aifc",
            L"avs"
        };

        m_EncoderOptions.nSupportedInputFormats = {
            PCM_FORMAT_WAVE,
            PCM_FORMAT_RAW,
            PCM_FORMAT_RAW,
            PCM_FORMAT_RAW,
            PCM_FORMAT_AIFF,
            PCM_FORMAT_AIFF,
            PCM_FORMAT_CAFF,
        };

        m_EncoderOptions.szSupportedOutputExt = {
           L"ac3"
        };

        #define AddOption(name, option, tip, dval, ival, group, begin) \
            m_EncoderOptions.m_Options.emplace_back(COption()); \
            nCurOpt++; \
            m_EncoderOptions.m_Options[nCurOpt].szName = name; \
            m_EncoderOptions.m_Options[nCurOpt].szOption = option; \
            m_EncoderOptions.m_Options[nCurOpt].szHelpText = tip; \
            m_EncoderOptions.m_Options[nCurOpt].nDefaultValue = dval; \
            m_EncoderOptions.m_Options[nCurOpt].nIgnoreValue = ival; \
            m_EncoderOptions.m_Options[nCurOpt].szGroupName = group; \
            m_EncoderOptions.m_Options[nCurOpt].bBeginGroup = begin;
        #define AddValue(name, value) \
            m_EncoderOptions.m_Options[nCurOpt].m_Values.emplace_back(std::make_pair(name, value));
        #define AddValueRange(start, end) \
            for (int i = start; i <= end; i++) { \
                m_EncoderOptions.m_Options[nCurOpt].m_Values.emplace_back(std::make_pair(std::to_wstring(i), i)); \
            }

        int nCurOpt = -1;

        m_EncoderOptions.m_Options.clear();

        AddOption(GetString(0x00301001), L"-fba", GetString(0x00301002), 0, -1, GetString(0x00208001), true)
        AddValue(GetString(0x00301003), 0)
        AddValue(GetString(0x00301004), 1)

        AddOption(GetString(0x00302001), L"-exps", GetString(0x00302002), 7, -1, L"", false)
        AddValue(GetString(0x00302003), 1)
        AddValueRange(2, 7)
        AddValue(GetString(0x00302004), 8)
        AddValueRange(9, 31)
        AddValue(GetString(0x00302005), 32)

        AddOption(GetString(0x00303001), L"-pad", GetString(0x00303002), 1, -1, L"", false)
        AddValue(GetString(0x00303003), 0)
        AddValue(GetString(0x00303004), 1)

        AddOption(GetString(0x00304001), L"-w", GetString(0x00304002), 0, -1, L"", false)
        AddValue(GetString(0x00304003), -1)
        AddValue(GetString(0x00304004), -2)
        AddValue(GetString(0x00304005), 0)
        AddValueRange(1, 59)
        AddValue(GetString(0x00304006), 60)

        AddOption(GetString(0x00305001), L"-wmin", GetString(0x00305002), 0, -1, L"", false)
        AddValue(GetString(0x00305003), 0)
        AddValueRange(1, 59)
        AddValue(L"60", 60)

        AddOption(GetString(0x00306001), L"-wmax", GetString(0x00306002), 60, -1, L"", false)
        AddValue(L"0", 0)
        AddValueRange(1, 59)
        AddValue(GetString(0x00306003), 60)

        AddOption(GetString(0x00307001), L"-m", GetString(0x00307002), 1, -1, L"", false)
        AddValue(GetString(0x00307003), 0)
        AddValue(GetString(0x00307004), 1)

        AddOption(GetString(0x00308001), L"-s", GetString(0x00308002), 0, -1, L"", false)
        AddValue(GetString(0x00308003), 0)
        AddValue(GetString(0x00308004), 1)

        AddOption(GetString(0x00401001), L"-cmix", GetString(0x00401002), 0, -1, GetString(0x00208002), true)
        AddValue(GetString(0x00401003), 0)
        AddValue(L"-4.5 dB", 1)
        AddValue(L"-6.0 dB", 2)

        AddOption(GetString(0x00402001), L"-smix", GetString(0x00402002), 0, -1, L"", false)
        AddValue(GetString(0x00402003), 0)
        AddValue(L"-6 dB", 1)
        AddValue(L"0", 2)

        AddOption(GetString(0x00403001) , L"-dsur", GetString(0x00403002), 0, -1, L"", false)
        AddValue(GetString(0x00403003), 0)
        AddValue(GetString(0x00403004), 1)
        AddValue(GetString(0x00403005), 2)

        AddOption(GetString(0x00501001), L"-dnorm", GetString(0x00501002), 31, -1, GetString(0x00208003), true)
        AddValueRange(0, 30)
        AddValue(GetString(0x00501003), 31)

        AddOption(GetString(0x00502001), L"-dynrng", GetString(0x00502002), 5, -1, L"", false)
        AddValue(GetString(0x00502003), 0)
        AddValue(GetString(0x00502004), 1)
        AddValue(GetString(0x00502005), 2)
        AddValue(GetString(0x00502006), 3)
        AddValue(GetString(0x00502007), 4)
        AddValue(GetString(0x00502008), 5)

        AddOption(GetString(0x00601001), L"-acmod", GetString(0x00601002), 8, 8, GetString(0x00208004), true)
        AddValue(L"1+1 (Ch1,Ch2)", 0)
        AddValue(L"1/0 (C)", 1)
        AddValue(L"2/0 (L,R)", 2)
        AddValue(L"3/0 (L,R,C)", 3)
        AddValue(L"2/1 (L,R,S)", 4)
        AddValue(L"3/1 (L,R,C,S)", 5)
        AddValue(L"2/2 (L,R,SL,SR)", 6)
        AddValue(L"3/2 (L,R,C,SL,SR)", 7)
        AddValue(GetString(0x00207003), 0)

        AddOption(GetString(0x00602001), L"-lfe", GetString(0x00602002), 2, 2, L"", false)
        AddValue(GetString(0x00602003), 0)
        AddValue(GetString(0x00602004), 1)
        AddValue(GetString(0x00207003), 0)

        AddOption(GetString(0x00603001), L"-chconfig", GetString(0x00603002), 16, 16, L"", false)
        AddValue(L"1+1 = (Ch1,Ch2)", 0)
        AddValue(L"1/0 = (C)", 1)
        AddValue(L"2/0 = (L,R)", 2)
        AddValue(L"3/0 = (L,R,C)", 3)
        AddValue(L"2/1 = (L,R,S)", 4)
        AddValue(L"3/1 = (L,R,C,S)", 5)
        AddValue(L"2/2 = (L,R,SL,SR)", 6)
        AddValue(L"3/2 = (L,R,C,SL,SR)", 7)
        AddValue(L"1+1+LFE = (Ch1,Ch2) + LFE", 8)
        AddValue(L"1/0+LFE = (C) + LFE", 9)
        AddValue(L"2/0+LFE = (L,R) + LFE", 10)
        AddValue(L"3/0+LFE = (L,R,C) + LFE", 11)
        AddValue(L"2/1+LFE = (L,R,S) + LFE", 12)
        AddValue(L"3/1+LFE = (L,R,C,S) + LFE", 13)
        AddValue(L"2/2+LFE = (L,R,SL,SR) + LFE", 14)
        AddValue(L"3/2+LFE = (L,R,C,SL,SR) + LFE", 15)
        AddValue(GetString(0x00207003), 0)

        AddOption(GetString(0x00604001), L"-chmap", GetString(0x00604002), 0, -1, L"", false)
        AddValue(GetString(0x00604003), 0)
        AddValue(GetString(0x00604004), 1)
        AddValue(GetString(0x00604005), 2)

        AddOption(GetString(0x00605001), L"-readtoeof", GetString(0x00605002), 0, -1, L"", false)
        AddValue(GetString(0x00605003), 0)
        AddValue(GetString(0x00605004), 1)

        AddOption(GetString(0x00701001), L"-bwfilter", GetString(0x00701002), 0, -1, GetString(0x00208005), true)
        AddValue(GetString(0x00701003), 0)
        AddValue(GetString(0x00701004), 1)

        AddOption(GetString(0x00702001), L"-dcfilter", GetString(0x00702002), 0, -1, L"", false)
        AddValue(GetString(0x00702003), 0)
        AddValue(GetString(0x00702004), 1)

        AddOption(GetString(0x00703001), L"-lfefilter", GetString(0x00703002), 0, -1, L"", false)
        AddValue(GetString(0x00703003), 0)
        AddValue(GetString(0x00703004), 1)

        AddOption(GetString(0x00801001), L"-xbsi1", GetString(0x00801002), 2, 2, GetString(0x00208006), true)
        AddValue(GetString(0x00801003), 0)
        AddValue(GetString(0x00801004), 1)
        AddValue(GetString(0x00207003), 0)

        AddOption(GetString(0x00802001), L"-dmixmod", GetString(0x00802002), 0, -1, L"", false)
        AddValue(GetString(0x00802003), 0)
        AddValue(GetString(0x00802004), 1)
        AddValue(GetString(0x00802005), 2)

        AddOption(GetString(0x00803001), L"-ltrtcmix", GetString(0x00803002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00803003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        AddOption(GetString(0x00804001), L"-ltrtsmix", GetString(0x00804002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00804003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        AddOption(GetString(0x00805001), L"-lorocmix", GetString(0x00805002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00805003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        AddOption(GetString(0x00806001), L"-lorosmix", GetString(0x00806002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00806003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        AddOption(GetString(0x00807001), L"-xbsi2", GetString(0x00807002), 2, 2, L"", false)
        AddValue(GetString(0x00807003), 0)
        AddValue(GetString(0x00807004), 1)
        AddValue(GetString(0x00207003), 0)

        AddOption(GetString(0x00808001), L"-dsurexmod", GetString(0x00808002), 0, -1, L"", false)
        AddValue(GetString(0x00808003), 0)
        AddValue(GetString(0x00808004), 1)
        AddValue(GetString(0x00808005), 2)

        AddOption(GetString(0x00809001), L"-dheadphon", GetString(0x00809002), 0, -1, L"", false)
        AddValue(GetString(0x00809003), 0)
        AddValue(GetString(0x00809004), 1)
        AddValue(GetString(0x00809005), 2)

        AddOption(GetString(0x0080A001), L"-adconvtyp", GetString(0x0080A002), 0, -1, L"", false)
        AddValue(GetString(0x0080A003), 0)
        AddValue(GetString(0x0080A004), 1)

        #undef AddOption
        #undef AddValue
    }

    CAtlString CConfiguration::GetSupportedInputFilesFilter()
    {
        CAtlString szFilter = L"";
        CAtlString szExtL = L"";
        CAtlString szExtU = L"";
        CAtlString szBuff = L"";

        for (int i = 0; i < (int)m_EncoderOptions.szSupportedInputExt.size(); i++)
        {
            szExtL = m_EncoderOptions.szSupportedInputExt[i].c_str();
            szBuff = L"*." + szExtL.MakeLower();
            szBuff += (i < (int)m_EncoderOptions.szSupportedInputExt.size() - 1) ? L";" : L"";
            szFilter += szBuff;
        }

        szFilter = CAtlString(this->GetString(0x00207006).c_str()) + L" (" + szFilter + L")|" + szFilter + L"|";

        for (int i = 0; i < (int)m_EncoderOptions.szSupportedInputExt.size(); i++)
        {
            szExtL = szExtU = m_EncoderOptions.szSupportedInputExt[i].c_str();
            szExtU.MakeUpper();
            szExtL.MakeLower();
            szBuff.Format(L"%s %s (*.%s)|*.%s|", szExtU, this->GetString(0x00207007).c_str(), szExtL, szExtL);
            szFilter += szBuff;
        }

        szFilter += CAtlString(this->GetString(0x00207008).c_str()) + L" (*.*)|*.*||";
        return szFilter;
    }
}
