#include "StdAfx.h"
#include "MainApp.h"
#include "dialogs\MainDlg.h"
#include "dialogs\MuxDlg.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"

CLangManager theLangManager;
CEncWAVtoAC3App theApp;

//
// CONFIG
//

bool LoadConfig(CString &szFileName, ConfigList_t &cl)
{
    try
    {
        CMyFile fp;
        if (fp.FOpen(szFileName, false) == false)
            return false;

        ConfigEntry ce;

        // clear list
        cl.RemoveAll();

        TCHAR Buffer;
        const ULONGLONG nLength = fp.FSize();
        CString szBuffer = _T("");

        while (fp.FRead(Buffer) == true)
        {
            if ((Buffer != '\r') && (Buffer != '\n'))
                szBuffer += Buffer;

            // we have full line if there is end of line mark or end of file
            if ((Buffer == '\n') || (fp.FPos() == nLength))
            {
                szBuffer += _T("\0");

                int nPos = szBuffer.Find('=', 0);
                if (nPos != -1)
                {
                    ce.szKey = szBuffer.Mid(0, nPos);
                    ce.szData = szBuffer.Mid(nPos + 1, szBuffer.GetLength() - 1);
                    cl.AddTail(ce);
                }

                szBuffer = _T("");
            }
        }

        fp.FClose();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool SaveConfig(CString &szFileName, ConfigList_t &cl)
{
    int nSize = (int)cl.GetSize();
    try
    {
        CMyFile fp;
        if (fp.FOpen(szFileName, true) == false)
            return false;

        for (int i = 0; i < nSize; i++)
        {
            CString szBuffer;
            ConfigEntry ce = cl.GetAt(cl.FindIndex(i));

            // format and save key/data pair
            szBuffer.Format(_T("%s=%s\r\n"), ce.szKey, ce.szData);
            fp.FWriteString(szBuffer.GetBuffer(), szBuffer.GetLength());
            szBuffer.ReleaseBuffer();
        }

        fp.FClose();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

//
// LANG
//

void CLangManager::SearchFolderForLang(CString szPath, const bool bRecurse, LangList_t& m_LangLst)
{
    try
    {
        WIN32_FIND_DATA w32FileData;
        HANDLE hSearch = NULL;
        BOOL fFinished = FALSE;
        TCHAR cTempBuf[(MAX_PATH * 2) + 1];

        ZeroMemory(&w32FileData, sizeof(WIN32_FIND_DATA));
        ZeroMemory(cTempBuf, MAX_PATH * 2);

        // remove '\' or '/' from end of search path
        szPath.TrimRight(_T("\\"));
        szPath.TrimRight(_T("/"));

        wsprintf(cTempBuf, _T("%s\\*.*\0"), szPath);

        hSearch = FindFirstFile(cTempBuf, &w32FileData);
        if (hSearch == INVALID_HANDLE_VALUE)
            return;

        while (fFinished == FALSE)
        {
            if (w32FileData.cFileName[0] != '.' &&
                !(w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {
                CString szTempBuf;
                szTempBuf.Format(_T("%s\\%s\0"), szPath, w32FileData.cFileName);

                // apply filter and add only .txt files
                CString szExt = ::PathFindExtension(szTempBuf);
                szExt.MakeLower();
                szExt.Remove('.');

                // add only files with proper file extensions
                if (szExt.CompareNoCase(_T("txt")) == 0)
                {
                    Lang lang;
                    LangMap_t *lm = new LangMap_t();

                    if (this->LoadLang(szTempBuf, lm) == true)
                    {
                        lang.lm = lm;
                        lang.szFileName = szTempBuf;
                        lang.szEnglishName = (*lang.lm)[0x00000001];
                        lang.szTargetName = (*lang.lm)[0x00000002];

                        m_LangLst.AddTail(lang);
                    }
                }
            }

            if (w32FileData.cFileName[0] != '.' &&
                w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                wsprintf(cTempBuf, _T("%s\\%s\0"), szPath, w32FileData.cFileName);

                // recurse subdirs
                if (bRecurse == true)
                    SearchFolderForLang(cTempBuf, true, m_LangLst);
            }

            if (FindNextFile(hSearch, &w32FileData) == FALSE)
            {
                if (GetLastError() == ERROR_NO_MORE_FILES)
                    fFinished = TRUE;
                else
                    return;
            }
        }

        if (FindClose(hSearch) == FALSE)
            return;
    }
    catch (...)
    {
        MessageBox(NULL,
            HaveLangStrings() ? GetLangString(0x0020702A) : _T("Error while searching for files!"),
            HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
            MB_OK | MB_ICONERROR);
    }
}

void CLangManager::CleanLangList(LangList_t& m_LangLst)
{
    POSITION pos = m_LangLst.GetHeadPosition();
    while (pos)
    {
        Lang lang = m_LangLst.GetNext(pos);
        delete lang.lm;
    }
}

bool CLangManager::LoadLang(CString &szFileName, LangMap_t *lm)
{
    try
    {
        CMyFile fp;
        if (fp.FOpen(szFileName, false) == false)
            return false;

        // clear list
        lm->RemoveAll();

        TCHAR Buffer;
        const ULONGLONG nLength = fp.FSize();
        CString szBuffer = _T("");

        int key;
        CString szKey = _T("");
        CString szValue = _T("");

        while (fp.FRead(Buffer) == true)
        {
            if ((Buffer != '\r') && (Buffer != '\n'))
                szBuffer += Buffer;

            // we have full line if there is end of line mark or end of file
            if ((Buffer == '\n') || (fp.FPos() == nLength))
            {
                szBuffer += _T("\0");

                int nPos = szBuffer.Find('=', 0);
                if (nPos != -1)
                {
                    szKey = szBuffer.Mid(0, nPos);
                    szValue = szBuffer.Mid(nPos + 1, szBuffer.GetLength() - 1);

                    szValue.Replace(_T("\\n"), _T("\n"));
                    szValue.Replace(_T("\\t"), _T("\t"));

                    _stscanf(szKey, _T("%x"), &key);

                    (*lm)[key] = szValue;
                }

                szBuffer = _T("");
            }
        }

        fp.FClose();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

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

//
// APP
//

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
