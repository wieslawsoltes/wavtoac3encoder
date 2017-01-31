//
// WAV to AC3 Encoder
// Copyright (C) 2007-2016 Wies³aw Šoltés <wieslaw.soltes@gmail.com>
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
#include "Language.h"
#include "MyFile.h"

void SearchFolderForLang(CString szPath, const bool bRecurse, LangList_t& m_LangLst)
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

                    if (::LoadLang(szTempBuf, lm) == true)
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

void CleanLangList(LangList_t& m_LangLst)
{
    POSITION pos = m_LangLst.GetHeadPosition();
    while (pos)
    {
        Lang lang = m_LangLst.GetNext(pos);
        delete lang.lm;
    }
}

bool LoadLang(CString &szFileName, LangMap_t *lm)
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
