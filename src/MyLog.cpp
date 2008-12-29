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

#include "stdafx.h"
#include "MyLog.h"

void LogDefaults(LogCtx *ctx)
{
    if(ctx == NULL)
        return;

    ctx->bInit = false;
    ctx->szLogFileName = DEFAULT_LOG_FILE_NAME;
}

void LogFile(LogCtx *ctx, CString szLogFileName)
{
    if(ctx == NULL)
        return;

    ctx->szLogFileName = szLogFileName;
}

bool LogOpen(LogCtx *ctx)
{
    if(ctx == NULL)
        return false;

    if(ctx->bInit == true)
        return true;

    BOOL bRet = FALSE;

    bRet = ctx->fp.Open(ctx->szLogFileName, 
        CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyWrite);
    if(bRet == FALSE)
    {
        ctx->bInit = false;
        return false;
    }
    else
    {
        if(ctx->fp.GetLength() > 0)
        {
            ctx->fp.Seek(0 ,CFile::end);
        }
#ifdef _UNICODE
        else
        {
            // mark the file contents as UNICODE text
            const unsigned char szUnicode[2] = { 0xFF, 0xFE };
            ctx->fp.Write(szUnicode, 2);
        }
#endif // _UNICODE

        ctx->bInit = true;
        return true;
    }
}

bool LogClose(LogCtx *ctx)
{
    if(ctx == NULL)
        return false;

    if(ctx->bInit == false)
        return true;

    ctx->fp.Close();
    ctx->bInit = false;
    ctx->szLogFileName = _T("");

    return true;
}

bool LogMessage(LogCtx *ctx, CString szMessage)
{
    if(ctx == NULL)
        return false;

    if(ctx->bInit == false)
        return false;

    // add current date&time end end-line chars to message
    CString szCurrentDateTtime;
    SYSTEMTIME st;
    ZeroMemory(&st, sizeof(SYSTEMTIME));
    GetLocalTime(&st);
    szCurrentDateTtime.Format(_T("[%04d-%02d-%02d, %02d:%02d:%02d.%03d] "), 
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    CString tmpBuffer = szCurrentDateTtime + szMessage + _T("\r\n");

    ctx->fp.Write(tmpBuffer.GetBuffer(), tmpBuffer.GetLength() * sizeof(TCHAR));
    tmpBuffer.ReleaseBuffer();

    return true;
}
