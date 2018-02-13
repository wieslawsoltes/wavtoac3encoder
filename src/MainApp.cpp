#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"
#include "utilities\Utilities.h"
#include "configuration\Configuration.h"

namespace app
{
    CEncWAVtoAC3App m_App;

    BEGIN_MESSAGE_MAP(CEncWAVtoAC3App, CWinAppEx)
        ON_COMMAND(ID_HELP, &CWinAppEx::OnHelp)
    END_MESSAGE_MAP()

    CEncWAVtoAC3App::CEncWAVtoAC3App()
    {
    }

    CEncWAVtoAC3App::~CEncWAVtoAC3App()
    {
    }

    BOOL CEncWAVtoAC3App::InitInstance()
    {
        LoadConfig();

        INITCOMMONCONTROLSEX InitCtrls;
        InitCtrls.dwSize = sizeof(InitCtrls);
        InitCtrls.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&InitCtrls);

        CWinAppEx::InitInstance();
        AfxEnableControlContainer();
        InitShellManager();

        CMainDlg dlg;
        m_pMainWnd = &dlg;
        dlg.DoModal();

        SaveConfig();
        return FALSE;
    }

    void CEncWAVtoAC3App::LoadConfig()
    {
        config::m_Config.m_EncoderOptions.Init();

        config::m_Config.m_bIsPortable = PathFileExists((util::Utilities::GetExeFilePath() + FILENAME_PORTABLE).c_str()) == TRUE ? true : false;

        if (config::m_Config.m_bIsPortable == true)
        {
            config::m_Config.m_szPresetsFilePath = util::Utilities::GetExeFilePath() + FILENAME_PRESETS;
            config::m_Config.m_szConfigFilePath = util::Utilities::GetExeFilePath() + FILENAME_CONFIG;
            config::m_Config.m_szEnginesFilePath = util::Utilities::GetExeFilePath() + FILENAME_ENGINES;
            config::m_Config.m_szFilesListFilePath = util::Utilities::GetExeFilePath() + FILENAME_FILES;
            config::m_Config.m_szLangFilePath = util::Utilities::GetExeFilePath() + FILENAME_LANG;
        }
        else
        {
            ::CreateDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str(), nullptr);

            config::m_Config.m_szPresetsFilePath = util::Utilities::GetSettingsFilePath(FILENAME_PRESETS, DIRECTORY_CONFIG);
            config::m_Config.m_szConfigFilePath = util::Utilities::GetSettingsFilePath(FILENAME_CONFIG, DIRECTORY_CONFIG);
            config::m_Config.m_szEnginesFilePath = util::Utilities::GetSettingsFilePath(FILENAME_ENGINES, DIRECTORY_CONFIG);
            config::m_Config.m_szFilesListFilePath = util::Utilities::GetSettingsFilePath(FILENAME_FILES, DIRECTORY_CONFIG);
            config::m_Config.m_szLangFilePath = util::Utilities::GetSettingsFilePath(FILENAME_LANG, DIRECTORY_CONFIG);
        }

        if (config::m_Config.m_bIsPortable == true)
        {
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        }
        else
        {
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());
        }

        config::m_Config.LoadLangConfig(config::m_Config.m_szLangFilePath);

        if (config::m_Config.m_bIsPortable == true)
        {
            config::m_Config.LoadLangStrings(util::Utilities::GetExeFilePath() + L"lang");
        }
        else
        {
            config::m_Config.LoadLangStrings(util::Utilities::GetSettingsFilePath(L"", std::wstring(DIRECTORY_CONFIG) + L"\\lang"));
        }
    }

    void CEncWAVtoAC3App::SaveConfig()
    {
        config::m_Config.SaveLangConfig(config::m_Config.m_szLangFilePath);
    }
}
