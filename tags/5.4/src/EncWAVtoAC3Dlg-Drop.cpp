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

/* $Id$ */

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

typedef struct TDRAGANDDROP
{
	CEncWAVtoAC3Dlg *pDlg;
	HDROP hDropInfo;
} DRAGANDDROP, *PDRAGANDDROP;

static volatile bool bHandleDrop = true;
static volatile HANDLE hDDThread;
static DWORD dwDDThreadID;
static volatile DRAGANDDROP m_DDParam;

DWORD WINAPI DragAndDropThread(LPVOID lpParam)
{
	PDRAGANDDROP m_ThreadParam = (PDRAGANDDROP)lpParam;
	m_ThreadParam->pDlg->HandleDropFiles(m_ThreadParam->hDropInfo);
	bHandleDrop = true;
	return ::CloseHandle(hDDThread);
}

void CEncWAVtoAC3Dlg::OnDropFiles(HDROP hDropInfo)
{
	if (bHandleDrop == true)
	{
		bHandleDrop = false;
		m_DDParam.pDlg = this;
		m_DDParam.hDropInfo = hDropInfo;

		hDDThread = ::CreateThread(NULL, 0, DragAndDropThread, (LPVOID)&m_DDParam, 0, &dwDDThreadID);
		if (hDDThread == NULL)
			bHandleDrop = true;
	}

	// under Win9x this does not work, we use separate thread to handle drop
	// this->HandleDropFiles(hDropInfo);
	CResizeDialog::OnDropFiles(hDropInfo);
}
