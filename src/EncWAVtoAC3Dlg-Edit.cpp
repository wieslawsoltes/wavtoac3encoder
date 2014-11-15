//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wies³aw Šoltés <wieslaw.soltes@gmail.com>
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
#include "EncWAVtoAC3Dlg.h"
#include "EncWAVtoAC3WorkDlg.h"
#include "EncWAVtoAC3MuxDlg.h"
#include "EncWAVtoAC3EngDlg.h"
#include "AboutDlg.h"
#include "Utilities.h"
#include "MyFile.h"
#include "EncWorkThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CEncWAVtoAC3Dlg::OnEnChangeEditRawSampleRate()
{
	// check if number is in range
	CString szBuff;
	int nPos;

	this->m_EdtRawSamplerate.GetWindowText(szBuff);

	// check if we have focus (needed for editing)
	if (szBuff.Compare(_T("")) == 0)
		return;

	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
	{
		nPos = 0;
		this->m_SpnRawSampleRate.SetPos(nPos);
	}
	else
	{
		nPos = (int)_ttoi(szBuff);
		if ((nPos <= 0) || (nPos > INT_MAX))
		{
			nPos = 0;
			this->m_SpnRawSampleRate.SetPos(0);
			this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
		}
		else
		{
			this->m_SpnRawSampleRate.SetPos(nPos);
		}
	}

	EncoderPreset tmpPreset = GetCurrentPreset();
	tmpPreset.nRawSampleRate = nPos;
	UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnEnChangeEditRawChannels()
{
	// check if number is in range
	CString szBuff;
	int nPos;

	this->m_EdtRawChannels.GetWindowText(szBuff);

	// check if we have focus (needed for editing)
	if (szBuff.Compare(_T("")) == 0)
		return;

	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
	{
		nPos = 0;
		this->m_SpnRawChannels.SetPos(nPos);
	}
	else
	{
		nPos = (int)_ttoi(szBuff);
		if ((nPos <= 0) || (nPos > INT_MAX))
		{
			nPos = 0;
			this->m_SpnRawChannels.SetPos(0);
			this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
		}
		else
		{
			this->m_SpnRawChannels.SetPos(nPos);
		}
	}

	EncoderPreset tmpPreset = GetCurrentPreset();
	tmpPreset.nRawChannels = nPos;
	UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnEnChangeEditThreads()
{
	// check if number is in range
	CString szBuff;
	int nPos;

	this->m_EdtThreads.GetWindowText(szBuff);

	// check if we have focus (needed for editing)
	if (szBuff.Compare(_T("")) == 0)
		return;

	if (szBuff.Compare(DEFAULT_TEXT_AUTO) == 0)
	{
		nPos = 0;
		this->m_SpnThreads.SetPos(nPos);
	}
	else
	{
		nPos = (int)_ttoi(szBuff);
		if ((nPos <= 0) || (nPos > INT_MAX))
		{
			nPos = 0;
			this->m_SpnThreads.SetPos(0);
			this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
		}
		else
		{
			this->m_SpnThreads.SetPos(nPos);
		}
	}

	EncoderPreset tmpPreset = GetCurrentPreset();
	tmpPreset.nThreads = nPos;
	UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnEnChangeEditOutputPath()
{
	CString szBuff;

	this->m_EdtOutPath.GetWindowText(szBuff);

	// update output file/path value
	if (this->bMultipleMonoInput == true)
		this->szOutputFile = szBuff;
	else
		this->szOutputPath = szBuff;
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditOutputPath()
{
	CString szBuff;
	this->m_EdtOutPath.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0 || szBuff.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
		this->m_EdtOutPath.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditRawSampleRate()
{
	CString szBuff;
	this->m_EdtRawSamplerate.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
		this->m_EdtRawSamplerate.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditRawChannels()
{
	CString szBuff;
	this->m_EdtRawChannels.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
		this->m_EdtRawChannels.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditThreads()
{
	CString szBuff;
	this->m_EdtThreads.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_AUTO) == 0)
		this->m_EdtThreads.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditOutputPath()
{
	CString szBuff;
	this->m_EdtOutPath.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
	{
		if (this->bMultipleMonoInput == true)
			this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
		else
			this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
	}
	else
	{
		if (this->bMultipleMonoInput == true)
			this->szOutputFile = szBuff;
		else
			this->szOutputPath = szBuff;
	}
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditRawSampleRate()
{
	CString szBuff;
	this->m_EdtRawSamplerate.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
		this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditRawChannels()
{
	CString szBuff;
	this->m_EdtRawChannels.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
		this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditThreads()
{
	CString szBuff;
	this->m_EdtThreads.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
		this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
}
