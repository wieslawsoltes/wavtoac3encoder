#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"
#include "utilities\Utilities.h"

CEncWAVtoAC3App theApp;

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
    CEncoderDefaults::InitEncoderOptions();

    m_Config.m_bIsPortable = PathFileExists(GetExeFilePath() + FILENAME_PORTABLE) == TRUE ? true : false;

    if (m_Config.m_bIsPortable == true)
    {
        m_Config.m_szPresetsFilePath = GetExeFilePath() + FILENAME_PRESETS;
        m_Config.m_szConfigFilePath = GetExeFilePath() + FILENAME_CONFIG;
        m_Config.m_szEnginesFilePath = GetExeFilePath() + FILENAME_ENGINES;
        m_Config.m_szFilesListFilePath = GetExeFilePath() + FILENAME_FILES;
        m_Config.m_szLangFilePath = GetExeFilePath() + FILENAME_LANG;
    }
    else
    {
        CreateDirectory(GetSettingsFilePath(_T(""), DIRECTORY_CONFIG), nullptr);

        m_Config.m_szPresetsFilePath = GetSettingsFilePath(FILENAME_PRESETS, DIRECTORY_CONFIG);
        m_Config.m_szConfigFilePath = GetSettingsFilePath(FILENAME_CONFIG, DIRECTORY_CONFIG);
        m_Config.m_szEnginesFilePath = GetSettingsFilePath(FILENAME_ENGINES, DIRECTORY_CONFIG);
        m_Config.m_szFilesListFilePath = GetSettingsFilePath(FILENAME_FILES, DIRECTORY_CONFIG);
        m_Config.m_szLangFilePath = GetSettingsFilePath(FILENAME_LANG, DIRECTORY_CONFIG);
    }

    if (m_Config.m_bIsPortable == true)
    {
        ::SetCurrentDirectory(GetExeFilePath());
    }
    else
    {
        ::SetCurrentDirectory(GetSettingsFilePath(_T(""), DIRECTORY_CONFIG));
    }

    m_Config.LoadLangConfig(m_Config.m_szLangFilePath);
    m_Config.LoadLangStrings();
}

void CEncWAVtoAC3App::SaveConfig()
{
    m_Config.SaveLangConfig(m_Config.m_szLangFilePath);
}
