//
// WAV to AC3 Encoder
// Copyright (C) 2007-2013 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

class CMyCounter
{
    LARGE_INTEGER li[3];
    LARGE_INTEGER total;
public:
    void Init()   { total.QuadPart = 0; memset(li, 0, 3 * sizeof(LARGE_INTEGER)); QueryPerformanceFrequency(&li[0]); }
    void Start()  { QueryPerformanceCounter(&li[1]); }
    void Stop()   { QueryPerformanceCounter(&li[2]); total.QuadPart += li[2].QuadPart - li[1].QuadPart; }
    double Time() { return (double) (total.QuadPart) / (double) li[0].QuadPart; }
};

CString FormatTime(double fTime, int nFormat);

void SetComboBoxHeight(HWND hDlg, int nComboBoxID);

ULONGLONG GetFileSize64(CString szFileName);

__int64 GetFileSizeInt64(FILE *fp);

CString GetFileExt(CString szFilePath);

CString GetFileName(CString szFilePath);

CString GetExeFilePath();

bool MakeFullPath(CString szPath);

void LaunchAndWait(LPCTSTR file, LPCTSTR params, BOOL bWait);

void ConvertAnsiToUnicode(const char *szAnsi, wchar_t *szUnicode, ULONG nLength);

void ConvertUnicodeToAnsi(const wchar_t *szUnicode, char *szAnsi, ULONG nLength);
