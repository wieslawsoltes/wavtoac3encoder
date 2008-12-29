//
// WAV to AC3 Encoder
// Copyright (C) 2007, 2008, 2009 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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
#include "afxcmn.h"

#include "MyButton.h"
#include "MyStatic.h"
#include "MyEdit.h"
#include "MyComboBoxEdit.h"
#include "MyListCtrl.h"
#include "MyComboBox.h"
#include "MySliderCtrl.h"

#include "EncoderOptions.h"

// return values from work thread
#define WORKDLG_RETURN_SUCCESS 0
#define WORKDLG_RETURN_FAILURE 1

// elapsed time timers message id's
#define WM_FILE_TIMER WM_USER + 0x01
#define WM_TOTAL_TIMER WM_USER + 0x02

class CEncWAVtoAC3WorkDlg;

// params used in worker thread
typedef struct
{
    CEncWAVtoAC3WorkDlg *pWorkDlg;
    CList<CString,CString> *list;
    CList<bool,bool> *listStatus;
    bool bUseOutPath;
    CString szOutPath;
    bool bParallelFileEncoding;
    bool bMultiMonoInput;
    AftenAPI api;
    int nThreads;
    double fTimeTotal;
    double fTimeEncoding;
    double fTimeIORead;
    double fTimeIOWrite;
	__int64 nInTotalSize;
	__int64 nOutTotalSize;
} WorkerParam;

class CEncWAVtoAC3WorkDlg : public CDialog
{
    DECLARE_DYNAMIC(CEncWAVtoAC3WorkDlg)
public:
    CEncWAVtoAC3WorkDlg(CWnd* pParent = NULL);
    virtual ~CEncWAVtoAC3WorkDlg();
    enum { IDD = IDD_ENCODING_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
protected:
    DECLARE_MESSAGE_MAP()
public:
    int nIDIn[NUM_MAX_INPUT_FILES];
    int nIDInInfo[NUM_MAX_INPUT_FILES];
public:
    volatile bool bTerminate;
    volatile bool bCanUpdateWindow;
    HANDLE hThread;
    DWORD dwThreadId;
    __int64 nTotalSize;
    unsigned long m_ElapsedTimeFile;
    unsigned long m_ElapsedTimeTotal;
    int nCount;
    WorkerParam workParam;
public:
    CMyStatic m_StcOut;
    CMyStatic m_StcOutInfo;
    CMyStatic m_StcSimdInfo;
    CMyStatic m_StcTimeCurrent;
    CMyStatic m_StcTimeTotal;
public:
    CProgressCtrl m_PrgCurrent;
    CProgressCtrl m_PrgTotal;
public:
    CMyListCtrl m_LstProgress;
public:
    CMyButton m_BtnCancel;
public:
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
    afx_msg LRESULT OnWorkerProgress(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedCancel();
};
