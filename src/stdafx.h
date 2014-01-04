//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

//
// DEFAULTS
//

// define program version
#define ENCWAVTOAC3_VERSION _T("5.2")

// default preset name
#define DEFAULT_PRESET_NAME (HaveLangStrings() ? GetLangString(0x00207001) : _T("Default"))

// default text for auto value
#define DEFAULT_TEXT_AUTO (HaveLangStrings() ? GetLangString(0x00207002) : _T("<Auto>"))

// default text for ignored value
#define DEFAULT_TEXT_IGNORED (HaveLangStrings() ? GetLangString(0x00207003) : _T("<Ignored>"))

// default text for same as source file output path value
#define DEFAULT_TEXT_OUTPUT_PATH (HaveLangStrings() ? GetLangString(0x00207004) : _T("<Same as input file path>"))

// default text for same as source file output path value (multi mono input)
#define DEFAULT_TEXT_OUTPUT_FILE (HaveLangStrings() ? GetLangString(0x00207005) : _T("<Same as first input file path + output.ac3>"))

// default directory for roaming data
#define DEFAULT_CONFIG_DIRECTORY _T("EncWAVtoAC3")

// default directory for language files
#ifdef _DEBUG
#define DEFAULT_LANG_DIRECTORY _T("..\\..\\Lang");
#else
#define DEFAULT_LANG_DIRECTORY  _T("Lang");
#endif

// default file name used to enable portable application
#define DEFAULT_PORTABLE_FILE_NAME _T("EncWAVtoAC3.portable")

// default file name for program configuration
#define DEFAULT_CONFIG_FILE_NAME _T("EncWAVtoAC3.config")

// default file name for language configuration
#define DEFAULT_LANG_FILE_NAME _T("EncWAVtoAC3.lang")

// default file name for encoder presets
#define DEFAULT_PRESETS_FILE_NAME _T("EncWAVtoAC3.presets")

// default file name for files list
#define DEFAULT_FILES_FILE_NAME _T("EncWAVtoAC3.files")

// default files list format (0 = *.files, 1 = *.mux)
#define DEFAULT_FILES_FORMAT 0

// default file name for log
#define DEFAULT_LOG_FILE_NAME _T("EncWAVtoAC3.txt")

// default file name for engines
#define DEFAULT_ENGINES_FILE_NAME _T("EncWAVtoAC3.engines")

// program home URL
#define ENCWAVTOAC3_URL_HOME _T("http://code.google.com/p/wavtoac3encoder/")

//
// MFC
//

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//
// AFTEN
//

#ifndef _UNICODE
#include <direct.h>
#endif
#include <shlwapi.h>

// include libaften common headers
#pragma warning(disable:4005)
#include "common.h"
#include "libaften/aften.h"
#pragma warning(default:4005)

// include Aften API wrapper headers
#include "AftenAPI.h"

// include aften wav reader headers
#ifdef __cplusplus
extern "C" {
#endif
#include "pcm.h"
#if defined(__cplusplus)
}
#endif

//
// LOG
//

typedef struct TLogCtx
{
    bool bInit;
    CString szLogFileName;
    CFile fp;
} LogCtx;

void LogDefaults();
void LogFile(CString szLogFileName);
bool LogOpen();
bool LogClose();
bool LogMessage(CString szMessage);
void LogAnsiMessage(const PTCHAR pszMessage, const char* msg);
