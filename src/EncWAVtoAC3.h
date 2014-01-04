//
// WAV to AC3 Encoder
// Copyright (C) 2007-2013 Wiesław Šoltés <wieslaw.soltes@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/* $Id$ */

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

#include "Language.h"

void ShowCommandLineHelp(HWND hWnd);

typedef class TCommandLineOpt
{
public:
    TCommandLineOpt()
    {
        this->bHelp = false;

        this->bEncodeAndExit = false;

        this->bSaveConfig = false;
        this->bHaveSaveConfig = false;

        this->szInputFile = _T("");
        this->bHaveInputFile = false;

        this->szOutputFile = _T("");
        this->bHaveOutputFile = false;

        this->szOutputPath = _T("");
        this->bHaveOutputPath = false;

        this->szLoadConfig = _T("");
        this->bHaveLoadConfig = false;

        this->szLoadPresets = _T("");
        this->bHaveLoadFiles = false;

        this->szLoadFiles = _T("");
        this->bHaveLoadPresets = false;

        this->szLoadEngines = _T("");
        this->bHaveLoadEngines = false;
    }
public:
    bool bHelp;
public:
    bool bEncodeAndExit;
public:
    bool bSaveConfig;
    bool bHaveSaveConfig;
public:
    CString szInputFile;
    bool bHaveInputFile;
public:
    CString szOutputFile;
    bool bHaveOutputFile;
public:
    CString szOutputPath;
    bool bHaveOutputPath;
public:
    CString szLoadConfig;
    bool bHaveLoadConfig;
public:
    CString szLoadFiles;
    bool bHaveLoadFiles;
public:
    CString szLoadPresets;
    bool bHaveLoadPresets;
public:
    CString szLoadEngines;
    bool bHaveLoadEngines;
} CommandLineOpt;

class CEncWAVtoAC3App : public CWinAppEx
{
public:
    CEncWAVtoAC3App();
	virtual ~CEncWAVtoAC3App();
public:
    virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
public:
	void InitLog();
public:
	LangMap_t *m_Lang;
	LangList_t m_LangLst;
	BOOL m_bHaveLang = FALSE;
	int m_nLangId = -1;
public:
	bool m_bIsPortable = true;
	CString m_szPresetsFilePath;
	CString m_szConfigFilePath;
	CString m_szEnginesFilePath;
	CString m_szFilesListFilePath;
	CString m_szLangFilePath;
	CString m_szLogFilePath;
};

extern CEncWAVtoAC3App theApp;
extern CString langFileName;

bool LoadLangConfig(CString &szFileName);
bool SaveLangConfig(CString &szFileName);

extern void LoadLangStrings();
extern BOOL HaveLangStrings();
extern CString& GetLangString(int id);
