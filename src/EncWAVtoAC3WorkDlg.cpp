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

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "EncWAVtoAC3WorkDlg.h"
#include "EncoderOptions.h"
#include "Utilities.h"
#include "MyFile.h"
#include "EncWorkThread.h"

IMPLEMENT_DYNAMIC(CEncWAVtoAC3WorkDlg, CDialog)

CEncWAVtoAC3WorkDlg::CEncWAVtoAC3WorkDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEncWAVtoAC3WorkDlg::IDD, pParent)
{
    nIDIn[0] = IDC_STATIC_IN_00;
    nIDIn[1] = IDC_STATIC_IN_01;
    nIDIn[2] = IDC_STATIC_IN_02;
    nIDIn[3] = IDC_STATIC_IN_03;
    nIDIn[4] = IDC_STATIC_IN_04;
    nIDIn[5] = IDC_STATIC_IN_05;

    nIDInInfo[0] = IDC_STATIC_IN_INFO_00;
    nIDInInfo[1] = IDC_STATIC_IN_INFO_01;
    nIDInInfo[2] = IDC_STATIC_IN_INFO_02;
    nIDInInfo[3] = IDC_STATIC_IN_INFO_03;
    nIDInInfo[4] = IDC_STATIC_IN_INFO_04;
    nIDInInfo[5] = IDC_STATIC_IN_INFO_05;

    this->bTerminate = false;
    this->bCanUpdateWindow = true;
    this->hThread = NULL;
    this->dwThreadId = 0;
    this->nCount = 0;
    this->m_ElapsedTimeFile = 0;
    this->m_ElapsedTimeTotal = 0;
}

CEncWAVtoAC3WorkDlg::~CEncWAVtoAC3WorkDlg()
{

}

void CEncWAVtoAC3WorkDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDCANCEL, m_BtnCancel);
    DDX_Control(pDX, IDC_STATIC_ELAPSED_CURRENT, m_StcTimeCurrent);
    DDX_Control(pDX, IDC_STATIC_ELAPSED_TOTAL, m_StcTimeTotal);
    DDX_Control(pDX, IDC_STATIC_OUT, m_StcOut);
    DDX_Control(pDX, IDC_STATIC_OUT_INFO, m_StcOutInfo);
    DDX_Control(pDX, IDC_STATIC_SIMD_INFO, m_StcSimdInfo);
    DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_PrgCurrent);
    DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_PrgTotal);
    DDX_Control(pDX, IDC_LIST_MT_PROGRESS, m_LstProgress);
}

BEGIN_MESSAGE_MAP(CEncWAVtoAC3WorkDlg, CDialog)
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDCANCEL, &CEncWAVtoAC3WorkDlg::OnBnClickedCancel)
    ON_MESSAGE(WM_WORKER_PROGRESS, OnWorkerProgress)
END_MESSAGE_MAP()

BOOL CEncWAVtoAC3WorkDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // setup worker thread settings
    workParam.pWorkDlg = this;
    workParam.bParallelFileEncoding = GetCurrentPreset().bParallelFileEncoding;

    if(workParam.bParallelFileEncoding == false)
    {
        // hide progress list because it's unused for single thread
        m_LstProgress.ShowWindow(SW_HIDE);

        if(this->workParam.bMultiMonoInput == false)
        {
            // hide unused static controls
            for(int i = 1; i < 6; i++)
            {
                this->GetDlgItem(nIDIn[i])->ShowWindow(SW_HIDE);
                this->GetDlgItem(nIDInInfo[i])->ShowWindow(SW_HIDE);
            }

            // move other controls
            CRect rcIn[NUM_MAX_INPUT_FILES], rcInInfo[NUM_MAX_INPUT_FILES];
            CRect rcOut, rcOutInfo;
            CRect rcSIMD;
            CRect rcElapsed[2];
            CRect rcProgress[2];
            CRect rcGroup, rcBtnCancel;
            CRect rcDlg;

            for(int i = 0; i < NUM_MAX_INPUT_FILES; i++)
            {
                this->GetDlgItem(this->nIDIn[i])->GetWindowRect(rcIn[i]);
                this->GetDlgItem(this->nIDInInfo[i])->GetWindowRect(rcInInfo[i]);
            }

            this->m_StcOut.GetWindowRect(rcOut);
            this->m_StcOutInfo.GetWindowRect(rcOutInfo);
            this->m_StcSimdInfo.GetWindowRect(rcSIMD);
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

            rcSIMD.MoveToY(rcSIMD.top - nHeight);
            this->ScreenToClient(rcSIMD);
            this->m_StcSimdInfo.MoveWindow(rcSIMD);

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

        // create  worker thread
        this->hThread = ::CreateThread(NULL, 
            0, 
            EncWorkThread, 
            &workParam, 
            0, 
            &this->dwThreadId);
    }
    else if(workParam.bParallelFileEncoding == true)
    {
        // hide current file progress because it's unused for MT
        for(int i = 0; i < 6; i++)
        {
            this->GetDlgItem(nIDIn[i])->ShowWindow(SW_HIDE);
            this->GetDlgItem(nIDInInfo[i])->ShowWindow(SW_HIDE);
        }

        this->m_StcOut.ShowWindow(SW_HIDE);
        this->m_StcOutInfo.ShowWindow(SW_HIDE);
        this->m_StcTimeCurrent.ShowWindow(SW_HIDE);
        this->m_StcTimeTotal.ShowWindow(SW_HIDE);
        this->m_StcSimdInfo.ShowWindow(SW_HIDE);
        this->m_PrgCurrent.ShowWindow(SW_HIDE);
        this->m_PrgTotal.ShowWindow(SW_HIDE);

        // add progress list columns
        this->m_LstProgress.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        this->m_LstProgress.InsertColumn(0, _T("#"), LVCFMT_LEFT, 28, 0);
        this->m_LstProgress.InsertColumn(1, _T("From"), LVCFMT_LEFT, 195, 0);
        this->m_LstProgress.InsertColumn(2, _T("To"), LVCFMT_LEFT, 195, 0);
        this->m_LstProgress.InsertColumn(3, _T("Progress"), LVCFMT_LEFT, 110, 0);

        // create MT worker thread
        this->hThread = ::CreateThread(NULL, 
            0, 
            EncWorkThreadMT, 
            &workParam, 
            0, 
            &this->dwThreadId);
    }

    if(this->hThread == NULL)
    {
        // show critical error message
        this->MessageBox(_T("Failed to create worker thread!"), 
            _T("Fatal Error"), 
            MB_OK | MB_ICONERROR);
    }

    return TRUE;
}

void CEncWAVtoAC3WorkDlg::OnClose()
{
    // stop file timer
    KillTimer(WM_FILE_TIMER);

    // stop total timer
    KillTimer(WM_TOTAL_TIMER);

    // stop encoding process and close work dialog
    if(this->bTerminate == false)
    {
        this->bTerminate = true;
        return;
    }
    else
        this->EndDialog(IDOK);

    CDialog::OnClose();
}

void CEncWAVtoAC3WorkDlg::OnDestroy()
{
    CDialog::OnDestroy();

    if(this->hThread != NULL)
    {
        ::TerminateThread(this->hThread, 0);
        ::CloseHandle(this->hThread);
        this->hThread = NULL;
    }
}

void CEncWAVtoAC3WorkDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case WM_TOTAL_TIMER:
        {
            TCHAR strTime[32] = _T("");
            m_ElapsedTimeTotal += 1L;

            // get time in format hh:mm:ss
            if(m_ElapsedTimeTotal <= 59)
            {
                _stprintf(strTime, _T("Total elapsed time: 00:00:%02u\0"), 
                    m_ElapsedTimeTotal); // ss
            }
            else if(m_ElapsedTimeTotal <= 3599)
            {
                _stprintf(strTime, _T("Total elapsed time: 00:%02u:%02u\0"),
                    (m_ElapsedTimeTotal / 60), // mm
                    (m_ElapsedTimeTotal % 60)); // ss
            }
            else
            {
                _stprintf(strTime, _T("Total elapsed time: %02u:%02u:%02u\0"), 
                    (m_ElapsedTimeTotal / 60) / 60, // hh
                    (m_ElapsedTimeTotal / 60) % 60, // mm
                    (((m_ElapsedTimeTotal / 60) % 60) * 60) % 60); // ss
            }

            // write to dialog
            if(this->bCanUpdateWindow == true)
            {
                this->bCanUpdateWindow = false;
                m_StcTimeTotal.SetWindowText(strTime);
                this->bCanUpdateWindow = true;
            }

            return;
        }
        break;

    case WM_FILE_TIMER:
        {
            TCHAR strTime[32] = _T("");
            m_ElapsedTimeFile += 1L;

            // get time in format hh:mm:ss
            if(m_ElapsedTimeFile <= 59)
            {
                _stprintf(strTime, _T("Elapsed time: 00:00:%02u\0"), 
                    m_ElapsedTimeFile); // ss
            }
            else if(m_ElapsedTimeFile <= 3599)
            {
                _stprintf(strTime, _T("Elapsed time: 00:%02u:%02u\0"),
                    (m_ElapsedTimeFile / 60), // mm
                    (m_ElapsedTimeFile % 60)); // ss
            }
            else
            {
                _stprintf(strTime, _T("Elapsed time: %02u:%02u:%02u\0"), 
                    (m_ElapsedTimeFile / 60) / 60, // hh
                    (m_ElapsedTimeFile / 60) % 60, // mm
                    (((m_ElapsedTimeFile / 60) % 60) * 60) % 60); // ss
            }

            // write to dialog
            if(this->bCanUpdateWindow == true)
            {
                this->bCanUpdateWindow = false;
                m_StcTimeCurrent.SetWindowText(strTime);
                this->bCanUpdateWindow = true;
            }

            return;
        }
        break;
    };

    CDialog::OnTimer(nIDEvent);
}

LRESULT CEncWAVtoAC3WorkDlg::OnWorkerProgress(WPARAM wParam, LPARAM lParam)
{
    // add item to list
    if((lParam == (LPARAM) 1) && (wParam != NULL))
    {
         SingleWorkerData *pworkData = (SingleWorkerData *) wParam;
         CString szBuff;
         int nID = pworkData->nID;

         szBuff.Format(_T("%d"), (nID + 1));
         this->m_LstProgress.InsertItem(nID, szBuff);

         this->m_LstProgress.SetItemText(nID, 1, GetFileName(pworkData->szInPath));
         this->m_LstProgress.SetItemText(nID, 2, GetFileName(pworkData->szOutPath));
         this->m_LstProgress.SetItemText(nID, 3, _T("0%"));

        this->m_LstProgress.EnsureVisible(nID, FALSE);
    }
    // update progress bar
    if((lParam == (LPARAM) 2) && (wParam != NULL))
    {
        SingleWorkerData *pworkData = (SingleWorkerData *) wParam;
        CString szBuff;
        int nProgress = pworkData->nProgress;
        int nID = pworkData->nID;

        szBuff.Format(_T("%d%%"), nProgress);
        this->m_LstProgress.SetItemText(nID, 3, szBuff);
    }
    // show encoding time
    if((lParam == (LPARAM) 3) && (wParam != NULL))
    {
        SingleWorkerData *pworkData = (SingleWorkerData *) wParam;
        CString szBuff;
        double fEncTime = pworkData->fEncTime;
        int nID = pworkData->nID;

        szBuff.Format(_T("Done in %0.3f sec"), fEncTime);
        this->m_LstProgress.SetItemText(nID, 3, szBuff);
    }

    return(0);
}

void CEncWAVtoAC3WorkDlg::OnBnClickedCancel()
{
    // stop encoding process and close work dialog
    if(this->bTerminate == false)
    {
        this->bTerminate = true;
    }
    else
    {
        this->EndDialog(IDOK);
    }

    // OnCancel();
}
