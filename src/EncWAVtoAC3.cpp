//
// WAV to AC3 Encoder
// Copyright (C) 2007-2016 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "EncWAVtoAC3Dlg.h"
#include "EncWAVtoAC3MuxDlg.h"

#include "CommandLine.h"
#include "OptionsParser.h"
#include "Utilities.h"
#include "MyFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// command-line help message
const CString szHelpMessage = 
    _T("WAV to AC3 Encoder Command-Line Help\n\n")
    _T("Usage: EncWAVtoAC3 [Options]\n\n")
    _T("Options:\n\n")
    _T("-h, --help\n")
    _T("Show command-line help message.\n\n")
    _T("-x,--encode-and-exit\n")
	_T("Encode input files and exit.\n\n")
    _T("-r, --read-only\n")
	_T("Do not save configuration on exit.\n\n")
    _T("-i, --input-file <file.ext>\n")
#ifndef DISABLE_AVISYNTH
	_T("Set input files path (*.wav, *.bin, *.pcm, *.raw, *.avs).\n\n")
#else
	_T("Set input files path (*.wav, *.bin, *.pcm, *.raw).\n\n")
#endif
    _T("-o, --output-file <file.ext>\n")
	_T("Set output file path (*.ac3).\n\n")
    _T("-d, --output-path <path>\n")
	_T("Set default output path (default: input file path is used).\n\n")
    _T("-c, --load-config <file.ext>\n")
	_T("Load program configation (*.config).\n\n")
    _T("-f, --load-files <file.ext>\n")
	_T("Load list of files to encode (*.files, *.mux).\n\n")
    _T("-p, --load-presets <file.ext>\n")
	_T("Load list of preconfigured presets (*.presets).\n\n")
    _T("-e, --load-engines <file.ext>\n")
	_T("Load list of Aften engines (*.engines).\n\n")
    _T("-m, --mux-wizard\n")
	_T("Show standalone MUX Wizard window.\n");

// command-line options Id's
enum CL_OPTIONS_CONST
{    
    CLOP_HELP,
    CLOP_ENCODE_AND_EXIT,
    CLOP_READ_ONLY,
    CLOP_INPUT_FILE,
    CLOP_OUTPUT_FILE,
    CLOP_OUTPUT_PATH,
    CLOP_LOAD_CONFIG,
    CLOP_LOAD_FILES,
    CLOP_LOAD_PRESETS,
    CLOP_LOAD_ENGINES,
    CLOP_MUX_WIZARD
};

// command-line options config
const COptionsParser::CL_OPTIONS clOptions[] = 
{
    { _T("h;help"),               CLOP_HELP,                                 0, false },
    { _T("x;encode-and-exit"),    CLOP_ENCODE_AND_EXIT,                      0, false },
    { _T("r;read-only"),          CLOP_READ_ONLY,                            0, false },
    { _T("i;input-file"),         CLOP_INPUT_FILE,                           1, true  },
    { _T("o;output-file"),        CLOP_OUTPUT_FILE,                          1, true  },
    { _T("d;output-path"),        CLOP_OUTPUT_PATH,                          1, true  },
    { _T("c;load-config"),        CLOP_LOAD_CONFIG,                          1, true  },
    { _T("f;load-files"),         CLOP_LOAD_FILES,                           1, true  },
    { _T("p;load-presets"),       CLOP_LOAD_PRESETS,                         1, true  },
    { _T("e;load-engines"),       CLOP_LOAD_ENGINES,                         1, true  },
    { _T("m;mux-wizard"),         CLOP_MUX_WIZARD,                           0, false },
    { NULL,                       0,                                         0, false }
};

void ShowCommandLineError(CString szMessage)
{
    CString szDisplayMessage;

    // format message
    szDisplayMessage.Format(
        _T("%s: %s\n\n%s"),
		HaveLangStrings() ? GetLangString(0x00D01004) : _T("Error"),
		HaveLangStrings() ? GetLangString(0x00D01005) : _T("Use --help option to obtain more informations."),
        szMessage);

	::LogMessage(_T("Command line error: ") + szMessage);

	// stop log
	LogClose();

    // show error message
    MessageBox(NULL, 
        szDisplayMessage, 
		HaveLangStrings() ? GetLangString(0x00D01004) : _T("Error"),
        MB_OK | MB_ICONERROR);
}

void ShowCommandLineHelp(HWND hWnd)
{
	CString szMessage = _T("");

	if (HaveLangStrings())
	{
#ifndef DISABLE_AVISYNTH
		szMessage = GetLangString(0x00D01001);
#else
		szMessage = GetLangString(0x00D01002);
#endif
	}

    MessageBox(hWnd, 
		HaveLangStrings() ? szMessage : szHelpMessage,
		HaveLangStrings() ? GetLangString(0x00D01003) : _T("Command-Line Help"),
        MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
}

BEGIN_MESSAGE_MAP(CEncWAVtoAC3App, CWinAppEx)
    ON_COMMAND(ID_HELP, &CWinAppEx::OnHelp)
END_MESSAGE_MAP()

CEncWAVtoAC3App::CEncWAVtoAC3App()
{

}

CEncWAVtoAC3App::~CEncWAVtoAC3App()
{

}

CEncWAVtoAC3App theApp;

BOOL CEncWAVtoAC3App::InitInstance()
{
	// set portable flag
	m_bIsPortable = PathFileExists(GetExeFilePath() + DEFAULT_PORTABLE_FILE_NAME) == TRUE ? true : false;

	// set configuration file paths
	if (m_bIsPortable == true)
	{
		m_szPresetsFilePath = GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME;
		m_szConfigFilePath = GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME;
		m_szEnginesFilePath = GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME;
		m_szFilesListFilePath = GetExeFilePath() + DEFAULT_FILES_FILE_NAME;
		m_szLangFilePath = GetExeFilePath() + DEFAULT_LANG_FILE_NAME;
		m_szLogFilePath = GetExeFilePath() + DEFAULT_LOG_FILE_NAME;
	}
	else
	{
		CreateDirectory(GetSettingsFilePath(_T("")), NULL);

		m_szPresetsFilePath = GetSettingsFilePath(DEFAULT_PRESETS_FILE_NAME);
		m_szConfigFilePath = GetSettingsFilePath(DEFAULT_CONFIG_FILE_NAME);
		m_szEnginesFilePath = GetSettingsFilePath(DEFAULT_ENGINES_FILE_NAME);
		m_szFilesListFilePath = GetSettingsFilePath(DEFAULT_FILES_FILE_NAME);
		m_szLangFilePath = GetSettingsFilePath(DEFAULT_LANG_FILE_NAME);
		m_szLogFilePath = GetSettingsFilePath(DEFAULT_LOG_FILE_NAME);
	}

	// initialize log file
	InitLog();

	// load language configuration file
	LoadLangConfig(m_szLangFilePath);

	// load all language strings
	LoadLangStrings();

	// initialize application
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	AfxEnableControlContainer();
	InitShellManager();

    // main dialog object
    CEncWAVtoAC3Dlg dlg;

    // command-line helper variables
    int nNumArgs = 1;
    LPTSTR *pArgv = NULL;
    LPTSTR lpCommandLine = NULL;
    volatile bool bExit = false;
    int nCountOptions = 0;
    COptionsParser op;

    // get command line from system
    lpCommandLine = ::GetCommandLine();

    // get command line in *argv[] format
    if(lpCommandLine != NULL)
        pArgv = MyCommandLineToArgv(lpCommandLine, &nNumArgs);

    // free memory if there is only one argument
    if((nNumArgs == 1) && (pArgv != NULL))
        ::GlobalFree(pArgv);

    // parse the command-line specified by user
    if(op.Init(nNumArgs, pArgv, clOptions) == true)
    {
        int nCurFormat = -1;
        int nCurPreset = -1;
        bool bRecurse = true;

        CString szParam;
        int nOptionId;
        while((nOptionId = op.Next()) != COptionsParser::NEXT_LAST_OPTION)
        {
            bExit = false;

            switch(nOptionId)
            {
            case COptionsParser::NEXT_INVALID_ARG:
                {
					ShowCommandLineError(HaveLangStrings() ? GetLangString(0x00D01006) : 
						_T("Invalid command-line argument!"));
                    bExit = true;
                }
                break;
            case CLOP_HELP:
                {
                    ShowCommandLineHelp(NULL);
                    dlg.cmdLineOpt.bHelp = true;
                    bExit = true;
                }
                break;
            case CLOP_ENCODE_AND_EXIT:
                {
                    dlg.cmdLineOpt.bEncodeAndExit = true;
                }
                break;
            case CLOP_READ_ONLY:
                {
                    dlg.cmdLineOpt.bSaveConfig = false;
                    dlg.cmdLineOpt.bHaveSaveConfig = true;
                }
                break;
            case CLOP_INPUT_FILE:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szInputFile = szParam;
                    dlg.cmdLineOpt.bHaveInputFile = true;
                }
                break;
            case CLOP_OUTPUT_FILE:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szOutputFile = szParam;
                    dlg.cmdLineOpt.bHaveOutputFile = true;
                }
                break;
            case CLOP_OUTPUT_PATH:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szOutputPath = szParam;
                    dlg.cmdLineOpt.bHaveOutputPath = true;
                }
                break;
            case CLOP_LOAD_CONFIG:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szLoadConfig = szParam;
                    dlg.cmdLineOpt.bHaveLoadConfig = true;
                }
                break;
            case CLOP_LOAD_FILES:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szLoadFiles = szParam;
                    dlg.cmdLineOpt.bHaveLoadFiles = true;
                }
                break;
            case CLOP_LOAD_PRESETS:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szLoadPresets = szParam;
                    dlg.cmdLineOpt.bHaveLoadPresets = true;
                }
                break;
            case CLOP_LOAD_ENGINES:
                {
                    op.GetParam(szParam, 0);
                    dlg.cmdLineOpt.szLoadEngines = szParam;
                    dlg.cmdLineOpt.bHaveLoadEngines = true;
                }
                break;
            case CLOP_MUX_WIZARD:
                {
                    // show standalone MUX Wizard window
                    CEncWAVtoAC3MuxDlg dlg;
                    m_pMainWnd = &dlg;
                    dlg.DoModal();

                    bExit = true;
                }
                break;
            default:
                {
                    // unknown option Id
					ShowCommandLineError(HaveLangStrings() ? GetLangString(0x00D01007) : 
						_T("Unknown option Id!"));
                    bExit = true;
                }
                break;
            };

            if(bExit == true)
            {
                // free used memory
                ::GlobalFree(pArgv);

				// stop log
				LogClose();

                return FALSE;
            }

            nCountOptions++;
        };

        // free used memory
        ::GlobalFree(pArgv);
    }

    // check for command-line errors
    if((dlg.cmdLineOpt.bHaveOutputFile == true) && (dlg.cmdLineOpt.bHaveInputFile == false))
    {
		ShowCommandLineError(HaveLangStrings() ? GetLangString(0x00D01008) :
			_T("Input file is missing! Please use also --input-file option!."));
        return FALSE;
    }

    if((dlg.cmdLineOpt.bHaveInputFile == true) && (dlg.cmdLineOpt.bHaveLoadFiles == true))
    {
		ShowCommandLineError(HaveLangStrings() ? GetLangString(0x00D01009) :
			_T("Can not combine --input-file with --load-files option!"));
        return FALSE;
    }

    if((dlg.cmdLineOpt.bHaveOutputFile == true) && (dlg.cmdLineOpt.bHaveLoadFiles == true))
    {
		ShowCommandLineError(HaveLangStrings() ? GetLangString(0x00D0100A) :
			_T("Can not combine --output-file with --load-files option!"));
        return FALSE;
    }

    if((dlg.cmdLineOpt.bHaveOutputFile == true) && (dlg.cmdLineOpt.bHaveOutputPath == true))
    {
		ShowCommandLineError(HaveLangStrings() ? GetLangString(0x00D0100B) :
			_T("Can not combine --output-file with --output-path option!"));
        return FALSE;
    }

    // show main dialog
    m_pMainWnd = &dlg;

    // show main window
    dlg.DoModal();

	SaveLangConfig(m_szLangFilePath);

	// stop log
	LogClose();

	// clean language list
	CleanLangList(theApp.m_LangLst);

    // terminate the program
    return FALSE;
}

void CEncWAVtoAC3App::InitLog()
{
	// enable log
	LogFile(m_szLogFilePath);
	LogOpen();
}

bool LoadLangConfig(CString &szFileName)
{
	try
	{
		CMyFile fp;
		if (fp.FOpen(szFileName, false) == false)
			return false;

		ULONGLONG  nRead = 0, nLength = fp.FSize();
		if (nLength == 0)
		{
			fp.FClose();
			return true;
		}

		TCHAR Buffer;
		CString szBuffer = _T("");
		int nFileCounter = 0;

		while (fp.FRead(Buffer) == true)
		{
			if ((Buffer != '\r') && (Buffer != '\n'))
				szBuffer += Buffer;

			if (Buffer == '\n' || nRead == nLength - (fp.FMode() == 1 ? 1 : sizeof(TCHAR)))
			{
				szBuffer += _T("\0");

				// terminate reading if max of input files is reached
				if (nFileCounter >= 1)
				{
					fp.FClose();
					return true;
				}

				// remove leading and trailing quotes (used for *.mux file format)
				szBuffer.TrimLeft('"');
				szBuffer.TrimRight('"');

				theApp.m_szLangFileName = szBuffer;

				// update file counter
				nFileCounter++;

				// reset buffer
				szBuffer = _T("");
			}

			nRead++;
		}

		fp.FClose();
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool SaveLangConfig(CString &szFileName)
{
	try
	{
		CMyFile fp;

		if (fp.FOpen(szFileName, true) == false)
			return false;

		CString szBuffer;

		szBuffer.Format(_T("%s\r\n"), theApp.m_szLangFileName);
		fp.FWriteString(szBuffer.GetBuffer(), szBuffer.GetLength());
		szBuffer.ReleaseBuffer();

		fp.FClose();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void LoadLangStrings()
{
	CString szLangPath = GetExeFilePath() + DEFAULT_LANG_DIRECTORY;

	SearchFolderForLang(szLangPath, false, theApp.m_LangLst);

	if (theApp.m_LangLst.GetCount() > 0)
	{
		bool haveLang = false;

		POSITION pos = theApp.m_LangLst.GetHeadPosition();
		int i = 0;
		while (pos)
		{
			Lang lang = theApp.m_LangLst.GetNext(pos);

			if (lang.szFileName.Compare(theApp.m_szLangFileName) == 0)
			{
				theApp.m_nLangId = i;
				theApp.m_bHaveLang = TRUE;
				theApp.m_Lang = lang.lm;
				haveLang = true;
				break;
			}

			i++;
		}

		if (haveLang == false)
		{
			Lang lang = theApp.m_LangLst.GetHead();

			theApp.m_nLangId = 0;
			theApp.m_bHaveLang = TRUE;
			theApp.m_Lang = lang.lm;
			theApp.m_szLangFileName = lang.szFileName;
		}
	}
	else
	{
		theApp.m_nLangId = -1;
		theApp.m_bHaveLang = FALSE;
	}
}

inline BOOL HaveLangStrings()
{
	return theApp.m_bHaveLang;
}

inline CString& GetLangString(int id)
{
	// return (*theApp.m_Lang)[id];
	return theApp.m_Lang->PLookup(id)->value;
}
