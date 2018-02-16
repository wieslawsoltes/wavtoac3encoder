#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"

namespace app
{
    CEncWAVtoAC3App m_App;

    BEGIN_MESSAGE_MAP(CEncWAVtoAC3App, CWinAppEx)
        ON_COMMAND(ID_HELP, &CWinAppEx::OnHelp)
    END_MESSAGE_MAP()

    CEncWAVtoAC3App::CEncWAVtoAC3App()
    {
        log::Log = std::make_unique<log::FileLog>();
        log::Log->Open(FILENAME_LOG);
    }

    CEncWAVtoAC3App::~CEncWAVtoAC3App()
    {
        log::Log->Close();
    }

    BOOL CEncWAVtoAC3App::InitInstance()
    {
        try
        {
            LoadDefaults();
            LoadConfig();
        }
        catch (...)
        {
            log::Log->Log(L"Error: Failed to load config.");
        }

        try
        {
            INITCOMMONCONTROLSEX InitCtrls;
            InitCtrls.dwSize = sizeof(InitCtrls);
            InitCtrls.dwICC = ICC_WIN95_CLASSES;
            InitCommonControlsEx(&InitCtrls);
    
            CWinAppEx::InitInstance();
            AfxEnableControlContainer();
            InitShellManager();
        }
        catch (...)
        {
            log::Log->Log(L"Error: Failed to init application.");
        }

        try
        {
            CMainDlg dlg;
            m_pMainWnd = &dlg;
            dlg.pConfig = &this->m_Config;
            dlg.DoModal();
        }
        catch (...)
        {
            log::Log->Log(L"Error: Main dialog exception.");
        }

        try
        {
            SaveConfig();
        }
        catch (...)
        {
            log::Log->Log(L"Error: Failed to save config.");
        }

        return FALSE;
    }

    void CEncWAVtoAC3App::LoadDefaults()
    {
        this->m_Config.m_szLangFileName = L"lang\\en-US.txt";
        this->m_Config.m_nLangId = -1;
        this->m_Config.m_bIsPortable = true;
        this->m_Config.nCurrentPreset = 0;
        this->m_Config.szOutputPath = L"";
        this->m_Config.szOutputFile = L"";
        this->m_Config.bMultipleMonoInput = false;
        this->m_Config.bDisableAllWarnings = false;
        this->m_Config.bSaveConfig = true;
        this->m_Config.nCurrentEngine = 0;
#if defined(_WIN32) & !defined(_WIN64)
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86", L"libaftendll_x86\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86 (SSE)", L"libaftendll_x86_SSE\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86 (SSE2)", L"libaftendll_x86_SSE2\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x86 (SSE3)", L"libaftendll_x86_SSE3\\libaften.dll"));
#else
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x64", L"libaftendll_AMD64\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x64 (SSE2)", L"libaftendll_AMD64_SSE2\\libaften.dll"));
        this->m_Config.m_Engines.emplace_back(config::CEngine(L"Aften x64 (SSE3)", L"libaftendll_AMD64_SSE3\\libaften.dll"));
#endif
    }

    void CEncWAVtoAC3App::LoadConfig()
    {
        this->m_Config.SetEncoderOptions();

        this->m_Config.m_bIsPortable = PathFileExists((util::Utilities::GetExeFilePath() + FILENAME_PORTABLE).c_str()) == TRUE ? true : false;

        if (this->m_Config.m_bIsPortable == true)
        {
            this->m_Config.m_szPresetsFilePath = util::Utilities::GetExeFilePath() + FILENAME_PRESETS;
            this->m_Config.m_szConfigFilePath = util::Utilities::GetExeFilePath() + FILENAME_CONFIG;
            this->m_Config.m_szEnginesFilePath = util::Utilities::GetExeFilePath() + FILENAME_ENGINES;
            this->m_Config.m_szFilesListFilePath = util::Utilities::GetExeFilePath() + FILENAME_FILES;
            this->m_Config.m_szLangFilePath = util::Utilities::GetExeFilePath() + FILENAME_LANG;
        }
        else
        {
            ::CreateDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str(), nullptr);

            this->m_Config.m_szPresetsFilePath = util::Utilities::GetSettingsFilePath(FILENAME_PRESETS, DIRECTORY_CONFIG);
            this->m_Config.m_szConfigFilePath = util::Utilities::GetSettingsFilePath(FILENAME_CONFIG, DIRECTORY_CONFIG);
            this->m_Config.m_szEnginesFilePath = util::Utilities::GetSettingsFilePath(FILENAME_ENGINES, DIRECTORY_CONFIG);
            this->m_Config.m_szFilesListFilePath = util::Utilities::GetSettingsFilePath(FILENAME_FILES, DIRECTORY_CONFIG);
            this->m_Config.m_szLangFilePath = util::Utilities::GetSettingsFilePath(FILENAME_LANG, DIRECTORY_CONFIG);
        }

        if (this->m_Config.m_bIsPortable == true)
        {
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        }
        else
        {
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());
        }

        this->m_Config.LoadLanguagePath(this->m_Config.m_szLangFilePath);

        if (this->m_Config.m_bIsPortable == true)
        {
            this->m_Config.LoadLLanguages(util::Utilities::GetExeFilePath() + L"lang");
        }
        else
        {
            this->m_Config.LoadLLanguages(util::Utilities::GetSettingsFilePath(L"", std::wstring(DIRECTORY_CONFIG) + L"\\lang"));
        }
    }

    void CEncWAVtoAC3App::SaveConfig()
    {
        this->m_Config.SaveLanguagePath(this->m_Config.m_szLangFilePath);
    }
}
