#pragma once

#include <string>
#include <afxwin.h>
#include "configuration\Configuration.h"
#include "utilities\StringHelper.h"
#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"

namespace app
{
    class CMuxDlg : public controls::CMyDialogEx
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
        bool LoadFilesList(std::wstring &szFileName);
        bool SaveFilesList(std::wstring &szFileName, int nFormat);
        void ShowOpenFileDlg(int nID, controls::CMyButton *m_BtnCurrent, controls::CMyEdit *m_EdtCurrent);
    public:
        int nChannelConfig;
        bool bLFE;
        std::wstring szInputFiles[config::CEncoderDefaults::nNumMaxInputFiles];
        std::wstring szTmpInputFiles[config::CEncoderDefaults::nNumMaxInputFiles];
    public:
        controls::CMyButton m_BtnChannelFL;
        controls::CMyButton m_BtnChannelFR;
        controls::CMyButton m_BtnChannelFC;
        controls::CMyButton m_BtnChannelLFE;
        controls::CMyButton m_BtnChannelSL;
        controls::CMyButton m_BtnChannelSR;
        controls::CMyButton m_BtnChannelS;
        controls::CMyButton m_ChkChannelConfigLFE;
        controls::CMyStatic m_StcLabelFL;
        controls::CMyStatic m_StcLabelFR;
        controls::CMyStatic m_StcLabelFC;
        controls::CMyStatic m_StcLabelLFE;
        controls::CMyStatic m_StcLabelSL;
        controls::CMyStatic m_StcLabelSR;
        controls::CMyComboBox m_CmbChannelConfig;
        controls::CMyEdit m_EdtChannelFL;
        controls::CMyEdit m_EdtChannelFR;
        controls::CMyEdit m_EdtChannelFC;
        controls::CMyEdit m_EdtChannelLFE;
        controls::CMyEdit m_EdtChannelSL;
        controls::CMyEdit m_EdtChannelSR;
    public:
        afx_msg void OnBnClickedButtonFl();
        afx_msg void OnBnClickedButtonFr();
        afx_msg void OnBnClickedButtonFc();
        afx_msg void OnBnClickedButtonLfe();
        afx_msg void OnBnClickedButtonSl();
        afx_msg void OnBnClickedButtonSr();
        afx_msg void OnBnClickedButtonS();
        afx_msg void OnBnClickedButtonClearFl();
        afx_msg void OnBnClickedButtonClearFc();
        afx_msg void OnBnClickedButtonClearFr();
        afx_msg void OnBnClickedButtonClearLfe();
        afx_msg void OnBnClickedButtonClearSl();
        afx_msg void OnBnClickedButtonClearSr();
        afx_msg void OnBnClickedButtonExport();
        afx_msg void OnBnClickedButtonImport();
        afx_msg void OnBnClickedCancel();
        afx_msg void OnBnClickedOk();
        afx_msg void OnBnClickedCheckChannelConfigLfe();
        afx_msg void OnCbnSelchangeComboChannelConfig();
    };
}
