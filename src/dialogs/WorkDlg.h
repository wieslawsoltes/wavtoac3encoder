﻿#pragma once

#define WM_FILE_TIMER WM_USER + 0x01
#define WM_TOTAL_TIMER WM_USER + 0x02

namespace dialogs
{
    class CWorkDlg : public controls::CMyDialogEx
    {
        DECLARE_DYNAMIC(CWorkDlg)
    public:
        CWorkDlg(CWnd* pParent = nullptr);
        virtual ~CWorkDlg();
        enum { IDD = IDD_DIALOG_WORK };
    public:
        controls::CMyStatic m_StcOut;
        controls::CMyStatic m_StcOutInfo;
        controls::CMyStatic m_StcTimeCurrent;
        controls::CMyStatic m_StcTimeTotal;
        controls::CMyProgressCtrl m_PrgCurrent;
        controls::CMyProgressCtrl m_PrgTotal;
        controls::CMyButton m_BtnCancel;
    public:
        static int nIDIn[6];
        static int nIDInInfo[6];
        config::CConfiguration * pConfig;
        std::unique_ptr<worker::CWorkerContext> pWorkerContext;
        std::thread m_Thread;
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);
        DECLARE_MESSAGE_MAP()
    public:
        virtual BOOL OnInitDialog();
    protected:
        void InitLang();
        void HideCtrls();
        void FormatTime(double fTime, TCHAR szBuffer[32], int nPrefixKey);
        void UpdateFileTimer();
        void UpdateTotalTimer();
        void CreateWorker();
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
        CWorkDlgWorkerContext(CWorkDlg* pDlg) : pWorkDlg(pDlg) { }
        virtual ~CWorkDlgWorkerContext() { }
    private:
        bool IsValid()
        {
            return (this->bTerminate == false) 
                && (pWorkDlg != nullptr) 
                && (::IsWindow(pWorkDlg->GetSafeHwnd()) == TRUE);
        }
    public:
        void SetTitleInfo(std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->SetWindowText(szInfo.c_str());
            }
        }
        void SetInputFileInfo(int nID, std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->GetDlgItem(pWorkDlg->nIDIn[nID])->SetWindowText(szInfo.c_str());
            }
        }
        void SetInputTypeInfo(int nID, std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->GetDlgItem(pWorkDlg->nIDInInfo[nID])->SetWindowText(szInfo.c_str());
            }
        }
        void SetOutputFileInfo(std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->m_StcOut.SetWindowText(szInfo.c_str());
            }
        }
        void SetOutputTypeInfo(std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->m_StcOutInfo.SetWindowText(szInfo.c_str());
            }
        }
        void SetCurrentTimerInfo(std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->m_StcTimeCurrent.SetWindowText(szInfo.c_str());
            }
        }
        void SetTotalTimerInfo(std::wstring szInfo)
        {
            if (IsValid())
            {
                pWorkDlg->m_StcTimeTotal.SetWindowText(szInfo.c_str());
            }
        }
    public:
        void SetCurrentProgressRange(int nMin, int nMax)
        {
            if (IsValid())
            {
                pWorkDlg->m_PrgCurrent.SetRange(nMin, nMax);
                pWorkDlg->m_PrgCurrent.SetPos(0);
            }
        }
        void SetTotalProgressRange(int nMin, int nMax)
        {
            if (IsValid())
            {
                pWorkDlg->m_PrgTotal.SetRange32(nMin, nMax);
                pWorkDlg->m_PrgTotal.SetPos(0);
            }
        }
        void SetCurrentProgress(int nPos)
        {
            if (IsValid())
            {
                pWorkDlg->m_PrgCurrent.SetPos(nPos);
            }
        }
        void SetTotalProgress(int nPos)
        {
            if (IsValid())
            {
                pWorkDlg->m_PrgTotal.SetPos(nPos);
            }
        }
    public:
        void StartCurrentTimer(int nResolution)
        {
            if (IsValid())
            {
                pWorkDlg->SetTimer(WM_FILE_TIMER, nResolution, nullptr);
            }
        }
        void StopCurrentTimer()
        {
            if (IsValid())
            {
                pWorkDlg->KillTimer(WM_FILE_TIMER);
            }
        }
        void StartTotalTimer(int nResolution)
        {
            if (IsValid())
            {
                pWorkDlg->SetTimer(WM_TOTAL_TIMER, nResolution, nullptr);
            }
        }
        void StopTotalTimer()
        {
            if (IsValid())
            {
                pWorkDlg->KillTimer(WM_TOTAL_TIMER);
            }
        }
    public:
        void Close()
        {
            ::PostMessage(pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
        }
    };
}
