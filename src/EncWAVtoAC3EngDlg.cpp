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
#include "EncWAVtoAC3EngDlg.h"
#include "Utilities.h"
#include "MyFile.h"

IMPLEMENT_DYNAMIC(CEncWAVtoAC3EngDlg, CDialog)

CEncWAVtoAC3EngDlg::CEncWAVtoAC3EngDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEncWAVtoAC3EngDlg::IDD, pParent)
{

}

CEncWAVtoAC3EngDlg::~CEncWAVtoAC3EngDlg()
{

}

void CEncWAVtoAC3EngDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ENGINES, m_LstEngines);
    DDX_Control(pDX, IDC_EDIT1, m_EdtEngineName);
    DDX_Control(pDX, IDC_EDIT2, m_EdtEnginePath);
}


BEGIN_MESSAGE_MAP(CEncWAVtoAC3EngDlg, CDialog)
END_MESSAGE_MAP()


// CEncWAVtoAC3EngDlg message handlers

BOOL CEncWAVtoAC3EngDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    this->m_LstEngines.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    this->m_LstEngines.InsertColumn(0, _T("Name"), 0, 150);
    this->m_LstEngines.InsertColumn(1, _T("Path"), 0, 440);

    return TRUE;
}
