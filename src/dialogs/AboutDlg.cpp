//
// WAV to AC3 Encoder
// Copyright (C) 2007-2017 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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
#include "MainApp.h"
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

    SetBuildInfo();
    InitLang();

    return TRUE;
}

void CAboutDlg::SetBuildInfo()
{
    CString szBuildInfo = _T("");

    szBuildInfo.Format(_T("%s %s,"),
        HaveLangStrings() ? GetLangString(0x00901004) : _T("Version"),
        ENCWAVTOAC3_VERSION);

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

    szBuildInfo += theApp.m_bIsPortable ? _T(" (Portable)") : _T(" (Roaming)");

    this->m_StcBuildInfo.SetWindowText(szBuildInfo);
}

void CAboutDlg::InitLang()
{
    if (HaveLangStrings())
    {
        this->SetWindowText(GetLangString(0x00901002));
        this->GetDlgItem(IDOK)->SetWindowText(GetLangString(0x00901001));
        this->GetDlgItem(IDC_STATIC_LICENSE)->SetWindowText(GetLangString(0x00901003));
    }
}
