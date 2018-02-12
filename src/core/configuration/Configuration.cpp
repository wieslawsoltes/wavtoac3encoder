#pragma once

#include "StdAfx.h"
#include "utilities\StringHelper.h"
#include "utilities\Utilities.h"
#include "Configuration.h"

namespace config
{
    CConfiguration m_Config;

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

    bool CConfiguration::LoadConfig(std::wstring &szFileName, CConfigList &cl)
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
                    cl.Insert(std::make_pair(parts[0], parts[1]));
                }
            }

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CConfiguration::SaveConfig(std::wstring &szFileName, CConfigList &cl)
    {
        int nSize = cl.Count();
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer;
            for (int i = 0; i < nSize; i++)
            {
                auto& ce = cl.Get(i);
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

    bool CConfiguration::LoadFiles(std::wstring &szFileName, util::CListT<std::wstring>& fl)
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
                    if (CEncoderDefaults::IsSupportedInputExt(util::Utilities::GetFileExtension(szBuffer)) == true)
                    {
                        std::wstring szPath = szBuffer;
                        fl.Insert(szPath);
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

    bool CConfiguration::SaveFiles(std::wstring &szFileName, util::CListT<std::wstring>& fl, int nFormat)
    {
        int nItems = fl.Count();
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer;

            for (int i = 0; i < nItems; i++)
            {
                std::wstring &szPath = fl.Get(i);
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

    bool CConfiguration::SearchFolderForLang(std::wstring szPath, const bool bRecurse, std::vector<CLang>& m_LangLst)
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
                        CLang lang;
                        if (this->LoadLang(file, lang.m_Strings) == true)
                        {
                            lang.szFileName = file;

                            if (lang.m_Strings.count(0x00000001) == 1)
                                lang.szEnglishName = lang.m_Strings[0x00000001];
                            else
                                lang.szEnglishName = L"??";

                            if (lang.m_Strings.count(0x00000002) == 1)
                                lang.szTargetName = lang.m_Strings[0x00000002];
                            else
                                lang.szTargetName = L"??";

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
                    m_bHaveLang = TRUE;
                    pStrings = &lang.m_Strings;
                    haveLang = true;
                    break;
                }
            }

            if (haveLang == false)
            {
                auto& lang = m_LangLst[0];
                m_nLangId = 0;
                m_bHaveLang = TRUE;
                pStrings = &lang.m_Strings;
                m_szLangFileName = lang.szFileName;
            }
        }
        else
        {
            m_nLangId = -1;
            m_bHaveLang = FALSE;
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

    int CEncoderDefaults::nValidCbrBitrates[nNumValidCbrBitrates] {
        0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640
    };

    CChannelConfig CEncoderDefaults::ccAften[nNumChannelConfigAften] = {
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

    std::wstring CEncoderDefaults::szRawSampleFormats[nNumRawSampleFormats] = {
        config::m_Config.GetString(0x00207003),
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

    std::wstring CEncoderDefaults::pszGroups[nNumEncoderOptionsGroups] = {
        L"Encoding options",
        L"Bitstream info metadata",
        L"Dynamic range compression and dialog normalization",
        L"Input options",
        L"Input filters",
        L"Alternate bit stream syntax"
    };

    std::wstring CEncoderDefaults::szCbrOption = L"-b";

    std::wstring CEncoderDefaults::szVbrOption = L"-q";

    std::wstring CEncoderDefaults::szThreadsOption = L"-threads";

    std::wstring CEncoderDefaults::szSimdOption = L"-nosimd";

    std::wstring CEncoderDefaults::szRawSampleFormatOption = L"-raw_fmt";

    std::wstring CEncoderDefaults::szRawSampleRateOption = L"-raw_sr";

    std::wstring CEncoderDefaults::szRawChannelsOption = L"-raw_ch";

    std::wstring CEncoderDefaults::szSupportedInputExt[nNumSupportedInputExt] = {
        L"wav",
        L"pcm",
        L"raw",
        L"bin",
        L"aiff",
        L"aif",
        L"aifc",
        L"avs"
    };

    int CEncoderDefaults::nSupportedInputFormats[nNumSupportedInputExt] = {
        PCM_FORMAT_WAVE,
        PCM_FORMAT_RAW,
        PCM_FORMAT_RAW,
        PCM_FORMAT_RAW,
        PCM_FORMAT_AIFF,
        PCM_FORMAT_AIFF,
        PCM_FORMAT_CAFF,
    };

    std::wstring CEncoderDefaults::szSupportedOutputExt[nNumSupportedOutputExt] = {
       L"ac3"
    };

    CEncoderOption CEncoderDefaults::encOpt[CEncoderPreset::nNumEncoderOptions];

    void CEncoderDefaults::InitEncoderOptions()
    {
        #define GetString(key) config::m_Config.GetString(key)
        #define SetOption(name, option, tip, dval, ival, group, begin) \
            nCurOpt++; \
            encOpt[nCurOpt].szName = name; \
            encOpt[nCurOpt].szOption = option; \
            encOpt[nCurOpt].szHelpText = tip; \
            encOpt[nCurOpt].nDefaultValue = dval; \
            encOpt[nCurOpt].nIgnoreValue = ival; \
            encOpt[nCurOpt].szGroupName = group; \
            encOpt[nCurOpt].bBeginGroup = begin;
        #define AddValue(name, value) \
            encOpt[nCurOpt].m_Names.Insert(name); \
            encOpt[nCurOpt].m_Values.Insert(value);
        #define AddValueRange(start, end) \
            for (int i = start; i <= end; i++) { \
                encOpt[nCurOpt].m_Names.Insert(std::to_wstring(i)); \
                encOpt[nCurOpt].m_Values.Insert(i); \
            }

        int nCurOpt = -1;

        for (int i = 0; i < CEncoderPreset::nNumEncoderOptions; i++)
        {
            encOpt[i].m_Names.RemoveAll();
            encOpt[i].m_Values.RemoveAll();
        }

        SetOption(GetString(0x00301001), L"-fba", GetString(0x00301002), 0, -1, GetString(0x00208001), true)
        AddValue(GetString(0x00301003), 0)
        AddValue(GetString(0x00301004), 1)

        SetOption(GetString(0x00302001), L"-exps", GetString(0x00302002), 7, -1, L"", false)
        AddValue(GetString(0x00302003), 1)
        AddValueRange(2, 7)
        AddValue(GetString(0x00302004), 8)
        AddValueRange(9, 31)
        AddValue(GetString(0x00302005), 32)

        SetOption(GetString(0x00303001), L"-pad", GetString(0x00303002), 1, -1, L"", false)
        AddValue(GetString(0x00303003), 0)
        AddValue(GetString(0x00303004), 1)

        SetOption(GetString(0x00304001), L"-w", GetString(0x00304002), 0, -1, L"", false)
        AddValue(GetString(0x00304003), -1)
        AddValue(GetString(0x00304004), -2)
        AddValue(GetString(0x00304005), 0)
        AddValueRange(1, 59)
        AddValue(GetString(0x00304006), 60)

        SetOption(GetString(0x00305001), L"-wmin", GetString(0x00305002), 0, -1, L"", false)
        AddValue(GetString(0x00305003), 0)
        AddValueRange(1, 59)
        AddValue(L"60", 60)

        SetOption(GetString(0x00306001), L"-wmax", GetString(0x00306002), 60, -1, L"", false)
        AddValue(L"0", 0)
        AddValueRange(1, 56)
        AddValue(GetString(0x00306003), 60)

        SetOption(GetString(0x00307001), L"-m", GetString(0x00307002), 1, -1, L"", false)
        AddValue(GetString(0x00307003), 0)
        AddValue(GetString(0x00307004), 1)

        SetOption(GetString(0x00308001), L"-s", GetString(0x00308002), 0, -1, L"", false)
        AddValue(GetString(0x00308003), 0)
        AddValue(GetString(0x00308004), 1)

        SetOption(GetString(0x00401001), L"-cmix", GetString(0x00401002), 0, -1, GetString(0x00208002), true)
        AddValue(GetString(0x00401003), 0)
        AddValue(L"-4.5 dB", 1)
        AddValue(L"-6.0 dB", 2)

        SetOption(GetString(0x00402001), L"-smix", GetString(0x00402002), 0, -1, L"", false)
        AddValue(GetString(0x00402003), 0)
        AddValue(L"-6 dB", 1)
        AddValue(L"0", 2)

        SetOption(GetString(0x00403001) , L"-dsur", GetString(0x00403002), 0, -1, L"", false)
        AddValue(GetString(0x00403003), 0)
        AddValue(GetString(0x00403004), 1)
        AddValue(GetString(0x00403005), 2)

        SetOption(GetString(0x00501001), L"-dnorm", GetString(0x00501002), 31, -1, GetString(0x00208003), true)
        AddValueRange(0, 30)
        AddValue(GetString(0x00501003), 31)

        SetOption(GetString(0x00502001), L"-dynrng", GetString(0x00502002), 5, -1, L"", false)
        AddValue(GetString(0x00502003), 0)
        AddValue(GetString(0x00502004), 1)
        AddValue(GetString(0x00502005), 2)
        AddValue(GetString(0x00502006), 3)
        AddValue(GetString(0x00502007), 4)
        AddValue(GetString(0x00502008), 5)

        SetOption(GetString(0x00601001), L"-acmod", GetString(0x00601002), 8, 8, GetString(0x00208004), true)
        AddValue(L"1+1 (Ch1,Ch2)", 0)
        AddValue(L"1/0 (C)", 1)
        AddValue(L"2/0 (L,R)", 2)
        AddValue(L"3/0 (L,R,C)", 3)
        AddValue(L"2/1 (L,R,S)", 4)
        AddValue(L"3/1 (L,R,C,S)", 5)
        AddValue(L"2/2 (L,R,SL,SR)", 6)
        AddValue(L"3/2 (L,R,C,SL,SR)", 7)
        AddValue(GetString(0x00207003), 0)

        SetOption(GetString(0x00602001), L"-lfe", GetString(0x00602002), 2, 2, L"", false)
        AddValue(GetString(0x00602003), 0)
        AddValue(GetString(0x00602004), 1)
        AddValue(GetString(0x00207003), 0)

        SetOption(GetString(0x00603001), L"-chconfig", GetString(0x00603002), 16, 16, L"", false)
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

        SetOption(GetString(0x00604001), L"-chmap", GetString(0x00604002), 0, -1, L"", false)
        AddValue(GetString(0x00604003), 0)
        AddValue(GetString(0x00604004), 1)
        AddValue(GetString(0x00604005), 2)

        SetOption(GetString(0x00605001), L"-readtoeof", GetString(0x00605002), 0, -1, L"", false)
        AddValue(GetString(0x00605003), 0)
        AddValue(GetString(0x00605004), 1)

        SetOption(GetString(0x00701001), L"-bwfilter", GetString(0x00701002), 0, -1, GetString(0x00208005), true)
        AddValue(GetString(0x00701003), 0)
        AddValue(GetString(0x00701004), 1)

        SetOption(GetString(0x00702001), L"-dcfilter", GetString(0x00702002), 0, -1, L"", false)
        AddValue(GetString(0x00702003), 0)
        AddValue(GetString(0x00702004), 1)

        SetOption(GetString(0x00703001), L"-lfefilter", GetString(0x00703002), 0, -1, L"", false)
        AddValue(GetString(0x00703003), 0)
        AddValue(GetString(0x00703004), 1)

        SetOption(GetString(0x00801001), L"-xbsi1", GetString(0x00801002), 2, 2, GetString(0x00208006), true)
        AddValue(GetString(0x00801003), 0)
        AddValue(GetString(0x00801004), 1)
        AddValue(GetString(0x00207003), 0)

        SetOption(GetString(0x00802001), L"-dmixmod", GetString(0x00802002), 0, -1, L"", false)
        AddValue(GetString(0x00802003), 0)
        AddValue(GetString(0x00802004), 1)
        AddValue(GetString(0x00802005), 2)

        SetOption(GetString(0x00803001), L"-ltrtcmix", GetString(0x00803002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00803003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        SetOption(GetString(0x00804001), L"-ltrtsmix", GetString(0x00804002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00804003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        SetOption(GetString(0x00805001), L"-lorocmix", GetString(0x00805002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00805003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        SetOption(GetString(0x00806001), L"-lorosmix", GetString(0x00806002), 4, -1, L"", false)
        AddValue(L"+3.0 dB", 0)
        AddValue(L"+1.5 dB", 1)
        AddValue(L"0.0 dB", 2)
        AddValue(L"-1.5 dB", 3)
        AddValue(GetString(0x00806003), 4)
        AddValue(L"-4.5 dB", 5)
        AddValue(L"-6.0 dB", 6)
        AddValue(L"-inf dB", 7)

        SetOption(GetString(0x00807001), L"-xbsi2", GetString(0x00807002), 2, 2, L"", false)
        AddValue(GetString(0x00807003), 0)
        AddValue(GetString(0x00807004), 1)
        AddValue(GetString(0x00207003), 0)

        SetOption(GetString(0x00808001), L"-dsurexmod", GetString(0x00808002), 0, -1, L"", false)
        AddValue(GetString(0x00808003), 0)
        AddValue(GetString(0x00808004), 1)
        AddValue(GetString(0x00808005), 2)

        SetOption(GetString(0x00809001), L"-dheadphon", GetString(0x00809002), 0, -1, L"", false)
        AddValue(GetString(0x00809003), 0)
        AddValue(GetString(0x00809004), 1)
        AddValue(GetString(0x00809005), 2)

        SetOption(GetString(0x0080A001), L"-adconvtyp", GetString(0x0080A002), 0, -1, L"", false)
        AddValue(GetString(0x0080A003), 0)
        AddValue(GetString(0x0080A004), 1)

        #undef GetString
        #undef SetOption
        #undef AddValue
    }

    int CEncoderDefaults::FindValidBitratePos(const int nBitrate)
    {
        for (int i = 0; i < nNumValidCbrBitrates; i++)
        {
            if (nValidCbrBitrates[i] == nBitrate)
                return i;
        }
        return 0;
    }

    int CEncoderDefaults::FindOptionIndex(std::wstring szOption)
    {
        for (int i = 0; i < CEncoderPreset::nNumEncoderOptions; i++)
        {
            std::wstring szBuffer = encOpt[i].szOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (szOption == szBuffer)
            {
                return i;
            }
        }
        return 0;
    }

    void CEncoderDefaults::ParsePreset(CEncoderPreset &preset, CConfigList &cl)
    {
        for (int i = 0; i < cl.Count(); i++)
        {
            std::wstring szBuffer;
            auto& ce = cl.Get(i);

            if (ce.first == L"engine")
            {
                preset.nCurrentEngine = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = szThreadsOption;
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

            szBuffer = szCbrOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nBitrate = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = szVbrOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nQuality = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = szRawSampleFormatOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nRawSampleFormat = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = szRawSampleRateOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nRawSampleRate = util::StringHelper::ToInt(ce.second);
                continue;
            }

            szBuffer = szRawChannelsOption;
            util::StringHelper::TrimLeft(szBuffer, '-');
            if (ce.first == szBuffer)
            {
                preset.nRawChannels = util::StringHelper::ToInt(ce.second);
                continue;
            }

            for (int i = 0; i < CEncoderPreset::nNumEncoderOptions; i++)
            {
                szBuffer = encOpt[i].szOption;
                util::StringHelper::TrimLeft(szBuffer, '-');
                if (ce.first == szBuffer)
                {
                    preset.nSetting[i] = util::StringHelper::ToInt(ce.second);
                    break;
                }
            }
        }
    }

    bool CEncoderDefaults::LoadPresets(CEncoderPresetList& presets, std::wstring& szFileName, CEncoderPreset& defaultPreset)
    {
        try
        {
            std::wstring data = ReadAllText(szFileName);
            if (data.empty())
                return false;

            CEncoderPreset temp;
            CConfigList cl;
            bool bHavePreset = false;
            presets.RemoveAll();

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
                        presets.Insert(preset);
                        cl.RemoveAll();
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
                        cl.Insert(std::make_pair(parts[0], parts[1]));
                    }
                }
            }

            if (bHavePreset == true)
            {
                ParsePreset(temp, cl);
                auto preset = temp;
                presets.Insert(preset);
                cl.RemoveAll();
            }

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool CEncoderDefaults::SavePresets(CEncoderPresetList& presets, std::wstring& szFileName, CEncoderPreset& defaultPreset)
    {
        const int nSize = (const int)presets.Count();
        try
        {
            FILE *fs;
            errno_t error = _wfopen_s(&fs, szFileName.c_str(), szWriteMode.c_str());
            if (error != 0)
                return false;

            std::wstring szBuffer;
            std::wstring szTmpBuffer;

            for (int i = 0; i < nSize; i++)
            {
                auto& preset = presets.Get(i);

                szBuffer = L"[" + preset.szName + L"]\n";
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szBuffer = L"engine" + szSeparator + std::to_wstring(preset.nCurrentEngine) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = szThreadsOption;
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

                szTmpBuffer = szCbrOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nBitrate) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = szVbrOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nQuality) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = szRawSampleFormatOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nRawSampleFormat) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = szRawSampleRateOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nRawSampleRate) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                szTmpBuffer = szRawChannelsOption;
                util::StringHelper::TrimLeft(szTmpBuffer, '-');
                szBuffer = szTmpBuffer + szSeparator + std::to_wstring(preset.nRawChannels) + szNewChar;
                std::fwrite(szBuffer.data(), sizeof(wchar_t), szBuffer.size(), fs);

                for (int j = 0; j < CEncoderPreset::nNumEncoderOptions; j++)
                {
                    szTmpBuffer = encOpt[j].szOption;
                    util::StringHelper::TrimLeft(szTmpBuffer, '-');
                    szBuffer = szTmpBuffer + szSeparator+ std::to_wstring(preset.nSetting[j]) + szNewChar;
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

    bool CEncoderDefaults::IsSupportedInputExt(std::wstring &szExt)
    {
        for (int i = 0; i < nNumSupportedInputExt; i++)
        {
            if (util::StringHelper::CompareNoCase(szExt, szSupportedInputExt[i]))
                return true;
        }
        return false;
    }

    int CEncoderDefaults::GetSupportedInputFormat(std::wstring &szExt)
    {
        for (int i = 0; i < nNumSupportedInputExt; i++)
        {
            if (util::StringHelper::CompareNoCase(szExt, szSupportedInputExt[i]))
            {
                return nSupportedInputFormats[i];
            }
        }
        return PCM_FORMAT_UNKNOWN;
    }

    CAtlString CEncoderDefaults::GetSupportedInputFilesFilter()
    {
        CAtlString szFilter = L"";
        CAtlString szExtL = L"";
        CAtlString szExtU = L"";
        CAtlString szBuff = L"";

        for (int i = 0; i < nNumSupportedInputExt; i++)
        {
            szExtL = szSupportedInputExt[i].c_str();
            szBuff = L"*." + szExtL.MakeLower();
            szBuff += (i < nNumSupportedInputExt - 1) ? L";" : L"";
            szFilter += szBuff;
        }

        szFilter = CAtlString(config::m_Config.GetString(0x00207006).c_str()) + L" (" + szFilter + L")|" + szFilter + L"|";

        for (int i = 0; i < nNumSupportedInputExt; i++)
        {
            szExtL = szExtU = szSupportedInputExt[i].c_str();
            szExtU.MakeUpper();
            szExtL.MakeLower();
            szBuff.Format(L"%s %s (*.%s)|*.%s|", szExtU, config::m_Config.GetString(0x00207007).c_str(), szExtL, szExtL);
            szFilter += szBuff;
        }

        szFilter += CAtlString(config::m_Config.GetString(0x00207008).c_str()) + L" (*.*)|*.*||";
        return szFilter;
    }
}
