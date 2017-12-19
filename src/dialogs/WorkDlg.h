#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include "configuration\Configuration.h"
#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"

#define WM_FILE_TIMER WM_USER + 0x01
#define WM_TOTAL_TIMER WM_USER + 0x02

class CWorkDlg;

class CWorkerParam
{
public:
    CWorkDlg *pWorkDlg;
public:
    AftenAPI api;
public:
    CListT<CString> *m_FilesList;
    CListT<bool> *m_StatusList;
public:
    CEncoderPreset *m_Preset;
public:
    bool bUseOutPath;
    CString szOutPath;
    bool bMultiMonoInput;
    int nThreads;
public:
    double fTimeTotal;
    double fTimeEncoding;
    double fTimeIORead;
    double fTimeIOWrite;
    __int64 nInTotalSize;
    __int64 nOutTotalSize;
public:
    volatile bool bTerminate;
    volatile bool bCanUpdateWindow;
    HANDLE hThread;
    DWORD dwThreadId;
    __int64 nTotalSize;
    double m_ElapsedTimeFile;
    double m_ElapsedTimeTotal;
    int nCount;
public:
    CString szSpeedEncoderAvg;
    CString szSpeedReadsAvg;
    CString szSpeedWritesAvg;
};

class CWorkDlg : public CMyDialogEx
{
    DECLARE_DYNAMIC(CWorkDlg)
public:
    CWorkDlg(CWnd* pParent = nullptr);
    virtual ~CWorkDlg();
    enum { IDD = IDD_DIALOG_WORK };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
protected:
    void InitSettings();
    void InitCtrls();
    void InitLang();
    void CreateWorker();
    void UpdateFileTimer();
    void UpdateTotalTimer();
protected:
    DECLARE_MESSAGE_MAP()
public:
    static int nIDIn[CEncoderDefaults::nNumMaxInputFiles];
    static int nIDInInfo[CEncoderDefaults::nNumMaxInputFiles];
public:
    CWorkerParam m_WorkerParam;
public:
    CMyStatic m_StcOut;
    CMyStatic m_StcOutInfo;
    CMyStatic m_StcSimdInfo;
    CMyStatic m_StcTimeCurrent;
    CMyStatic m_StcTimeTotal;
    CProgressCtrl m_PrgCurrent;
    CProgressCtrl m_PrgTotal;
    CMyButton m_BtnCancel;
public:
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedCancel();
};
