#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"
#include "utilities\Utilities.h"

namespace app
{
    CEncWAVtoAC3App m_App;
    config::CConfiguration m_Config;

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
        config::CEncoderDefaults::InitEncoderOptions();

        m_Config.m_bIsPortable = PathFileExists((util::Utilities::GetExeFilePath() + FILENAME_PORTABLE).c_str()) == TRUE ? true : false;

        if (m_Config.m_bIsPortable == true)
        {
            m_Config.m_szPresetsFilePath = util::Utilities::GetExeFilePath() + FILENAME_PRESETS;
            m_Config.m_szConfigFilePath = util::Utilities::GetExeFilePath() + FILENAME_CONFIG;
            m_Config.m_szEnginesFilePath = util::Utilities::GetExeFilePath() + FILENAME_ENGINES;
            m_Config.m_szFilesListFilePath = util::Utilities::GetExeFilePath() + FILENAME_FILES;
            m_Config.m_szLangFilePath = util::Utilities::GetExeFilePath() + FILENAME_LANG;
        }
        else
        {
            ::CreateDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str(), nullptr);

            m_Config.m_szPresetsFilePath = util::Utilities::GetSettingsFilePath(FILENAME_PRESETS, DIRECTORY_CONFIG);
            m_Config.m_szConfigFilePath = util::Utilities::GetSettingsFilePath(FILENAME_CONFIG, DIRECTORY_CONFIG);
            m_Config.m_szEnginesFilePath = util::Utilities::GetSettingsFilePath(FILENAME_ENGINES, DIRECTORY_CONFIG);
            m_Config.m_szFilesListFilePath = util::Utilities::GetSettingsFilePath(FILENAME_FILES, DIRECTORY_CONFIG);
            m_Config.m_szLangFilePath = util::Utilities::GetSettingsFilePath(FILENAME_LANG, DIRECTORY_CONFIG);
        }

        if (m_Config.m_bIsPortable == true)
        {
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        }
        else
        {
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());
        }

        m_Config.LoadLangConfig(m_Config.m_szLangFilePath);
        m_Config.LoadLangStrings();
    }

    void CEncWAVtoAC3App::SaveConfig()
    {
        m_Config.SaveLangConfig(m_Config.m_szLangFilePath);
    }
}
