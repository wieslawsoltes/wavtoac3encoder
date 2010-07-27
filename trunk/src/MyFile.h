//
// WAV to AC3 Encoder
// Copyright (C) 2007,2008,2009,2010 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

class CMyFile : private CFile
{
private:
    static const UINT nOpenFlagsRead = modeRead | typeBinary;
    static const UINT nOpenFlagsWrite = modeCreate | modeRead | modeWrite | typeBinary;
private:
    CString _szFileName;
    bool _bOpen;
    bool _bWrite;
private:
    static const size_t nSizeOf_TCHAR = sizeof(TCHAR);
    static const size_t nSizeOf_wchar_t = sizeof(wchar_t);
    static const size_t nSizeOf_char = sizeof(char);
private:
    // Ansi buffer
    char BufferA;
    // Unicode buffer
    wchar_t BufferU;
private:
    // read & write modes
    // 0 -> src = Unicode, dst = Unicode
    // 1 -> src = Ansi, dst = Unicode
    // 2 -> src = Unicode, dst = Ansi
    // 3 -> src = Ansi, dst = Ansi
    int _nMode;
public:
    CMyFile();
    CMyFile(CString szFileName, bool bWrite = false);
    virtual ~CMyFile();
public:
#ifdef _UNICODE
    bool FSetMode(int nMode = 0);
#else
    bool FSetMode(int nMode = 3);
#endif
    int FMode();
public:
    ULONGLONG FSize();
    ULONGLONG FPos();
public:
    bool FOpen(CString szFileName = _T(""), bool bWrite = false);
    bool FClose();
public:
    bool FRead(TCHAR &Buffer);
    bool FWrite(TCHAR &Buffer);
public:
    UINT FReadString(TCHAR *Buffer, UINT nLength);
    bool FWriteString(TCHAR *Buffer, UINT nLength);
};
