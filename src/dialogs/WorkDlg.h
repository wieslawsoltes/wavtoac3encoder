#pragma once

#include <afxwin.h>
#include <afxcmn.h>
#include "controls\MyDialogEx.h"
#include "controls\MyStatic.h"
#include "controls\MyButton.h"
#include "controls\MyProgressCtrl.h"
#include "configuration\Configuration.h"
#include "worker\Worker.h"

#define WM_FILE_TIMER WM_USER + 0x01
#define WM_TOTAL_TIMER WM_USER + 0x02

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
    CWorkerContext *pWorkerContext;
public:
    CMyStatic m_StcOut;
    CMyStatic m_StcOutInfo;
    CMyStatic m_StcSimdInfo;
    CMyStatic m_StcTimeCurrent;
    CMyStatic m_StcTimeTotal;
    CMyProgressCtrl m_PrgCurrent;
    CMyProgressCtrl m_PrgTotal;
    CMyButton m_BtnCancel;
public:
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedCancel();
};

class CWorkDlgWorkerContext : public CWorkerContext
{
private:
    CWorkDlg * pWorkDlg;
public:
    CWorkDlgWorkerContext(CWorkDlg* pDlg)
        : CWorkerContext()
    {
        this->pWorkDlg = pDlg;
    }
    virtual ~CWorkDlgWorkerContext()
    {
    }
public:
    void SetTitleInfo(CString szInfo)
    {
        pWorkDlg->SetWindowText(szInfo);
    }
    void SetInputFileInfo(int nID, CString szInfo)
    {
        pWorkDlg->GetDlgItem(pWorkDlg->nIDIn[nID])->SetWindowText(szInfo);
    }
    void SetInputTypeInfo(int nID, CString szInfo)
    {
        pWorkDlg->GetDlgItem(pWorkDlg->nIDInInfo[nID])->SetWindowText(szInfo);
    }
    void SetOutputFileInfo(CString szInfo)
    {
        pWorkDlg->m_StcOut.SetWindowText(szInfo);
    }
    void SetOutputTypeInfo(CString szInfo)
    {
        pWorkDlg->m_StcOutInfo.SetWindowText(szInfo);
    }
    void SetSimdInfo(CString szInfo)
    {
        pWorkDlg->m_StcSimdInfo.SetWindowText(szInfo);
    }
    void SetCurrentTimerInfo(CString szInfo)
    {
        pWorkDlg->m_StcTimeCurrent.SetWindowText(szInfo);
    }
    void SetTotalTimerInfo(CString szInfo)
    {
        pWorkDlg->m_StcTimeTotal.SetWindowText(szInfo);
    }
public:
    void SetCurrentProgressRange(int nMin, int nMax)
    {
        pWorkDlg->m_PrgCurrent.SetRange(nMin, nMax);
    }
    void SetTotalProgressRange(int nMin, int nMax)
    {
        pWorkDlg->m_PrgTotal.SetRange32(nMin, nMax);
    }
    void SetCurrentProgress(int nPos)
    {
        pWorkDlg->m_PrgCurrent.SetPos(nPos);
    }
    void SetTotalProgress(int nPos)
    {
        pWorkDlg->m_PrgTotal.SetPos(nPos);
    }
public:
    void StartCurrentTimer(int nResolution)
    {
        pWorkDlg->SetTimer(WM_FILE_TIMER, nResolution, nullptr);
    }
    void StopCurrentTimer()
    {
        pWorkDlg->KillTimer(WM_FILE_TIMER);
    }
    void StartTotalTimer(int nResolution)
    {
        pWorkDlg->SetTimer(WM_TOTAL_TIMER, nResolution, nullptr);
    }
    void StopTotalTimer()
    {
        pWorkDlg->KillTimer(WM_TOTAL_TIMER);
    }
public:
    void Close()
    {
        ::PostMessage(pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
    }
};
