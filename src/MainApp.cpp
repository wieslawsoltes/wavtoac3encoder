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
    m_Config.m_bIsPortable = PathFileExists(GetExeFilePath() + DEFAULT_PORTABLE_FILE_NAME) == TRUE ? true : false;

    if (m_Config.m_bIsPortable == true)
    {
        m_Config.m_szPresetsFilePath = GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME;
        m_Config.m_szConfigFilePath = GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME;
        m_Config.m_szEnginesFilePath = GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME;
        m_Config.m_szFilesListFilePath = GetExeFilePath() + DEFAULT_FILES_FILE_NAME;
        m_Config.m_szLangFilePath = GetExeFilePath() + DEFAULT_LANG_FILE_NAME;
    }
    else
    {
        CreateDirectory(GetSettingsFilePath(_T(""), DEFAULT_CONFIG_DIRECTORY), NULL);

        m_Config.m_szPresetsFilePath = GetSettingsFilePath(DEFAULT_PRESETS_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_Config.m_szConfigFilePath = GetSettingsFilePath(DEFAULT_CONFIG_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_Config.m_szEnginesFilePath = GetSettingsFilePath(DEFAULT_ENGINES_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_Config.m_szFilesListFilePath = GetSettingsFilePath(DEFAULT_FILES_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_Config.m_szLangFilePath = GetSettingsFilePath(DEFAULT_LANG_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
    }

    m_Config.LoadLangConfig(m_Config.m_szLangFilePath);
    m_Config.LoadLangStrings();

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

    m_Config.SaveLangConfig(m_Config.m_szLangFilePath);

    return FALSE;
}
