﻿#include "StdAfx.h"
#include "MainApp.h"
#include "AboutDlg.h"

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
    : CMyDialogEx(CAboutDlg::IDD, pParent)
{
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CMyDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDOK, m_BtnOK);
    DDX_Control(pDX, IDC_STATIC_BUILD_INFO, m_StcBuildInfo);
    DDX_Control(pDX, IDC_STATIC_LICENSE, m_StcLicense);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CMyDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CMyDialogEx::OnInitDialog();

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
