#pragma once

#include <afxcmn.h>
#include <afxwin.h>
#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"

class CAboutDlg : public CMyDialogEx
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
protected:
    void SetBuildInfo();
    void InitLang();
protected:
    DECLARE_MESSAGE_MAP()
public:
    CMyStatic m_StcLicense;
    CMyStatic m_StcBuildInfo;
    CMyButton m_BtnOK;
};
