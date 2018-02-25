#pragma once

#ifndef __AFXWIN_H__
#error "Include 'StdAfx.h' before including this file for PCH"
#endif

#include "res\resource.h"
#include "logger\Log.h"
#include "configuration\Configuration.h"
#include "utilities\Utilities.h"

#ifdef _M_X64
#define DIRECTORY_CONFIG _T("EncWAVtoAC3-x64")
#else
#define DIRECTORY_CONFIG _T("EncWAVtoAC3-x86")
#endif

#define FILENAME_LOG _T("EncWAVtoAC3.log")
#define FILENAME_ENGINES _T("EncWAVtoAC3.engines")
#define FILENAME_LANG _T("EncWAVtoAC3.lang")
#define FILENAME_PORTABLE _T("EncWAVtoAC3.portable")
#define FILENAME_CONFIG _T("EncWAVtoAC3.config")
#define FILENAME_PRESETS _T("EncWAVtoAC3.presets")
#define FILENAME_FILES _T("EncWAVtoAC3.files")

namespace app
{
    class CMainApp : public CWinAppEx
    {
    public:
        CMainApp();
        virtual ~CMainApp();
    public:
        virtual BOOL InitInstance();
        DECLARE_MESSAGE_MAP()
    public:
        config::CConfiguration m_Config;
    public:
        void OpenLog();
        void CloseLog();
        void DefaultConfig();
        void LoadLang();
        void SaveLang();
    };

    extern CMainApp m_App;
}
