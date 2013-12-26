﻿//
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

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "AboutDlg.h"

IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDOK, m_BtnOK);
    DDX_Control(pDX, IDC_STATIC_BUILD_INFO, m_StcBuildInfo);
    DDX_Control(pDX, IDC_STATIC_LICENSE, m_StcLicense);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // set build information
    CString szBuildInfo = _T("");
    szBuildInfo.Format(_T("Version %s,"),  ENCWAVTOAC3_VERSION);

#if defined(_WIN32) & !defined(_WIN64)
    szBuildInfo += _T(" x86, Win32");
#else
    szBuildInfo += _T(" AMD64, Win64");
#endif // _WIN32

#if defined(_UNICODE)
    szBuildInfo += _T(" Unicode");
#else
    szBuildInfo += _T(" Ansi");
#endif // _UNICODE

#if defined(_DEBUG)
    szBuildInfo += _T(" DEBUG");
#endif // _DEBUG

    szBuildInfo += _T(", ");
    szBuildInfo += __DATE__;
    szBuildInfo += _T(" ");
    szBuildInfo += __TIME__;

    this->m_StcBuildInfo.SetWindowText(szBuildInfo);

    return TRUE;
}