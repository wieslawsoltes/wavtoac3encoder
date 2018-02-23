#include "stdafx.h"
#include "App.h"
#include "ArgvParser.h"

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
    log->Log(L"--input,-i <filename,...|*.wav>     Input file names or path with wildcard.");
    log->Log(L"--output,-o <path|file.ac3>         Output path or output file name.");
    log->Log(L"--help,-h                           Show command-line help.");
}

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

std::vector<util::Option> m_Options
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

int wmain(int argc, wchar_t *argv[])
{
    App app;

    app.OpenLog();
    app.DefaultConfig();

    std::vector<util::Result> results;
    util::ArgvParser parser { false };
    if (parser.ParseOptions(argc, argv, m_Options, results) != 0)
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
            if (app.AddFiles(result.Params) == false)
            {
                app.CloseLog();
                return -1;
            }
            break;
        case OptionId::OptionOutput:
            app.m_Config.szOutputPath = result.Params[0];
            break;
        case OptionId::OptionHelp:
            Help(app.m_Config.Log);
            app.CloseLog();
            return 0;
        default:
            app.m_Config.Log->Log(L"[Error] Unknown option.");
            Help(app.m_Config.Log);
            app.CloseLog();
            return -1;
        }
    }

    if (app.Init() == false)
    {
        app.CloseLog();
        return -1;
    }

    if (app.Encode() == false)
    {
        app.CloseLog();
        return -1;
    }

    return 0;
}
