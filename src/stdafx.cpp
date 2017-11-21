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
#include "..\utilities\Utilities.h"

LogCtx logCtx;

void LogFile(CString szLogFileName)
{
    logCtx.szLogFileName = szLogFileName;
}

bool LogOpen()
{
    if (logCtx.bInit == true)
        return false;

    BOOL bRet = FALSE;
    bRet = logCtx.fp.Open(logCtx.szLogFileName,
        CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyWrite);
    if (bRet == FALSE)
    {
        logCtx.bInit = false;
        return false;
    }
    else
    {
        if (logCtx.fp.GetLength() > 0)
        {
            logCtx.fp.Seek(0, CFile::end);
        }
#ifdef _UNICODE
        else
        {
            // mark the file contents as UNICODE text
            const unsigned char szUnicode[2] = { 0xFF, 0xFE };
            logCtx.fp.Write(szUnicode, 2);
        }
#endif // _UNICODE

        logCtx.bInit = true;
        return true;
    }
}

bool LogClose()
{
    if (logCtx.bInit == false)
        return false;

    logCtx.fp.Close();
    logCtx.bInit = false;
    logCtx.szLogFileName = _T("");
    return true;
}

bool LogMessage(CString szMessage)
{
    if (logCtx.bInit == false)
        return false;

    CString szCurrentDateTtime;
    SYSTEMTIME st;
    ZeroMemory(&st, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    szCurrentDateTtime.Format(_T("[%04d-%02d-%02d, %02d:%02d:%02d.%03d] "),
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    CString tmpBuffer = szCurrentDateTtime + szMessage + _T("\r\n");
    logCtx.fp.Write(tmpBuffer.GetBuffer(), tmpBuffer.GetLength() * sizeof(TCHAR));
    tmpBuffer.ReleaseBuffer();

    return true;
}

void LogAnsiMessage(const PTCHAR pszMessage, const char* msg)
{
    const char *ansistr = msg;
    int lenA = lstrlenA(ansistr);
    int lenW;
    BSTR unicodestr;
    lenW = ::MultiByteToWideChar(CP_ACP, 0, ansistr, lenA, 0, 0);
    if (lenW > 0)
    {
        unicodestr = ::SysAllocStringLen(0, lenW);
        ::MultiByteToWideChar(CP_ACP, 0, ansistr, lenA, unicodestr, lenW);
        CString szBuff;
        szBuff.Format(_T("%s: %s"), pszMessage, unicodestr);
        ::LogMessage(szBuff);
    }
    ::SysFreeString(unicodestr);
}