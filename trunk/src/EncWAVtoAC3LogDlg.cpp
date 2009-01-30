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

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "EncWAVtoAC3LogDlg.h"

IMPLEMENT_DYNAMIC(CEncWAVtoAC3LogDlg, CDialog)

CEncWAVtoAC3LogDlg::CEncWAVtoAC3LogDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CEncWAVtoAC3LogDlg::IDD, pParent)
{

}

CEncWAVtoAC3LogDlg::~CEncWAVtoAC3LogDlg()
{

}

void CEncWAVtoAC3LogDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LOG, m_LstLog);
}

BEGIN_MESSAGE_MAP(CEncWAVtoAC3LogDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CEncWAVtoAC3LogDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_LOG, &CEncWAVtoAC3LogDlg::OnBnClickedButtonSaveLog)
END_MESSAGE_MAP()

void CEncWAVtoAC3LogDlg::OnBnClickedOk()
{
    OnOK();
}

void CEncWAVtoAC3LogDlg::OnBnClickedButtonSaveLog()
{

}
