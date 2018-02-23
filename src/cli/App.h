#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <thread>
#include "logger\Log.h"
#include "utilities\StringHelper.h"
#include "utilities\TimeCount.h"
#include "utilities\Utilities.h"
#include "avs\src\Avs2Raw\Avs2Raw.h"
#include "configuration\Configuration.h"
#include "worker\Worker.h"

class CConsoleWorkerContext : public worker::CWorkerContext
{
private:
    std::unique_ptr<logger::ILog>& pLog;
public:
    CConsoleWorkerContext(std::unique_ptr<logger::ILog>& pLog) : pLog(pLog) { }
    virtual ~CConsoleWorkerContext() { }
private:
    bool IsValid()
    {
        return (this->bTerminate == false);
    }
public:
    void SetTitleInfo(std::wstring szInfo)
    {
        if (IsValid())
        {
            ::SetConsoleTitle(szInfo.c_str());
        }
    }
    void SetInputFileInfo(int nID, std::wstring szInfo)
    {
        if (IsValid())
        {
            pLog->Log(L"[Info] " + szInfo);
        }
    }
    void SetInputTypeInfo(int nID, std::wstring szInfo)
    {
        if (IsValid())
        {
            pLog->Log(L"[Info] " + szInfo);
        }
    }
    void SetOutputFileInfo(std::wstring szInfo)
    {
        if (IsValid())
        {
            pLog->Log(L"[Info] " + szInfo);
        }
    }
    void SetOutputTypeInfo(std::wstring szInfo)
    {
        if (IsValid())
        {
            pLog->Log(L"[Info] " + szInfo);
        }
    }
    void SetCurrentTimerInfo(std::wstring szInfo)
    {
    }
    void SetTotalTimerInfo(std::wstring szInfo)
    {
    }
public:
    void SetCurrentProgressRange(int nMin, int nMax)
    {
    }
    void SetTotalProgressRange(int nMin, int nMax)
    {
    }
    void SetCurrentProgress(int nPos)
    {
        pLog->Log(L"\r[Info] Progress: " + std::to_wstring(nPos) + L"%", false);
        if (nPos == 100)
            pLog->Log(L"\n[Info] Done.", true);
    }
    void SetTotalProgress(int nPos)
    {
    }
public:
    void StartCurrentTimer(int nResolution)
    {
    }
    void StopCurrentTimer()
    {
    }
    void StartTotalTimer(int nResolution)
    {
    }
    void StopTotalTimer()
    {
    }
public:
    void Close()
    {
    }
};

class App
{
public:
    config::CConfiguration m_Config;
public:
    bool GetAvisynthFileInfo(std::wstring szFileName, AvsAudioInfo *pInfoAVS)
    {
        if (pInfoAVS == nullptr)
            return false;

        memset(pInfoAVS, 0, sizeof(AvsAudioInfo));

        CAvs2Raw decoderAVS;
        std::string szInputFileAVS = util::StringHelper::Convert(szFileName);
        if (decoderAVS.OpenAvisynth(szInputFileAVS.c_str()) == false)
        {
            this->m_Config.Log->Log(L"[Error] Failed to initialize Avisynth.");
            return false;
        }
        else
        {
            (*pInfoAVS) = decoderAVS.GetInputInfo();
            decoderAVS.CloseAvisynth();
            return true;
        }
    }
    ULONGLONG GetFileSize(const std::wstring& szPath)
    {
        std::wstring szExt = util::Utilities::GetFileExtension(szPath);
        if (util::StringHelper::TowLower(szExt) == L"avs")
        {
            AvsAudioInfo infoAVS;
            memset(&infoAVS, 0, sizeof(AvsAudioInfo));
            if (GetAvisynthFileInfo(szPath, &infoAVS) == true)
            {
                ULONGLONG nFileSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
                return nFileSize;
            }
        }
        else
        {
            ULONGLONG nFileSize = util::Utilities::GetFileSize64(szPath);
            return nFileSize;
        }
        return 0;
    }
    bool AddFile(const std::wstring& szPath)
    {
        std::wstring szExt = util::Utilities::GetFileExtension(szPath);
        if (this->m_Config.IsSupportedInputExt(szExt) == true)
        {
            ULONGLONG nSize = this->GetFileSize(szPath);
            config::CFile file{ szPath, nSize };
            this->m_Config.m_Files.emplace_back(file);
            return true;
        }
        this->m_Config.Log->Log(L"[Error] Not supported input file: " + szPath);
        return false;
    }
    bool FindFiles(const std::wstring pattern, std::vector<std::wstring>& files)
    {
        try
        {
            WIN32_FIND_DATA w32FileData;
            ZeroMemory(&w32FileData, sizeof(WIN32_FIND_DATA));
            HANDLE hSearch = FindFirstFile(pattern.c_str(), &w32FileData);
            if (hSearch == INVALID_HANDLE_VALUE)
                return false;

            BOOL fFinished = FALSE;
            while (fFinished == FALSE)
            {
                if (w32FileData.cFileName[0] != '.' && !(w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    std::wstring szFileName = w32FileData.cFileName;
                    std::wstring szFilePath = util::Utilities::GetFullPathName(szFileName);
                    files.push_back(szFilePath);
                }
                if (FindNextFile(hSearch, &w32FileData) == FALSE)
                {
                    if (GetLastError() == ERROR_NO_MORE_FILES)
                        fFinished = TRUE;
                    else
                        return false;
                }
            }
            if (FindClose(hSearch) == FALSE)
                return false;
        }
        catch (...)
        {
            return false;
        }
        return true;
    }
    bool AddFiles(const std::vector<std::wstring>& files)
    {
        for (auto& file : files)
        {
            std::vector<std::wstring> findFiles;
            if (this->FindFiles(file, findFiles) == true)
            {
                for (auto& findFile : findFiles)
                    this->AddFile(findFile);
            }
            else
            {
                if (this->AddFile(file) == false)
                    return false;
            }
        }
        return true;
    }
public:
    void OpenLog()
    {
        this->m_Config.Log = std::make_unique<logger::ConsoleLog>();
        this->m_Config.Log->Open();
        this->m_Config.Log->Log(L"[Info] Program started: CLI");
    }
    void CloseLog()
    {
        this->m_Config.Log->Log(L"[Info] Program exited: CLI");
        this->m_Config.Log->Close();
    }
public:
    void DefaultConfig()
    {
        this->m_Config.m_bIsPortable = true;
        this->m_Config.nCurrentPreset = 0;
        this->m_Config.szOutputPath = L"";
        this->m_Config.szOutputFile = L"";
        this->m_Config.bUseOutputPath = false;
        this->m_Config.bMultiMonoInput = false;
        this->m_Config.bDisableAllWarnings = true;
        this->m_Config.bSaveConfig = false;
        this->m_Config.nCurrentEngine = 0;
        this->m_Config.SetEncoderOptions();
        this->m_Config.InitDefaultPreset();
        this->m_Config.InitDefaultEngine();
        this->m_Config.m_Presets.emplace_back(this->m_Config.m_DefaultPreset);
        this->m_Config.m_Engines.emplace_back(this->m_Config.m_DefaultEngine);
        this->m_Config.szLogPath = L"";
        this->m_Config.szConfigPath = L"";
        this->m_Config.szLangPath = L"";
        this->m_Config.szPresetsPath = L"";
        this->m_Config.szEnginesPath = L"";
        this->m_Config.szFilesPath = L"";
    }
    void DefaultPresets()
    {
        this->m_Config.m_Presets.clear();
        this->m_Config.m_Presets.emplace_back(this->m_Config.m_DefaultPreset);
        this->m_Config.nCurrentPreset = 0;
    }
    void DefaultEngines()
    {
        this->m_Config.m_Engines.clear();
#if defined(_WIN32) & !defined(_WIN64)
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86", L"libaftendll_x86\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86 (SSE)", L"libaftendll_x86_SSE\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86 (SSE2)", L"libaftendll_x86_SSE2\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86 (SSE3)", L"libaftendll_x86_SSE3\\libaften.dll"));
#else
        this->m_Config.m_Engines.emplace_back(this->m_Config.m_DefaultEngine);
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x64", L"libaftendll_AMD64\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x64 (SSE2)", L"libaftendll_AMD64_SSE2\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x64 (SSE3)", L"libaftendll_AMD64_SSE3\\libaften.dll"));
#endif
        this->m_Config.nCurrentEngine = 0;
    }
public:
    bool LoadPresets(const std::wstring &szFileName)
    {
        std::vector<config::CPreset> presets;
        if (this->m_Config.LoadPresets(presets, szFileName, this->m_Config.m_DefaultPreset))
        {
            this->m_Config.nCurrentPreset = 0;
            this->m_Config.m_Presets = presets;
            return true;
        }
        return false;
    }
    bool LoadEngines(const std::wstring &szFileName)
    {
        std::vector<config::CEngine> engines;
        if (this->m_Config.LoadEngines(engines, szFileName) == true)
        {
            this->m_Config.nCurrentEngine = 0;
            this->m_Config.m_Engines = engines;
            return true;
        }
        return false;
    }
    bool LoadFiles(const std::wstring &szFileName)
    {
        std::vector<std::wstring> fl;
        if (this->m_Config.LoadFiles(szFileName, fl))
        {
            this->m_Config.m_Files.clear();
            for (int i = 0; i < (int)fl.size(); i++)
                this->AddFile(fl[i]);
            return true;
        }
        return false;
    }
public:
    bool Init()
    {
        if (!this->m_Config.szPresetsPath.empty() && this->LoadPresets(this->m_Config.szPresetsPath))
        {
            this->m_Config.Log->Log(L"[Info] Loaded presets: " + this->m_Config.szPresetsPath);
        }
        else
        {
            this->DefaultPresets();
            this->m_Config.Log->Log(L"[Info] Using default presets.");
        }

        if (!this->m_Config.szEnginesPath.empty() && this->LoadEngines(this->m_Config.szEnginesPath))
        {
            this->m_Config.Log->Log(L"[Info] Loaded engines: " + this->m_Config.szEnginesPath);
        }
        else
        {
            this->DefaultEngines();
            this->m_Config.Log->Log(L"[Info] Using default engines.");
        }

        if (!this->m_Config.szFilesPath.empty())
        {
            if (this->LoadFiles(this->m_Config.szFilesPath))
            {
                this->m_Config.Log->Log(L"[Info] Loaded files: " + this->m_Config.szFilesPath);
            }
            else
            {
                this->m_Config.Log->Log(L"[Error] Failed to load files: " + this->m_Config.szFilesPath);
                return false;
            }
        }

        int nItemsCount = this->m_Config.m_Files.size();
        if (nItemsCount <= 0)
        {
            this->m_Config.Log->Log(L"[Error] Add at least one file to the file list.");
            return false;
        }

        if ((this->m_Config.bMultiMonoInput == true) && (nItemsCount < 1 || nItemsCount > 6))
        {
            this->m_Config.Log->Log(L"[Error] Supported are minimum 1 and maximum 6 mono input files.");
            return false;
        }

        if (this->m_Config.m_bIsPortable == true)
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());

        return true;
    }
    bool Encode()
    {
        std::unique_ptr<worker::CWorkerContext> pContext = std::make_unique<CConsoleWorkerContext>(this->m_Config.Log);

        pContext->bTerminate = false;
        pContext->bCanUpdateWindow = true;
        pContext->nEncodedFiles = 0;
        pContext->m_ElapsedTimeFile = 0;
        pContext->m_ElapsedTimeTotal = 0;
        pContext->nTotalSize = 0;

        int nItemsCount = this->m_Config.m_Files.size();
        for (int i = 0; i < nItemsCount; i++)
        {
            config::CFile& file = this->m_Config.m_Files[i];
            std::wstring szExt = util::Utilities::GetFileExtension(file.szPath);
            if (util::StringHelper::TowLower(szExt) == L"avs")
            {
                if (this->m_Config.bMultiMonoInput == true)
                {
                    this->m_Config.Log->Log(L"[Error] Disable 'Multiple mono input' mode in order to use Avisynth scripts.");
                    return false;
                }
            }
            file.bStatus = false;
            pContext->nTotalSize += file.nSize;
        }

        if (!this->m_Config.szOutputPath.empty())
        {
            if (this->m_Config.bMultiMonoInput == false)
            {
                if (util::Utilities::MakeFullPath(this->m_Config.szOutputPath) == false)
                {
                    this->m_Config.Log->Log(L"[Error] Failed to create output path: " + this->m_Config.szOutputPath);
                    return false;
                }
            }
            else
            {
                std::wstring szFile = util::Utilities::GetFileName(this->m_Config.szOutputPath);
                std::wstring szOutputPath = this->m_Config.szOutputPath.substr(0, this->m_Config.szOutputPath.length() - szFile.length());
                if (util::Utilities::MakeFullPath(szOutputPath) == false)
                {
                    this->m_Config.Log->Log(L"[Error] Failed to create output path: " + szOutputPath);
                    return false;
                }
            }
            this->m_Config.bUseOutputPath = true;
        }

        util::CTimeCount countTime;
        countTime.Start();

        std::thread m_Thread = std::thread([this, &pContext]()
        {
            try
            {
                worker::CWorker m_Worker(pContext);
                m_Worker.Run(&this->m_Config);
            }
            catch (...)
            {
                this->m_Config.Log->Log(L"[Error] Unknown exception thrown while encoding.");
            }
            pContext->bTerminate = true;
            pContext->Close();
        });

        m_Thread.join();
        countTime.Stop();

        std::wstring szElapsedFormatted = countTime.Format(countTime.ElapsedTime());
        double szElapsedSeconds = countTime.ElapsedTime().count() / 1000.0f;

        if (pContext->nEncodedFiles <= 0)
        {
            this->m_Config.Log->Log(L"[Error] Failed to encode all files.");
            return false;
        }
        else
        {
            if (this->m_Config.bMultiMonoInput == true)
            {
                this->m_Config.Log->Log(
                    L"[Info] Encoded " + std::to_wstring(pContext->nEncodedFiles) +
                    L" mono files in " + szElapsedFormatted + L" (" + std::to_wstring(szElapsedSeconds) + L"s).");
            }
            else
            {
                this->m_Config.Log->Log(
                    L"[Info] Encoded " + std::to_wstring(pContext->nEncodedFiles) +
                    L" file" + ((pContext->nEncodedFiles == 1) ? L"" : L"s") +
                    L" in " + szElapsedFormatted + L" (" + std::to_wstring(szElapsedSeconds) + L"s).");
            }
            return true;
        }
    }
};
