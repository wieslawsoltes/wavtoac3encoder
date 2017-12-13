#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"
#include "dialogs\MuxDlg.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"

CLangManager theLangManager
CEncWAVtoAC3App theApp;

bool CLangManager::LoadLangConfig(CString &szFileName)
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

                m_szLangFileName = szBuffer;

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

bool CLangManager::SaveLangConfig(CString &szFileName)
{
    try
    {
        CMyFile fp;

        if (fp.FOpen(szFileName, true) == false)
            return false;

        CString szBuffer;

        szBuffer.Format(_T("%s\r\n"), m_szLangFileName);
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

void CLangManager::LoadLangStrings()
{
    CString szLangPath = GetExeFilePath() + DEFAULT_LANG_DIRECTORY;

    SearchFolderForLang(szLangPath, false, m_LangLst);

    if (m_LangLst.GetCount() > 0)
    {
        bool haveLang = false;

        POSITION pos = m_LangLst.GetHeadPosition();
        int i = 0;
        while (pos)
        {
            Lang lang = m_LangLst.GetNext(pos);

            if (lang.szFileName.Compare(m_szLangFileName) == 0)
            {
                m_nLangId = i;
                m_bHaveLang = TRUE;
                m_Lang = lang.lm;
                haveLang = true;
                break;
            }

            i++;
        }

        if (haveLang == false)
        {
            Lang lang = m_LangLst.GetHead();

            m_nLangId = 0;
            m_bHaveLang = TRUE;
            m_Lang = lang.lm;
            m_szLangFileName = lang.szFileName;
        }
    }
    else
    {
        m_nLangId = -1;
        m_bHaveLang = FALSE;
    }
}

BOOL CLangManager::HaveLangStrings()
{
    return m_bHaveLang;
}

CString& CLangManager::GetLangString(int id)
{
    // return (*m_Lang)[id];
    return m_Lang->PLookup(id)->value;
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

BOOL CEncWAVtoAC3App::InitInstance()
{
    m_bIsPortable = PathFileExists(GetExeFilePath() + DEFAULT_PORTABLE_FILE_NAME) == TRUE ? true : false;

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
        CreateDirectory(GetSettingsFilePath(_T(""), DEFAULT_CONFIG_DIRECTORY), NULL);

        m_szPresetsFilePath = GetSettingsFilePath(DEFAULT_PRESETS_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_szConfigFilePath = GetSettingsFilePath(DEFAULT_CONFIG_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_szEnginesFilePath = GetSettingsFilePath(DEFAULT_ENGINES_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_szFilesListFilePath = GetSettingsFilePath(DEFAULT_FILES_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_szLangFilePath = GetSettingsFilePath(DEFAULT_LANG_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
        m_szLogFilePath = GetSettingsFilePath(DEFAULT_LOG_FILE_NAME, DEFAULT_CONFIG_DIRECTORY);
    }

    theLangManager.LoadLangConfig(m_szLangFilePath);
    theLangManager.LoadLangStrings();

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

    theLangManager.SaveLangConfig(m_szLangFilePath);
    CleanLangList(theLangManager..m_LangLst);

    return FALSE;
}
