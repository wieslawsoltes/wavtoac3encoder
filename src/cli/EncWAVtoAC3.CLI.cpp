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
    log->Log(L"options:");
    log->Log(L"--presets <input.presets>           Input presets file.");
    log->Log(L"--preset,-p <index>                 Set current preset, default: 0.");
    log->Log(L"--engines <input.engines>           Input engines file.");
    log->Log(L"--engine,-e <index>                 Set current engine, default: 0.");
    log->Log(L"--files <input.files|input.mux>     Input files or mux file.");
    log->Log(L"--mono,-m                           Set multi-mono input files flag.");
    log->Log(L"--input,-i <filename,...>           Input file names (one or more).");
    log->Log(L"--output,-o <path|file.ac3>         Output path or output file name.");
    log->Log(L"--help,-h                           Show command-line help.");
}

int wmain(int argc, wchar_t *argv[])
{
    App app;
    app.OpenLog();

    app.DefaultConfig();
    app.m_Config.szLogPath = L"";
    app.m_Config.szConfigPath = L"";
    app.m_Config.szLangPath = L"";
    app.m_Config.szPresetsPath = util::Utilities::GetExeFilePath() + FILENAME_PRESETS;
    app.m_Config.szEnginesPath = util::Utilities::GetExeFilePath() + FILENAME_ENGINES;
    app.m_Config.szFilesPath = util::Utilities::GetExeFilePath() + FILENAME_FILES;

    enum OptionId : int
    {
        OptionPresets,
        OptionPreset,
        OptionEngines,
        OptionEngine,
        OptionFiles,
        OptionMono,
        OptionInput,
        OptionOutput,
        OptionHelp
    };

    std::vector<util::Option> options
    {
        { OptionId::OptionPresets, { L"--presets"       }, 1  },
        { OptionId::OptionPreset,  { L"--preset", L"-p" }, 1  },
        { OptionId::OptionEngines, { L"--engines"       }, 1  },
        { OptionId::OptionEngine,  { L"--engine", L"-e" }, 1  },
        { OptionId::OptionFiles,   { L"--files"         }, 1  },
        { OptionId::OptionMono,    { L"--mono", L"-m"   }, 0  },
        { OptionId::OptionInput,   { L"--input", L"-i"  }, -1 },
        { OptionId::OptionOutput,  { L"--output", L"-o" }, 1  },
        { OptionId::OptionHelp,    { L"--help", L"-h"   }, 0  }
    };

    std::vector<util::Result> results;
    util::ArgvParser parser { false };
    if (parser.ParseOptions(argc, argv, options, results) != 0)
    {
        app.m_Config.Log->Log(L"[Error] Invalid command-line options.");
        Help(app.m_Config.Log);
        app.CloseLog();
        return -1;
    }

    for (auto& result : results)
    {
        switch (result.Id)
        {
        case OptionId::OptionPresets:
            app.m_Config.szPresetsPath = result.Params[0];
            break;
        case OptionId::OptionPreset:
            app.m_Config.nCurrentPreset = std::stoi(result.Params[0]);
            break;
        case OptionId::OptionEngines:
            app.m_Config.szEnginesPath = result.Params[0];
            break;
        case OptionId::OptionEngine:
            app.m_Config.nCurrentEngine = std::stoi(result.Params[0]);
            break;
        case OptionId::OptionFiles:
            app.m_Config.szFilesPath = result.Params[0];
            break;
        case OptionId::OptionMono:
            app.m_Config.bMultiMonoInput = true;
            break;
        case OptionId::OptionInput:
            for (auto& param : result.Params)
            {
                if (app.AddFile(param) == false)
                {
                    app.m_Config.Log->Log(L"[Error] Not supported input file: " + param);
                    app.CloseLog();
                    return -1;
                }
            }
            break;
        case OptionId::OptionOutput:
            app.m_Config.szOutputPath = result.Params[0];
            app.m_Config.bUseOutputPath = true;
            break;
        case OptionId::OptionHelp:
            Help(app.m_Config.Log);
            app.CloseLog();
            return 0;
            break;
        default:
            app.m_Config.Log->Log(L"[Error] Unknown option.");
            Help(app.m_Config.Log);
            app.CloseLog();
            return -1;
            break;
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
