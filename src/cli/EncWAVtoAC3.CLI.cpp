#include "stdafx.h"

#define FILENAME_PRESETS std::wstring(L"EncWAVtoAC3.presets")
#define FILENAME_ENGINES std::wstring(L"EncWAVtoAC3.engines")
#define FILENAME_FILES std::wstring(L"EncWAVtoAC3.files")

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
        if (IsValid())
        {
            // TODO:
        }
    }
    void SetTotalTimerInfo(std::wstring szInfo)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
public:
    void SetCurrentProgressRange(int nMin, int nMax)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
    void SetTotalProgressRange(int nMin, int nMax)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
    void SetCurrentProgress(int nPos)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
    void SetTotalProgress(int nPos)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
public:
    void StartCurrentTimer(int nResolution)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
    void StopCurrentTimer()
    {
        if (IsValid())
        {
            // TODO:
        }
    }
    void StartTotalTimer(int nResolution)
    {
        if (IsValid())
        {
            // TODO:
        }
    }
    void StopTotalTimer()
    {
        if (IsValid())
        {
            // TODO:
        }
    }
public:
    void Close()
    {
        // TODO:
    }
};

class App
{
public:
    config::CConfiguration m_Config;
private:
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
        return false;
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
};

void Help(std::unique_ptr<logger::ILog>& log)
{
    log->Log(L"EncWAVtoAC3.CLI [options]");
    log->Log(L"Command-line options:");
    log->Log(L"-p <input.presets>           Input presets.");
    log->Log(L"-e <input.engines>           Input engines.");
    log->Log(L"-f <input.files|input.mux>   Input files or mux.");
    log->Log(L"-m [0|1]                     Multi-mono input.");
    log->Log(L"-o <path|file.ac3>           Output path or output file name.");
}

int wmain(int argc, wchar_t *argv[])
{
    App app;
    app.OpenLog();

    if (argc != 1 && argc != 2 && argc != 3 && argc != 5 && argc != 7 && argc != 9 && argc != 11)
    {
        app.m_Config.Log->Log(L"[Error] Invalid command-line options.");
        Help(app.m_Config.Log);
        app.CloseLog();
        return -1;
    }

    app.DefaultConfig();

    app.m_Config.szLogPath = L"";
    app.m_Config.szConfigPath = L"";
    app.m_Config.szLangPath = L"";

    app.m_Config.szPresetsPath = util::Utilities::GetExeFilePath() + FILENAME_PRESETS;
    app.m_Config.szEnginesPath = util::Utilities::GetExeFilePath() + FILENAME_ENGINES;
    app.m_Config.szFilesPath = util::Utilities::GetExeFilePath() + FILENAME_FILES;

    std::vector<std::pair<std::wstring, std::wstring>> m_Options;

    if (argc == 2)
    {
        std::wstring opt = argv[1];
        if (opt == L"-h" || opt == L"--help")
        {
            Help(app.m_Config.Log);
            app.CloseLog();
        }
        return -1;
    }
    else if (argc >= 3)
        m_Options.emplace_back(std::make_pair(argv[1], argv[2]));
    else if (argc >= 5)
        m_Options.emplace_back(std::make_pair(argv[3], argv[4]));
    else if (argc >= 7)
        m_Options.emplace_back(std::make_pair(argv[5], argv[6]));
    else if (argc >= 9)
        m_Options.emplace_back(std::make_pair(argv[7], argv[8]));
    else if (argc >= 11)
        m_Options.emplace_back(std::make_pair(argv[9], argv[10]));

    for (auto& opt : m_Options)
    {
        if (opt.first == L"-p")
        {
            app.m_Config.szPresetsPath = opt.second;
        }
        else if (opt.first == L"-e")
        {
            app.m_Config.szEnginesPath = opt.second;
        }
        else if (opt.first == L"-f")
        {
            app.m_Config.szFilesPath = opt.second;
        }
        else if (opt.first == L"-m")
        {
            app.m_Config.bMultiMonoInput = std::stoi(opt.second) == 1;
        }
        else if (opt.first == L"-o")
        {
            app.m_Config.szOutputPath = opt.second;
            app.m_Config.bUseOutputPath = true;
        }
    }

    if (app.LoadPresets(app.m_Config.szPresetsPath))
    {
        app.m_Config.Log->Log(L"[Info] Loaded presets: " + app.m_Config.szPresetsPath);
    }
    else
    {
        app.DefaultEngines();
        app.m_Config.Log->Log(L"[Info] Using default presets.");
    }

    if (app.LoadEngines(app.m_Config.szEnginesPath))
    {
        app.m_Config.Log->Log(L"[Info] Loaded engines: " + app.m_Config.szEnginesPath);
    }
    else
    {
        app.DefaultEngines();
        app.m_Config.Log->Log(L"[Info] Using default presets.");
    }

    if (app.LoadFiles(app.m_Config.szFilesPath))
    {
        app.m_Config.Log->Log(L"[Info] Loaded files: " + app.m_Config.szFilesPath);
    }   
    else
    {
        app.m_Config.Log->Log(L"[Error] Failed to load files: " + app.m_Config.szFilesPath);
        return -1;
    }

    int nItemsCount = app.m_Config.m_Files.size();
    if (nItemsCount <= 0)
    {
        app.m_Config.Log->Log(L"[Error] Add at least one file to the file list.");
        return -1;
    }

    if ((app.m_Config.bMultiMonoInput == true) && (nItemsCount < 1 || nItemsCount > 6))
    {
        app.m_Config.Log->Log(L"[Error] Supported are minimum 1 and maximum 6 mono input files.");
        return -1;
    }

    if (app.m_Config.m_bIsPortable == true)
        ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());

    std::unique_ptr<worker::CWorkerContext> pContext = std::make_unique<CConsoleWorkerContext>(app.m_Config.Log);

    pContext->bTerminate = false;
    pContext->bCanUpdateWindow = true;
    pContext->nEncodedFiles = 0;
    pContext->m_ElapsedTimeFile = 0;
    pContext->m_ElapsedTimeTotal = 0;
    pContext->nTotalSize = 0;

    for (int i = 0; i < nItemsCount; i++)
    {
        config::CFile& file = app.m_Config.m_Files[i];
        std::wstring szExt = util::Utilities::GetFileExtension(file.szPath);
        if (util::StringHelper::TowLower(szExt) == L"avs")
        {
            if (app.m_Config.bMultiMonoInput == true)
            {
                app.m_Config.Log->Log(L"[Error] Disable 'Multiple mono input' mode in order to use Avisynth scripts.");
                return -1;
            }
        }
        file.bStatus = false;
        pContext->nTotalSize += file.nSize;
    }

    util::CTimeCount countTime;
    countTime.Start();

    std::thread m_Thread = std::thread([&app, &pContext]()
    {
        try
        {
            worker::CWorker m_Worker(pContext);
            m_Worker.Run(&app.m_Config);
        }
        catch (...)
        {
            app.m_Config.Log->Log(L"[Error] Unknown exception thrown while encoding.");
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
        app.m_Config.Log->Log(L"[Error] Failed to encode all files.");
        app.CloseLog();
        return -1;
    }
    else
    {
        if (app.m_Config.bMultiMonoInput == true)
        {
            app.m_Config.Log->Log(
                L"[Info] Encoded " + std::to_wstring(pContext->nEncodedFiles) +
                L" mono files in " + szElapsedFormatted + L" (" + std::to_wstring(szElapsedSeconds) + L"s).");
        }
        else
        {
            app.m_Config.Log->Log(
                L"[Info] Encoded " + std::to_wstring(pContext->nEncodedFiles) +
                L" file" + ((pContext->nEncodedFiles == 1) ? L"" : L"s") +
                L" in " + szElapsedFormatted + L" (" + std::to_wstring(szElapsedSeconds) + L"s).");
        }

        app.CloseLog();
        return 0;
    }
}
