//
// WAV to AC3 Encoder
// Copyright (C) 2007,2008,2009,2010 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#include "MyLog.h"

extern LogCtx logCtx;

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

        this->bEnableLog = false;
        this->szLogFile = _T("");
        this->bHaveLogFile = false;
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
public:
    bool bEnableLog;
    CString szLogFile;
    bool bHaveLogFile;
} CommandLineOpt;

class CEncWAVtoAC3App : public CWinApp
{
public:
    CEncWAVtoAC3App();
public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CEncWAVtoAC3App theApp;
