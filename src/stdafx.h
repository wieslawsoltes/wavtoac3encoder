//
// WAV to AC3 Encoder
// Copyright (C) 2007, 2008, 2009 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

// standard MFC includes and defines
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// select proper manifest for each build
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#ifndef _UNICODE
#include <direct.h>
#endif

#include <shlwapi.h>

// include libaften common headers
#pragma warning(disable:4005)
#include "common.h"
#include "aften.h"
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

// default text for auto value
#define DEFAULT_TEXT_AUTO _T("<Auto>")

// default text for ignored value
#define DEFAULT_TEXT_IGNORED _T("<Ignored>")

// default text for same as source file output path value
#define DEFAULT_TEXT_OUTPUT_PATH _T("<Same as input file path>")

// default text for same as source file output path value (multi mono input)
#define DEFAULT_TEXT_OUTPUT_FILE _T("<Same as first input file path + output.ac3>")

// define default file name for program configuration
#define DEFAULT_CONFIG_FILE_NAME _T("EncWAVtoAC3.config")

// define default file name for encoder presets
#define DEFAULT_PRESETS_FILE_NAME _T("EncWAVtoAC3.presets")

// define default file name for files list
#define DEFAULT_FILES_FILE_NAME _T("EncWAVtoAC3.files")

// define default files list format (0 = *.files, 1 = *.mux)
#define DEFAULT_FILES_FORMAT 0

// define default file name for log
#define DEFAULT_LOG_FILE_NAME _T("EncWAVtoAC3.log")

// define default file name for engines
#define DEFAULT_ENGINES_FILE_NAME _T("EncWAVtoAC3.engines")

// define program home url
#define ENCWAVTOAC3_URL_HOME _T("http://code.google.com/p/wavtoac3encoder/")

// define program version
#define ENCWAVTOAC3_VERSION _T("4.3")
