#pragma once

namespace dialogs
{
    class CAboutDlg : public controls::CMyDialogEx
    {
        DECLARE_DYNAMIC(CAboutDlg)
    public:
        CAboutDlg(CWnd* pParent = nullptr);
        virtual ~CAboutDlg();
    public:
        enum { IDD = IDD_DIALOG_ABOUT };
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();
    public:
        config::CConfiguration * pConfig;
    protected:
        void InitLang();
    protected:
        DECLARE_MESSAGE_MAP()
    public:
        controls::CMyButton m_BtnOK;
        controls::CMyStatic m_StcMainAppName;
        controls::CMyHyperlink m_StcWebsite;
        controls::CMyHyperlink m_StcEmail;
        controls::CMyStatic m_StcLicense;
    };
}
