﻿#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"

namespace app
{
    CMainApp m_App;

    BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
    END_MESSAGE_MAP()

    CMainApp::CMainApp()
    {
    }

    CMainApp::~CMainApp()
    {
    }

    void CMainApp::OpenLog()
    {
        this->m_Config.Log = std::make_unique<util::FileLog>(this->m_Config.szLogPath);
        this->m_Config.Log->Open();

        std::wstring szConfigMode = this->m_Config.m_bIsPortable ? L"Portable" : L"Roaming";
        this->m_Config.Log->Log(L"[Info] Program started: " + szConfigMode);
    }

    void CMainApp::CloseLog()
    {
        std::wstring szConfigMode = this->m_Config.m_bIsPortable ? L"Portable" : L"Roaming";
        this->m_Config.Log->Log(L"[Info] Program exited: " + szConfigMode);
        this->m_Config.Log->Close();
    }

    void CMainApp::DefaultConfig()
    {
        this->m_Config.m_bIsPortable = util::PathFileExists_((util::GetExeFilePath() + FILENAME_PORTABLE));

        this->m_Config.m_szLangFileName = L"lang\\en-US.txt";
        this->m_Config.m_nLangId = -1;

        this->m_Config.nCurrentPreset = 0;
        this->m_Config.szOutputPath = L"";
        this->m_Config.szOutputFile = L"";
        this->m_Config.bMultiMonoInput = false;
        this->m_Config.bDisableAllWarnings = false;
        this->m_Config.bSaveConfig = true;
        this->m_Config.nCurrentEngine = 0;

        this->m_Config.SetEncoderOptions();

        if (this->m_Config.m_bIsPortable == true)
        {
            this->m_Config.szLogPath = util::GetExeFilePath() + FILENAME_LOG;
            this->m_Config.szPresetsPath = util::GetExeFilePath() + FILENAME_PRESETS;
            this->m_Config.szConfigPath = util::GetExeFilePath() + FILENAME_CONFIG;
            this->m_Config.szEnginesPath = util::GetExeFilePath() + FILENAME_ENGINES;
            this->m_Config.szFilesPath = util::GetExeFilePath() + FILENAME_FILES;
            this->m_Config.szLangPath = util::GetExeFilePath() + FILENAME_LANG;
        }
        else
        {
            util::CreateDirectory_(util::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG));
            this->m_Config.szLogPath = util::GetSettingsFilePath(FILENAME_LOG, DIRECTORY_CONFIG);
            this->m_Config.szPresetsPath = util::GetSettingsFilePath(FILENAME_PRESETS, DIRECTORY_CONFIG);
            this->m_Config.szConfigPath = util::GetSettingsFilePath(FILENAME_CONFIG, DIRECTORY_CONFIG);
            this->m_Config.szEnginesPath = util::GetSettingsFilePath(FILENAME_ENGINES, DIRECTORY_CONFIG);
            this->m_Config.szFilesPath = util::GetSettingsFilePath(FILENAME_FILES, DIRECTORY_CONFIG);
            this->m_Config.szLangPath = util::GetSettingsFilePath(FILENAME_LANG, DIRECTORY_CONFIG);
        }

        if (this->m_Config.m_bIsPortable == true)
            util::SetCurrentDirectory_(util::GetExeFilePath());
        else
            util::SetCurrentDirectory_(util::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG));
    }

    void CMainApp::LoadLang()
    {
        if (this->m_Config.LoadLanguagePath(this->m_Config.szLangPath))
            this->m_Config.Log->Log(L"[Info] Loaded language config: " + this->m_Config.szLangPath);
        else
            this->m_Config.Log->Log(L"[Error] Failed to load language config: " + this->m_Config.szLangPath);

        std::wstring szLangPath = (this->m_Config.m_bIsPortable == true) ?
            (util::GetExeFilePath() + L"lang") :
            (util::GetSettingsFilePath(L"", std::wstring(DIRECTORY_CONFIG) + L"\\lang"));

        if (this->m_Config.LoadLanguages(szLangPath))
            this->m_Config.Log->Log(L"[Info] Loaded languages from: " + szLangPath);
        else
            this->m_Config.Log->Log(L"[Error] Failed to load languages from: " + szLangPath);
    }

    void CMainApp::SaveLang()
    {
        if (this->m_Config.SaveLanguagePath(this->m_Config.szLangPath))
            this->m_Config.Log->Log(L"[Info] Saved language config: " + this->m_Config.szLangPath);
        else
            this->m_Config.Log->Log(L"[Error] Failed to save language config: " + this->m_Config.szLangPath);
    }

    BOOL CMainApp::InitInstance()
    {
        this->DefaultConfig();
        this->OpenLog();
        this->LoadLang();

        try
        {
            INITCOMMONCONTROLSEX InitCtrls;
            InitCtrls.dwSize = sizeof(InitCtrls);
            InitCtrls.dwICC = ICC_WIN95_CLASSES;
            InitCommonControlsEx(&InitCtrls);

            CWinApp::InitInstance();
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

        this->SaveLang();
        this->CloseLog();
        return FALSE;
    }
}
