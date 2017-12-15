#pragma once

#include "afxwin.h"
#include "configuration\Configuration.h"
#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"

class CMuxDlg : public CMyDialogEx
{
    DECLARE_DYNAMIC(CMuxDlg)
public:
    CMuxDlg(CWnd* pParent = nullptr);
    virtual ~CMuxDlg();
    enum { IDD = IDD_DIALOG_MUX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
protected:
    void InitCtrls();
    void InitLang();
public:
    void RemapFilesToChannels();
    void SetFilePaths();
    void SetControlsState();
    bool LoadFilesList(CString &szFileName);
    bool SaveFilesList(CString &szFileName, int nFormat);
    void ShowOpenFileDlg(int nID, CMyButton *m_BtnCurrent, CMyEdit *m_EdtCurrent);
public:
    int nChannelConfig;
    bool bLFE;
    CString szInputFiles[CEncoderDefaults::nNumMaxInputFiles];
    CString szTmpInputFiles[CEncoderDefaults::nNumMaxInputFiles];
public:
    CMyButton m_BtnChannelFL;
    CMyButton m_BtnChannelFR;
    CMyButton m_BtnChannelFC;
    CMyButton m_BtnChannelLFE;
    CMyButton m_BtnChannelSL;
    CMyButton m_BtnChannelSR;
    CMyButton m_BtnChannelS;
    CMyButton m_ChkChannelConfigLFE;
public:
    CMyStatic m_StcLabelFL;
    CMyStatic m_StcLabelFR;
    CMyStatic m_StcLabelFC;
    CMyStatic m_StcLabelLFE;
    CMyStatic m_StcLabelSL;
    CMyStatic m_StcLabelSR;
public:
    CMyComboBox m_CmbChannelConfig;
public:
    CMyEdit m_EdtChannelFL;
    CMyEdit m_EdtChannelFR;
    CMyEdit m_EdtChannelFC;
    CMyEdit m_EdtChannelLFE;
    CMyEdit m_EdtChannelSL;
    CMyEdit m_EdtChannelSR;
public:
    afx_msg void OnBnClickedButtonFl();
    afx_msg void OnBnClickedButtonFr();
    afx_msg void OnBnClickedButtonFc();
    afx_msg void OnBnClickedButtonLfe();
    afx_msg void OnBnClickedButtonSl();
    afx_msg void OnBnClickedButtonSr();
    afx_msg void OnBnClickedButtonS();
public:
    afx_msg void OnBnClickedButtonClearFl();
    afx_msg void OnBnClickedButtonClearFc();
    afx_msg void OnBnClickedButtonClearFr();
    afx_msg void OnBnClickedButtonClearLfe();
    afx_msg void OnBnClickedButtonClearSl();
    afx_msg void OnBnClickedButtonClearSr();
public:
    afx_msg void OnBnClickedButtonExport();
    afx_msg void OnBnClickedButtonImport();
public:
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
public:
    afx_msg void OnBnClickedCheckChannelConfigLfe();
public:
    afx_msg void OnCbnSelchangeComboChannelConfig();
};
