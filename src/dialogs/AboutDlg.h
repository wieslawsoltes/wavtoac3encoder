#pragma once

#include <string>
#include <afxcmn.h>
#include <afxwin.h>
#include "utilities\StringHelper.h"
#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "logger\Log.h"
#include "configuration\Configuration.h"

namespace app
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
        void SetBuildInfo();
        void InitLang();
    protected:
        DECLARE_MESSAGE_MAP()
    public:
        controls::CMyStatic m_StcLicense;
        controls::CMyStatic m_StcBuildInfo;
        controls::CMyButton m_BtnOK;
    };
}
