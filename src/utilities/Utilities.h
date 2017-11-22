// Copyright (c) Wiesław Šoltés. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

void SetComboBoxHeight(HWND hDlg, int nComboBoxID, int nSizeLimit);

ULONGLONG GetFileSize64(CString szFileName);

__int64 GetFileSizeInt64(FILE *fp);

CString GetFileExt(CString szFilePath);

CString GetFileName(CString szFilePath);

CString GetExeFilePath();

CString GetSettingsFilePath(CString szFileName);

bool MakeFullPath(CString szPath);

void LaunchAndWait(LPCTSTR file, LPCTSTR params, BOOL bWait);

void ConvertAnsiToUnicode(const char *szAnsi, wchar_t *szUnicode, ULONG nLength);

void ConvertUnicodeToAnsi(const wchar_t *szUnicode, char *szAnsi, ULONG nLength);
