#include "StdAfx.h"
#include "MainApp.h"
#include "WorkDlg.h"
#include "utilities\Utilities.h"

namespace app
{
    int CWorkDlg::nIDIn[config::CEncoderDefaults::nNumMaxInputFiles]
    {
        IDC_STATIC_IN_00,
        IDC_STATIC_IN_01,
        IDC_STATIC_IN_02,
        IDC_STATIC_IN_03,
        IDC_STATIC_IN_04,
        IDC_STATIC_IN_05
    };

    int CWorkDlg::nIDInInfo[config::CEncoderDefaults::nNumMaxInputFiles]
    {
        IDC_STATIC_IN_INFO_00,
        IDC_STATIC_IN_INFO_01,
        IDC_STATIC_IN_INFO_02,
        IDC_STATIC_IN_INFO_03,
        IDC_STATIC_IN_INFO_04,
        IDC_STATIC_IN_INFO_05
    };

    IMPLEMENT_DYNAMIC(CWorkDlg, CDialog)
    CWorkDlg::CWorkDlg(CWnd* pParent /*=nullptr*/)
        : CMyDialogEx(CWorkDlg::IDD, pParent)
    {
        pWorkerContext = new CWorkDlgWorkerContext(this);
        pWorkerContext->pConfig = &m_Config;
        pWorkerContext->bTerminate = false;
        pWorkerContext->bCanUpdateWindow = true;
        pWorkerContext->hThread = nullptr;
        pWorkerContext->dwThreadId = 0;
        pWorkerContext->nCount = 0;
        pWorkerContext->m_ElapsedTimeFile = 0;
        pWorkerContext->m_ElapsedTimeTotal = 0;
    }

    CWorkDlg::~CWorkDlg()
    {
        if (pWorkerContext != nullptr)
            delete pWorkerContext;
    }

    void CWorkDlg::DoDataExchange(CDataExchange* pDX)
    {
        CMyDialogEx::DoDataExchange(pDX);
        DDX_Control(pDX, IDCANCEL, m_BtnCancel);
        DDX_Control(pDX, IDC_STATIC_ELAPSED_CURRENT, m_StcTimeCurrent);
        DDX_Control(pDX, IDC_STATIC_ELAPSED_TOTAL, m_StcTimeTotal);
        DDX_Control(pDX, IDC_STATIC_OUT, m_StcOut);
        DDX_Control(pDX, IDC_STATIC_OUT_INFO, m_StcOutInfo);
        DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_PrgCurrent);
        DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_PrgTotal);
    }

    BEGIN_MESSAGE_MAP(CWorkDlg, CMyDialogEx)
        ON_WM_CLOSE()
        ON_WM_DESTROY()
        ON_WM_TIMER()
        ON_BN_CLICKED(IDCANCEL, &CWorkDlg::OnBnClickedCancel)
    END_MESSAGE_MAP()

    BOOL CWorkDlg::OnInitDialog()
    {
        CMyDialogEx::OnInitDialog();

        InitCtrls();
        InitLang();
        CreateWorker();

        return TRUE;
    }

    void CWorkDlg::OnClose()
    {
        KillTimer(WM_FILE_TIMER);
        KillTimer(WM_TOTAL_TIMER);

        if (pWorkerContext->bTerminate == false)
        {
            pWorkerContext->bTerminate = true;
            return;
        }
        else
            this->EndDialog(IDOK);

        CMyDialogEx::OnClose();
    }

    void CWorkDlg::OnDestroy()
    {
        CMyDialogEx::OnDestroy();

        if (pWorkerContext->hThread != nullptr)
        {
            ::TerminateThread(pWorkerContext->hThread, 0);
            ::CloseHandle(pWorkerContext->hThread);
            pWorkerContext->hThread = nullptr;
        }
    }

    void CWorkDlg::OnTimer(UINT_PTR nIDEvent)
    {
        switch (nIDEvent)
        {
        case WM_TOTAL_TIMER:
            UpdateTotalTimer();
            return;
        case WM_FILE_TIMER:
            UpdateFileTimer();
            return;
        };

        CMyDialogEx::OnTimer(nIDEvent);
    }

    void CWorkDlg::OnBnClickedCancel()
    {
        if (pWorkerContext->bTerminate == false)
            pWorkerContext->bTerminate = true;
        else
            this->EndDialog(IDOK);
    }

    void CWorkDlg::InitCtrls()
    {
        if (pWorkerContext->bMultiMonoInput == false)
        {
            for (int i = 1; i < config::CEncoderDefaults::nNumMaxInputFiles; i++)
            {
                this->GetDlgItem(nIDIn[i])->ShowWindow(SW_HIDE);
                this->GetDlgItem(nIDInInfo[i])->ShowWindow(SW_HIDE);
            }

            CRect rcIn[config::CEncoderDefaults::nNumMaxInputFiles], rcInInfo[config::CEncoderDefaults::nNumMaxInputFiles];
            CRect rcOut, rcOutInfo;
            CRect rcElapsed[2];
            CRect rcProgress[2];
            CRect rcGroup, rcBtnCancel;
            CRect rcDlg;

            for (int i = 0; i < config::CEncoderDefaults::nNumMaxInputFiles; i++)
            {
                this->GetDlgItem(this->nIDIn[i])->GetWindowRect(rcIn[i]);
                this->GetDlgItem(this->nIDInInfo[i])->GetWindowRect(rcInInfo[i]);
            }

            this->m_StcOut.GetWindowRect(rcOut);
            this->m_StcOutInfo.GetWindowRect(rcOutInfo);
            this->m_StcTimeCurrent.GetWindowRect(rcElapsed[0]);
            this->m_StcTimeTotal.GetWindowRect(rcElapsed[1]);
            this->m_PrgCurrent.GetWindowRect(rcProgress[0]);
            this->m_PrgTotal.GetWindowRect(rcProgress[1]);
            this->GetDlgItem(IDC_STATIC_GROUP_ENCODING)->GetWindowRect(rcGroup);
            this->m_BtnCancel.GetWindowRect(rcBtnCancel);

            this->GetWindowRect(rcDlg);

            int nHeight = 0;
            nHeight = abs(rcInInfo[1].top - rcOutInfo.top);

            rcOut.MoveToY(rcOut.top - nHeight);
            this->ScreenToClient(rcOut);
            this->m_StcOut.MoveWindow(rcOut);

            rcOutInfo.MoveToY(rcOutInfo.top - nHeight);
            this->ScreenToClient(rcOutInfo);
            this->m_StcOutInfo.MoveWindow(rcOutInfo);

            rcElapsed[0].MoveToY(rcElapsed[0].top - nHeight);
            this->ScreenToClient(rcElapsed[0]);
            this->m_StcTimeCurrent.MoveWindow(rcElapsed[0]);

            rcElapsed[1].MoveToY(rcElapsed[1].top - nHeight);
            this->ScreenToClient(rcElapsed[1]);
            this->m_StcTimeTotal.MoveWindow(rcElapsed[1]);

            rcProgress[0].MoveToY(rcProgress[0].top - nHeight);
            this->ScreenToClient(rcProgress[0]);
            this->m_PrgCurrent.MoveWindow(rcProgress[0]);

            rcProgress[1].MoveToY(rcProgress[1].top - nHeight);
            this->ScreenToClient(rcProgress[1]);
            this->m_PrgTotal.MoveWindow(rcProgress[1]);

            rcBtnCancel.MoveToY(rcBtnCancel.top - nHeight);
            this->ScreenToClient(rcBtnCancel);
            this->m_BtnCancel.MoveWindow(rcBtnCancel);

            rcGroup.bottom -= nHeight;
            this->ScreenToClient(rcGroup);
            this->GetDlgItem(IDC_STATIC_GROUP_ENCODING)->MoveWindow(rcGroup);

            rcDlg.bottom -= nHeight;
            this->MoveWindow(rcDlg);
        }
    }

    void CWorkDlg::UpdateTotalTimer()
    {
        TCHAR strTime[32] = _T("");
        pWorkerContext->m_ElapsedTimeTotal += 0.25;

        if (pWorkerContext->m_ElapsedTimeTotal <= 59)
        {
            _stprintf(strTime, _T("%s 00:00:%02u\0"),
                m_Config.HaveLangStrings() ? (LPCTSTR)m_Config.GetLangString(0x00A01006).c_str() : _T("Total elapsed time:"),
                (unsigned long)pWorkerContext->m_ElapsedTimeTotal);
        }
        else if (pWorkerContext->m_ElapsedTimeTotal <= 3599)
        {
            _stprintf(strTime, _T("%s 00:%02u:%02u\0"),
                m_Config.HaveLangStrings() ? (LPCTSTR)m_Config.GetLangString(0x00A01006).c_str() : _T("Total elapsed time:"),
                ((unsigned long)pWorkerContext->m_ElapsedTimeTotal / 60),
                ((unsigned long)pWorkerContext->m_ElapsedTimeTotal % 60));
        }
        else
        {
            _stprintf(strTime, _T("%s %02u:%02u:%02u\0"),
                m_Config.HaveLangStrings() ? (LPCTSTR)m_Config.GetLangString(0x00A01006).c_str() : _T("Total elapsed time:"),
                ((unsigned long)pWorkerContext->m_ElapsedTimeTotal / 60) / 60,
                ((unsigned long)pWorkerContext->m_ElapsedTimeTotal / 60) % 60,
                ((((unsigned long)pWorkerContext->m_ElapsedTimeTotal / 60) % 60) * 60) % 60);
        }

        if (pWorkerContext->bCanUpdateWindow == true)
        {
            pWorkerContext->bCanUpdateWindow = false;
            m_StcTimeTotal.SetWindowText(strTime);
            pWorkerContext->bCanUpdateWindow = true;
        }
    }

    void CWorkDlg::UpdateFileTimer()
    {
        TCHAR strTime[32] = _T("");
        pWorkerContext->m_ElapsedTimeFile += 0.25;

        if (pWorkerContext->m_ElapsedTimeFile <= 59)
        {
            _stprintf(strTime, _T("%s 00:00:%02u\0"),
                m_Config.HaveLangStrings() ? (LPCTSTR)m_Config.GetLangString(0x00A01005).c_str() : _T("Elapsed time:"),
                (unsigned long)pWorkerContext->m_ElapsedTimeFile);
        }
        else if (pWorkerContext->m_ElapsedTimeFile <= 3599)
        {
            _stprintf(strTime, _T("%s 00:%02u:%02u\0"),
                m_Config.HaveLangStrings() ? (LPCTSTR)m_Config.GetLangString(0x00A01005).c_str() : _T("Elapsed time:"),
                ((unsigned long)pWorkerContext->m_ElapsedTimeFile / 60),
                ((unsigned long)pWorkerContext->m_ElapsedTimeFile % 60));
        }
        else
        {
            _stprintf(strTime, _T("%s %02u:%02u:%02u\0"),
                m_Config.HaveLangStrings() ? (LPCTSTR)m_Config.GetLangString(0x00A01005).c_str() : _T("Elapsed time:"),
                ((unsigned long)pWorkerContext->m_ElapsedTimeFile / 60) / 60,
                ((unsigned long)pWorkerContext->m_ElapsedTimeFile / 60) % 60,
                ((((unsigned long)pWorkerContext->m_ElapsedTimeFile / 60) % 60) * 60) % 60);
        }

        if (pWorkerContext->bCanUpdateWindow == true)
        {
            pWorkerContext->bCanUpdateWindow = false;
            m_StcTimeCurrent.SetWindowText(strTime);
            pWorkerContext->bCanUpdateWindow = true;
        }
    }

    void CWorkDlg::CreateWorker()
    {
        pWorkerContext->hThread = ::CreateThread(nullptr,
            0,
            worker::EncWorkThread,
            pWorkerContext,
            0,
            &pWorkerContext->dwThreadId);

        if (pWorkerContext->hThread == nullptr)
        {
            OutputDebugString(_T("Error: Failed to create worker thread!"));
            this->MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00A0100B).c_str() : _T("Failed to create worker thread!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00A0100A).c_str() : _T("Fatal Error"),
                MB_OK | MB_ICONERROR);
        }
    }

    void CWorkDlg::InitLang()
    {
        if (m_Config.HaveLangStrings())
        {
            this->SetWindowText(m_Config.GetLangString(0x00A01001).c_str());
            this->GetDlgItem(IDCANCEL)->SetWindowText(m_Config.GetLangString(0x00A01002).c_str());
        }
    }
}
