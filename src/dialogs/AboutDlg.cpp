#include "StdAfx.h"
#include "MainApp.h"
#include "AboutDlg.h"

namespace dialogs
{
    IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)
    CAboutDlg::CAboutDlg(CWnd* pParent /*=nullptr*/)
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
        DDX_Control(pDX, IDC_STATIC_TEXT_APP_NAME, m_StcMainAppName);
        DDX_Control(pDX, IDC_STATIC_TEXT_WEBSITE, m_StcWebsite);
        DDX_Control(pDX, IDC_STATIC_TEXT_EMAIL, m_StcEmail);
        DDX_Control(pDX, IDC_STATIC_LICENSE, m_StcLicense);
    }

    BEGIN_MESSAGE_MAP(CAboutDlg, CMyDialogEx)
    END_MESSAGE_MAP()

    BOOL CAboutDlg::OnInitDialog()
    {
        CMyDialogEx::OnInitDialog();

        m_StcMainAppName.SetBold(true);
        m_StcMainAppName.SetWindowText(_T(VER_PRODUCTNAME_STR " v" VER_FILE_VERSION_SHORT_STR " " VER_COPYRIGHT_STR));
        m_StcWebsite.SetWindowText(_T("https://github.com/wieslawsoltes/wavtoac3encoder"));
        m_StcWebsite.SetTargetUrl(_T("https://github.com/wieslawsoltes/wavtoac3encoder"));
        m_StcEmail.SetWindowText(_T("wieslaw.soltes@gmail.com"));
        m_StcEmail.SetTargetUrl(_T("mailto:wieslaw.soltes@gmail.com"));

        InitLang();

        return TRUE;
    }

    void CAboutDlg::InitLang()
    {
        this->SetWindowText(this->pConfig->GetString(0x00901002).c_str());
        this->GetDlgItem(IDOK)->SetWindowText(this->pConfig->GetString(0x00901001).c_str());
        this->GetDlgItem(IDC_STATIC_LICENSE)->SetWindowText(this->pConfig->GetString(0x00901003).c_str());
    }
}
