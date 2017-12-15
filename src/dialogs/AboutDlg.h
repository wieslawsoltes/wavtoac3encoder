#pragma once

#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"

#include "afxwin.h"

class CAboutDlg : public CMyDialogEx
{
    DECLARE_DYNAMIC(CAboutDlg)
public:
    CAboutDlg(CWnd* pParent = NULL);
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
