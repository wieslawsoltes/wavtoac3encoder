//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "EncWAVtoAC3WorkDlg.h"
#include "EncoderOptions.h"
#include "Utilities.h"
#include "MyFile.h"
#include "EncWorkThread.h"

IMPLEMENT_DYNAMIC(CEncWAVtoAC3WorkDlg, CDialogEx)

CEncWAVtoAC3WorkDlg::CEncWAVtoAC3WorkDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CEncWAVtoAC3WorkDlg::IDD, pParent)
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
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDCANCEL, m_BtnCancel);
    DDX_Control(pDX, IDC_STATIC_ELAPSED_CURRENT, m_StcTimeCurrent);
    DDX_Control(pDX, IDC_STATIC_ELAPSED_TOTAL, m_StcTimeTotal);
    DDX_Control(pDX, IDC_STATIC_OUT, m_StcOut);
    DDX_Control(pDX, IDC_STATIC_OUT_INFO, m_StcOutInfo);
    DDX_Control(pDX, IDC_STATIC_SIMD_INFO, m_StcSimdInfo);
    DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_PrgCurrent);
    DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_PrgTotal);
}

BEGIN_MESSAGE_MAP(CEncWAVtoAC3WorkDlg, CDialogEx)
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDCANCEL, &CEncWAVtoAC3WorkDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL CEncWAVtoAC3WorkDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

	InitSettings();
	InitCtrls();
	InitLang();
	CreateWorker();

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

    CDialogEx::OnClose();
}

void CEncWAVtoAC3WorkDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

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
		UpdateTotalTimer();
		return;
    case WM_FILE_TIMER: 
		UpdateFileTimer(); 
		return;
    };

    CDialogEx::OnTimer(nIDEvent);
}

void CEncWAVtoAC3WorkDlg::OnBnClickedCancel()
{
    // stop encoding process and close work dialog
    if(this->bTerminate == false)
        this->bTerminate = true;
    else
        this->EndDialog(IDOK);

    // OnCancel();
}

void CEncWAVtoAC3WorkDlg::InitSettings()
{
	workParam.pWorkDlg = this;
}

void CEncWAVtoAC3WorkDlg::InitCtrls()
{
	if (this->workParam.bMultiMonoInput == false)
	{
		// hide unused static controls
		for (int i = 1; i < 6; i++)
		{
			this->GetDlgItem(nIDIn[i])->ShowWindow(SW_HIDE);
			this->GetDlgItem(nIDInInfo[i])->ShowWindow(SW_HIDE);
		}

		// move other controls
		CRect rcIn[NUM_MAX_INPUT_FILES], rcInInfo[NUM_MAX_INPUT_FILES];
		CRect rcSpeedInfo[3][3];
		CRect rcOut, rcOutInfo;
		CRect rcSIMD;
		CRect rcElapsed[2];
		CRect rcProgress[2];
		CRect rcGroup, rcGroupStats, rcBtnCancel;
		CRect rcDlg;

		for (int i = 0; i < NUM_MAX_INPUT_FILES; i++)
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

		this->GetDlgItem(IDC_STATIC_ENCODER_LABEL)->GetWindowRect(rcSpeedInfo[0][0]);
		this->GetDlgItem(IDC_STATIC_ENCODER_UNIT)->GetWindowRect(rcSpeedInfo[0][1]);
		this->GetDlgItem(IDC_STATIC_ENCODER_SPEED_AVG)->GetWindowRect(rcSpeedInfo[0][2]);

		this->GetDlgItem(IDC_STATIC_READS_LABEL)->GetWindowRect(rcSpeedInfo[1][0]);
		this->GetDlgItem(IDC_STATIC_READS_UNIT)->GetWindowRect(rcSpeedInfo[1][1]);
		this->GetDlgItem(IDC_STATIC_READS_SPEED_AVG)->GetWindowRect(rcSpeedInfo[1][2]);

		this->GetDlgItem(IDC_STATIC_WRITES_LABEL)->GetWindowRect(rcSpeedInfo[2][0]);
		this->GetDlgItem(IDC_STATIC_WRITES_UNIT)->GetWindowRect(rcSpeedInfo[2][1]);
		this->GetDlgItem(IDC_STATIC_WRITES_SPEED_AVG)->GetWindowRect(rcSpeedInfo[2][2]);

		this->GetDlgItem(IDC_STATIC_GROUP_STATS)->GetWindowRect(rcGroupStats);
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

		rcSpeedInfo[0][0].MoveToY(rcSpeedInfo[0][0].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[0][0]);
		this->GetDlgItem(IDC_STATIC_ENCODER_LABEL)->MoveWindow(rcSpeedInfo[0][0]);

		rcSpeedInfo[0][1].MoveToY(rcSpeedInfo[0][1].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[0][1]);
		this->GetDlgItem(IDC_STATIC_ENCODER_UNIT)->MoveWindow(rcSpeedInfo[0][1]);

		rcSpeedInfo[0][2].MoveToY(rcSpeedInfo[0][2].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[0][2]);
		this->GetDlgItem(IDC_STATIC_ENCODER_SPEED_AVG)->MoveWindow(rcSpeedInfo[0][2]);

		rcSpeedInfo[1][0].MoveToY(rcSpeedInfo[1][0].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[1][0]);
		this->GetDlgItem(IDC_STATIC_READS_LABEL)->MoveWindow(rcSpeedInfo[1][0]);

		rcSpeedInfo[1][1].MoveToY(rcSpeedInfo[1][1].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[1][1]);
		this->GetDlgItem(IDC_STATIC_READS_UNIT)->MoveWindow(rcSpeedInfo[1][1]);

		rcSpeedInfo[1][2].MoveToY(rcSpeedInfo[1][2].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[1][2]);
		this->GetDlgItem(IDC_STATIC_READS_SPEED_AVG)->MoveWindow(rcSpeedInfo[1][2]);

		rcSpeedInfo[2][0].MoveToY(rcSpeedInfo[2][0].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[2][0]);
		this->GetDlgItem(IDC_STATIC_WRITES_LABEL)->MoveWindow(rcSpeedInfo[2][0]);

		rcSpeedInfo[2][1].MoveToY(rcSpeedInfo[2][1].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[2][1]);
		this->GetDlgItem(IDC_STATIC_WRITES_UNIT)->MoveWindow(rcSpeedInfo[2][1]);

		rcSpeedInfo[2][2].MoveToY(rcSpeedInfo[2][2].top - nHeight);
		this->ScreenToClient(rcSpeedInfo[2][2]);
		this->GetDlgItem(IDC_STATIC_WRITES_SPEED_AVG)->MoveWindow(rcSpeedInfo[2][2]);

		rcGroupStats.MoveToY(rcGroupStats.top - nHeight);
		this->ScreenToClient(rcGroupStats);
		this->GetDlgItem(IDC_STATIC_GROUP_STATS)->MoveWindow(rcGroupStats);

		rcGroup.bottom -= nHeight;
		this->ScreenToClient(rcGroup);
		this->GetDlgItem(IDC_STATIC_GROUP_ENCODING)->MoveWindow(rcGroup);

		rcDlg.bottom -= nHeight;
		this->MoveWindow(rcDlg);
	}
}

void CEncWAVtoAC3WorkDlg::UpdateTotalTimer()
{
	TCHAR strTime[32] = _T("");
	m_ElapsedTimeTotal += 0.25;

	// get time in format hh:mm:ss
	if (m_ElapsedTimeTotal <= 59)
	{
		_stprintf(strTime, _T("%s 00:00:%02u\0"),
			HaveLangStrings() ? (LPCTSTR)GetLangString(0x00A01006) : _T("Total elapsed time:"),
			(unsigned long)m_ElapsedTimeTotal); // ss
	}
	else if (m_ElapsedTimeTotal <= 3599)
	{
		_stprintf(strTime, _T("%s 00:%02u:%02u\0"),
			HaveLangStrings() ? (LPCTSTR)GetLangString(0x00A01006) : _T("Total elapsed time:"),
			((unsigned long)m_ElapsedTimeTotal / 60), // mm
			((unsigned long)m_ElapsedTimeTotal % 60)); // ss
	}
	else
	{
		_stprintf(strTime, _T("%s %02u:%02u:%02u\0"),
			HaveLangStrings() ? (LPCTSTR)GetLangString(0x00A01006) : _T("Total elapsed time:"),
			((unsigned long)m_ElapsedTimeTotal / 60) / 60, // hh
			((unsigned long)m_ElapsedTimeTotal / 60) % 60, // mm
			((((unsigned long)m_ElapsedTimeTotal / 60) % 60) * 60) % 60); // ss
	}

	// write to dialog
	if (this->bCanUpdateWindow == true)
	{
		this->bCanUpdateWindow = false;
		m_StcTimeTotal.SetWindowText(strTime);
		this->bCanUpdateWindow = true;
	}
}

void CEncWAVtoAC3WorkDlg::UpdateFileTimer()
{
	TCHAR strTime[32] = _T("");
	m_ElapsedTimeFile += 0.25;

	// get time in format hh:mm:ss
	if (m_ElapsedTimeFile <= 59)
	{
		_stprintf(strTime, _T("%s 00:00:%02u\0"),
			HaveLangStrings() ? (LPCTSTR)GetLangString(0x00A01005) : _T("Elapsed time:"),
			(unsigned long)m_ElapsedTimeFile); // ss
	}
	else if (m_ElapsedTimeFile <= 3599)
	{
		_stprintf(strTime, _T("%s 00:%02u:%02u\0"),
			HaveLangStrings() ? (LPCTSTR)GetLangString(0x00A01005) : _T("Elapsed time:"),
			((unsigned long)m_ElapsedTimeFile / 60), // mm
			((unsigned long)m_ElapsedTimeFile % 60)); // ss
	}
	else
	{
		_stprintf(strTime, _T("%s %02u:%02u:%02u\0"),
			HaveLangStrings() ? (LPCTSTR)GetLangString(0x00A01005) : _T("Elapsed time:"),
			((unsigned long)m_ElapsedTimeFile / 60) / 60, // hh
			((unsigned long)m_ElapsedTimeFile / 60) % 60, // mm
			((((unsigned long)m_ElapsedTimeFile / 60) % 60) * 60) % 60); // ss
	}

	// write to dialog
	if (this->bCanUpdateWindow == true)
	{
		this->bCanUpdateWindow = false;

		// show current time
		m_StcTimeCurrent.SetWindowText(strTime);

		// show current avg. speed
		this->GetDlgItem(IDC_STATIC_ENCODER_SPEED_AVG)->SetWindowText(szSpeedEncoderAvg);
		this->GetDlgItem(IDC_STATIC_READS_SPEED_AVG)->SetWindowText(szSpeedReadsAvg);
		this->GetDlgItem(IDC_STATIC_WRITES_SPEED_AVG)->SetWindowText(szSpeedWritesAvg);

		this->bCanUpdateWindow = true;
	}
}

void CEncWAVtoAC3WorkDlg::CreateWorker()
{
	this->hThread = ::CreateThread(NULL,
		0,
		EncWorkThread,
		&workParam,
		0,
		&this->dwThreadId);

	if (this->hThread == NULL)
	{
		::LogMessage(_T("Error: Failed to create worker thread!"));

		// show critical error message
		this->MessageBox(HaveLangStrings() ? GetLangString(0x00A0100B) : _T("Failed to create worker thread!"),
			HaveLangStrings() ? GetLangString(0x00A0100A) : _T("Fatal Error"),
			MB_OK | MB_ICONERROR);
	}
}

void CEncWAVtoAC3WorkDlg::InitLang()
{
	if (HaveLangStrings())
	{
		this->SetWindowText(GetLangString(0x00A01001));
		this->GetDlgItem(IDCANCEL)->SetWindowText(GetLangString(0x00A01002));
		this->GetDlgItem(IDC_STATIC_ENCODER_LABEL)->SetWindowText(GetLangString(0x00A01007));
		this->GetDlgItem(IDC_STATIC_READS_LABEL)->SetWindowText(GetLangString(0x00A01008));
		this->GetDlgItem(IDC_STATIC_WRITES_LABEL)->SetWindowText(GetLangString(0x00A01009));
	}
}
