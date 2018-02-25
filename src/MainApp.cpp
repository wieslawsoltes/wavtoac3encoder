#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"

namespace app
{
    CMainApp m_App;

    BEGIN_MESSAGE_MAP(CMainApp, CWinAppEx)
        ON_COMMAND(ID_HELP, &CWinAppEx::OnHelp)
    END_MESSAGE_MAP()

    CMainApp::CMainApp()
    {
    }

    CMainApp::~CMainApp()
    {
    }

    BOOL CMainApp::InitInstance()
    {
        this->m_Config.m_bIsPortable = PathFileExists((util::Utilities::GetExeFilePath() + FILENAME_PORTABLE).c_str()) == TRUE ? true : false;

        if (this->m_Config.m_bIsPortable == true)
        {
            this->m_Config.szLogPath = util::Utilities::GetExeFilePath() + FILENAME_LOG;
            this->m_Config.szPresetsPath = util::Utilities::GetExeFilePath() + FILENAME_PRESETS;
            this->m_Config.szConfigPath = util::Utilities::GetExeFilePath() + FILENAME_CONFIG;
            this->m_Config.szEnginesPath = util::Utilities::GetExeFilePath() + FILENAME_ENGINES;
            this->m_Config.szFilesPath = util::Utilities::GetExeFilePath() + FILENAME_FILES;
            this->m_Config.szLangPath = util::Utilities::GetExeFilePath() + FILENAME_LANG;
        }
        else
        {
            ::CreateDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str(), nullptr);
            this->m_Config.szLogPath = util::Utilities::GetSettingsFilePath(FILENAME_LOG, DIRECTORY_CONFIG);
            this->m_Config.szPresetsPath = util::Utilities::GetSettingsFilePath(FILENAME_PRESETS, DIRECTORY_CONFIG);
            this->m_Config.szConfigPath = util::Utilities::GetSettingsFilePath(FILENAME_CONFIG, DIRECTORY_CONFIG);
            this->m_Config.szEnginesPath = util::Utilities::GetSettingsFilePath(FILENAME_ENGINES, DIRECTORY_CONFIG);
            this->m_Config.szFilesPath = util::Utilities::GetSettingsFilePath(FILENAME_FILES, DIRECTORY_CONFIG);
            this->m_Config.szLangPath = util::Utilities::GetSettingsFilePath(FILENAME_LANG, DIRECTORY_CONFIG);
        }

        if (this->m_Config.m_bIsPortable == true)
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        else
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());

        this->m_Config.Log = std::make_unique<logger::FileLog>(this->m_Config.szLogPath);
        this->m_Config.Log->Open();

        std::wstring szConfigMode = this->m_Config.m_bIsPortable ? L"Portable" : L"Roaming";
        this->m_Config.Log->Log(L"[Info] Program started: " + szConfigMode);

        try
        {
            this->m_Config.m_szLangFileName = L"lang\\en-US.txt";
            this->m_Config.m_nLangId = -1;
            this->m_Config.m_bIsPortable = true;
            this->m_Config.nCurrentPreset = 0;
            this->m_Config.szOutputPath = L"";
            this->m_Config.szOutputFile = L"";
            this->m_Config.bMultiMonoInput = false;
            this->m_Config.bDisableAllWarnings = false;
            this->m_Config.bSaveConfig = true;
            this->m_Config.nCurrentEngine = 0;

            this->m_Config.SetEncoderOptions();

            this->m_Config.LoadLanguagePath(this->m_Config.szLangPath);
            if (this->m_Config.m_bIsPortable == true)
                this->m_Config.LoadLanguages(util::Utilities::GetExeFilePath() + L"lang");
            else
                this->m_Config.LoadLanguages(util::Utilities::GetSettingsFilePath(L"", std::wstring(DIRECTORY_CONFIG) + L"\\lang"));
        }
        catch (...)
        {
            this->m_Config.Log->Log(L"[Error] Failed to load config.");
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
            this->m_Config.Log->Log(L"[Error] Failed to init application.");
        }

        this->m_Config.InitDefaultPreset();
        this->m_Config.InitDefaultEngine();
        this->m_Config.DefaultPresets();
        this->m_Config.DefaultEngines();

        try
        {
            dialogs::CMainDlg dlg;
            m_pMainWnd = &dlg;
            dlg.pConfig = &this->m_Config;
            dlg.DoModal();
        }
        catch (...)
        {
            this->m_Config.Log->Log(L"[Error] Main dialog exception.");
        }

        try
        {
            this->m_Config.SaveLanguagePath(this->m_Config.szLangPath);
        }
        catch (...)
        {
            this->m_Config.Log->Log(L"[Error] Failed to save config.");
        }

        this->m_Config.Log->Log(L"[Info] Program exited: " + szConfigMode);
        this->m_Config.Log->Close();

        return FALSE;
    }
}
