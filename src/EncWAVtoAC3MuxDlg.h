//
// WAV to AC3 Encoder
// Copyright (C) 2007-2008 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#pragma once

#include "afxwin.h"

#include "MyButton.h"
#include "MyStatic.h"
#include "MyEdit.h"
#include "MyComboBoxEdit.h"
#include "MyListCtrl.h"
#include "MyComboBox.h"
#include "MySliderCtrl.h"

#include "EncoderOptions.h"

class CEncWAVtoAC3MuxDlg : public CDialog
{
    DECLARE_DYNAMIC(CEncWAVtoAC3MuxDlg)
public:
    CEncWAVtoAC3MuxDlg(CWnd* pParent = NULL);
    virtual ~CEncWAVtoAC3MuxDlg();
    enum { IDD = IDD_MUX_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
public:
    void RemapFilesToChannels();
    void SetFilePaths();
    void SetControlsState();
    bool LoadFilesList(CString &szFileName);
    bool SaveFilesList(CString &szFileName, int nFormat);
    void ShowOpenFileDlg(int nID, CMyButton *m_BtnCurrent, CMyStatic *m_StcCurrent);
public:
    int nChannelConfig;
    bool bLFE;
    CString szInputFiles[NUM_MAX_INPUT_FILES];
    CString szTmpInputFiles[NUM_MAX_INPUT_FILES];
public:
    CMyButton m_BtnChannelFL;
    CMyButton m_BtnChannelFR;
    CMyButton m_BtnChannelFC;
    CMyButton m_BtnChannelLFE;
    CMyButton m_BtnChannelSL;
    CMyButton m_BtnChannelSR;
    CMyButton m_ChkChannelConfigLFE;
public:
    CMyStatic m_StcLabelFL;
    CMyStatic m_StcLabelFR;
    CMyStatic m_StcLabelFC;
    CMyStatic m_StcLabelLFE;
    CMyStatic m_StcLabelSL;
    CMyStatic m_StcLabelSR;
public:
    CMyStatic m_StcChannelFL;
    CMyStatic m_StcChannelFR;
    CMyStatic m_StcChannelFC;
    CMyStatic m_StcChannelLFE;
    CMyStatic m_StcChannelSL;
    CMyStatic m_StcChannelSR;
    CMyButton m_BtnChannelS;
public:
    CMyComboBox m_CmbChannelConfig;
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
