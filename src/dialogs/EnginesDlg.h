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

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "controls\MyResizeDialog.h"
#include "configuration\ProgramConfig.h"
#include "configuration\EncoderOptions.h"
#include "controls\MyResizeDialog.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"

class CEnginesDlg : public CMyResizeDialog
{
    DECLARE_DYNAMIC(CEnginesDlg)
public:
    CEnginesDlg(CWnd* pParent = NULL);
    virtual ~CEnginesDlg();
    enum { IDD = IDD_DIALOG_ENGINES };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
    ConfigList_t m_EngineList;
    CString szCurrentFileName;
public:
    volatile bool bUpdateList;
public:
    CListCtrl m_LstEngines;
    CEdit m_EdtEngineName;
    CEdit m_EdtEnginePath;
    int nCurrSel;
public:
    bool InsertProgramEngines();
    bool LoadProgramEngines(CString szFileName);
    bool SaveProgramEngines(CString szFileName);
protected:
    void InitLang();
public:
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonEnginesBrowse();
    afx_msg void OnBnClickedButtonEnginesImport();
    afx_msg void OnBnClickedButtonEnginesExport();
    afx_msg void OnBnClickedButtonEnginesAdd();
    afx_msg void OnBnClickedButtonEnginesRemove();
public:
    afx_msg void OnLvnItemchangedListEngines(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnEnChangeEditEngineName();
    afx_msg void OnEnChangeEditEnginePath();
    afx_msg void OnLvnKeydownListEngines(NMHDR *pNMHDR, LRESULT *pResult);
};
