#pragma once

#include <string>
#include <memory>
#include <thread>
#include <afxwin.h>
#include <afxcmn.h>
#include "controls\MyDialogEx.h"
#include "controls\MyStatic.h"
#include "controls\MyButton.h"
#include "controls\MyProgressCtrl.h"
#include "utilities\StringHelper.h"
#include "configuration\Configuration.h"
#include "worker\Worker.h"

#define WM_FILE_TIMER WM_USER + 0x01
#define WM_TOTAL_TIMER WM_USER + 0x02

namespace app
{
    class CWorkDlg : public controls::CMyDialogEx
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
        static int nIDIn[6];
        static int nIDInInfo[6];
    public:
        config::CConfiguration * pConfig;
        std::unique_ptr<worker::CWorkerContext> pWorkerContext;
        std::thread m_Thread;
    public:
        controls::CMyStatic m_StcOut;
        controls::CMyStatic m_StcOutInfo;
        controls::CMyStatic m_StcTimeCurrent;
        controls::CMyStatic m_StcTimeTotal;
        controls::CMyProgressCtrl m_PrgCurrent;
        controls::CMyProgressCtrl m_PrgTotal;
        controls::CMyButton m_BtnCancel;
    public:
        afx_msg void OnClose();
        afx_msg void OnDestroy();
        afx_msg void OnTimer(UINT_PTR nIDEvent);
        afx_msg void OnBnClickedCancel();
    };

    class CWorkDlgWorkerContext : public worker::CWorkerContext
    {
    private:
        CWorkDlg * pWorkDlg;
    public:
        CWorkDlgWorkerContext(config::CConfiguration * pConfig, CWorkDlg* pDlg)
            : worker::CWorkerContext(pConfig), pWorkDlg(pDlg)
        {
        }
        virtual ~CWorkDlgWorkerContext()
        {
        }
    public:
        void SetTitleInfo(std::wstring szInfo)
        {
            pWorkDlg->SetWindowText(szInfo.c_str());
        }
        void SetInputFileInfo(int nID, std::wstring szInfo)
        {
            pWorkDlg->GetDlgItem(pWorkDlg->nIDIn[nID])->SetWindowText(szInfo.c_str());
        }
        void SetInputTypeInfo(int nID, std::wstring szInfo)
        {
            pWorkDlg->GetDlgItem(pWorkDlg->nIDInInfo[nID])->SetWindowText(szInfo.c_str());
        }
        void SetOutputFileInfo(std::wstring szInfo)
        {
            pWorkDlg->m_StcOut.SetWindowText(szInfo.c_str());
        }
        void SetOutputTypeInfo(std::wstring szInfo)
        {
            pWorkDlg->m_StcOutInfo.SetWindowText(szInfo.c_str());
        }
        void SetCurrentTimerInfo(std::wstring szInfo)
        {
            pWorkDlg->m_StcTimeCurrent.SetWindowText(szInfo.c_str());
        }
        void SetTotalTimerInfo(std::wstring szInfo)
        {
            pWorkDlg->m_StcTimeTotal.SetWindowText(szInfo.c_str());
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
}
