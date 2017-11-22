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

#include "StdAfx.h"
#include "Utilities.h"

void SetComboBoxHeight(HWND hDlg, int nComboBoxID, int nSizeLimit)
{
	HWND hComboxBox = ::GetDlgItem(hDlg, nComboBoxID);
	if (hComboxBox != NULL)
		::SendMessage(hComboxBox, CB_SETMINVISIBLE, (WPARAM)nSizeLimit, 0);
}

ULONGLONG GetFileSize64(CString szFileName)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    ULARGE_INTEGER ulSize;
    ULONGLONG nFileSize;

    hFind = ::FindFirstFile(szFileName, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        return 0;

    ::FindClose(hFind);

    ulSize.HighPart = FindFileData.nFileSizeHigh;
    ulSize.LowPart = FindFileData.nFileSizeLow;
    nFileSize = ulSize.QuadPart;

    return nFileSize;
}

__int64 GetFileSizeInt64(FILE *fp)
{
    __int64 nCurPos, nSize;
    nCurPos = _ftelli64(fp);
    _fseeki64(fp, 0, SEEK_END);
    nSize = _ftelli64(fp);
    _fseeki64(fp, nCurPos, SEEK_SET);
    return nSize;
}

CString GetFileExt(CString szFilePath)
{
    CString szExt = ::PathFindExtension(szFilePath);
    szExt.Remove('.');
    return szExt;
}

static UINT MyGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
    LPTSTR lpszTemp = ::PathFindFileName(lpszPathName);
    if (lpszTitle == NULL)
        return lstrlen(lpszTemp) + 1;

    lstrcpyn(lpszTitle, lpszTemp, nMax);
    return(0);
}

CString GetFileName(CString szFilePath)
{
    CString strResult;
    MyGetFileName(szFilePath, strResult.GetBuffer(_MAX_FNAME), _MAX_FNAME);
    strResult.ReleaseBuffer();
    return strResult;
}

CString GetExeFilePath()
{
    TCHAR szExeFilePath[MAX_PATH + 1] = _T("");
    DWORD dwRet = ::GetModuleFileName(::GetModuleHandle(NULL), szExeFilePath, MAX_PATH);
    if (dwRet > 0)
    {
        CString szTempBuff1;
        CString szTempBuff2;

        szTempBuff1.Format(_T("%s"), szExeFilePath);
        szTempBuff2 = ::GetFileName(szTempBuff1);
        szTempBuff1.TrimRight(szTempBuff2);

        return szTempBuff1;
    }
    return NULL;
}

CString GetSettingsFilePath(CString szFileName)
{
    TCHAR szPath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL,
        CSIDL_APPDATA | CSIDL_FLAG_CREATE,
        NULL,
        0,
        szPath)))
    {
        PathAppend(szPath, DEFAULT_CONFIG_DIRECTORY);
        PathAppend(szPath, szFileName);
        return szPath;
    }
    return NULL;
}

bool MakeFullPath(CString szPath)
{
    if (szPath[szPath.GetLength() - 1] != '\\')
        szPath = szPath + _T("\\");

    CString szTmpDir = szPath.Left(2);
    _tchdir(szTmpDir);

    int nStart = 3;
    int nEnd = 0;
    while (true)
    {
        nEnd = szPath.Find('\\', nStart);
        if (nEnd == -1)
            return true;

        CString szNextDir = szPath.Mid(nStart, nEnd - nStart);
        CString szCurDir = szTmpDir + _T("\\") + szNextDir;
        if (_tchdir(szCurDir) != 0)
        {
            _tchdir(szTmpDir);
            if (_tmkdir(szNextDir) != 0)
                return false;
        }

        szTmpDir += _T("\\") + szNextDir;
        nStart = nEnd + 1;
    }
    return false;
}

void LaunchAndWait(LPCTSTR file, LPCTSTR params, BOOL bWait)
{
    SHELLEXECUTEINFO sei;
    ::ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    // sei.lpVerb is uninitialized, so that default action will be taken
    sei.nShow = SW_SHOWNORMAL;
    sei.lpFile = file;
    sei.lpParameters = params;

    // check the return value
    ::ShellExecuteEx(&sei);

    // wait till the child terminates
    if (bWait == TRUE)
        ::WaitForSingleObject(sei.hProcess, INFINITE);

    ::CloseHandle(sei.hProcess);
}

void ConvertAnsiToUnicode(const char *szAnsi, wchar_t *szUnicode, ULONG nLength)
{
    // use always + 1 to null-terminate string
    _mbstowcsz(szUnicode, szAnsi, nLength + 1);
}

void ConvertUnicodeToAnsi(const wchar_t *szUnicode, char *szAnsi, ULONG nLength)
{
    // use always + 1 to null-terminate string
    _wcstombsz(szAnsi, szUnicode, nLength + 1);
}
