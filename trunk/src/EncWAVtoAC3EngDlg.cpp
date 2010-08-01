//
// WAV to AC3 Encoder
// Copyright (C) 2007,2008,2009,2010 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

IMPLEMENT_DYNAMIC(CEncWAVtoAC3EngDlg, CDialogEx)

CEncWAVtoAC3EngDlg::CEncWAVtoAC3EngDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEncWAVtoAC3EngDlg::IDD, pParent)
{
    bUpdateList = true;
    nCurrSel = 0;
}

CEncWAVtoAC3EngDlg::~CEncWAVtoAC3EngDlg()
{

}

void CEncWAVtoAC3EngDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ENGINES, m_LstEngines);
    DDX_Control(pDX, IDC_EDIT_ENGINE_NAME, m_EdtEngineName);
    DDX_Control(pDX, IDC_EDIT_ENGINE_PATH, m_EdtEnginePath);
}

BEGIN_MESSAGE_MAP(CEncWAVtoAC3EngDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CEncWAVtoAC3EngDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CEncWAVtoAC3EngDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_BROWSE, &CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesBrowse)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_IMPORT, &CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesImport)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_EXPORT, &CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesExport)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_ADD, &CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesAdd)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_REMOVE, &CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesRemove)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ENGINES, &CEncWAVtoAC3EngDlg::OnLvnItemchangedListEngines)
    ON_EN_CHANGE(IDC_EDIT_ENGINE_NAME, &CEncWAVtoAC3EngDlg::OnEnChangeEditEngineName)
    ON_EN_CHANGE(IDC_EDIT_ENGINE_PATH, &CEncWAVtoAC3EngDlg::OnEnChangeEditEnginePath)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_ENGINES, &CEncWAVtoAC3EngDlg::OnLvnKeydownListEngines)
END_MESSAGE_MAP()

BOOL CEncWAVtoAC3EngDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // set style of the engines list
    this->m_LstEngines.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // add columns to engines list
    this->m_LstEngines.InsertColumn(0, _T("Name"), 0, 150);
    this->m_LstEngines.InsertColumn(1, _T("Path"), 0, 440);

    // populate engines list
    this->InsertProgramEngines();

    // select currently used engine
    this->m_LstEngines.SetItemState(-1,  0, LVIS_SELECTED);
    this->m_LstEngines.SetItemState(this->nCurrSel, LVIS_SELECTED, LVIS_SELECTED);

    return TRUE;
}

void CEncWAVtoAC3EngDlg::OnBnClickedOk()
{
    OnOK();
}

void CEncWAVtoAC3EngDlg::OnBnClickedCancel()
{
    OnCancel();
}

void CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesBrowse()
{
    CFileDialog fd(TRUE, 
        _T("dll"),
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("DLL Files (*.dll)|*.dll|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

        // set engine path
        this->m_EdtEnginePath.SetWindowText(szFileName);
    }
}

void CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesImport()
{
    CFileDialog fd(TRUE, 
        _T("engines"),
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Aften Engines (*.engines)|*.engines|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

        // load engines from file
        this->LoadProgramEngines(szFileName);
    }
}

void CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesExport()
{
    CFileDialog fd(FALSE, 
        _T("engines"),
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Aften Engines (*.engines)|*.engines|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

         // save engines from file
        this->SaveProgramEngines(szFileName);
    }
}

void CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesAdd()
{
    int nSize = this->m_EngineList.GetSize();
    ConfigEntry ce;

    this->m_EdtEngineName.GetWindowText(ce.szKey);
    this->m_EdtEnginePath.GetWindowText(ce.szData);

    this->m_EngineList.AddTail(ce);

    this->m_LstEngines.InsertItem(nSize, ce.szKey);
    this->m_LstEngines.SetItemText(nSize, 1, ce.szData);
}

void CEncWAVtoAC3EngDlg::OnBnClickedButtonEnginesRemove()
{
    CList<int,int> list;
    POSITION pos;

    // get all selected items
    pos = this->m_LstEngines.GetFirstSelectedItemPosition();
    while(pos != NULL)
    {
        list.AddTail(this->m_LstEngines.GetNextSelectedItem(pos));
    }

    // remove all selected items
    pos = list.GetTailPosition();
    while(pos != NULL)
    {
        int nIndex = list.GetPrev(pos);
        this->m_LstEngines.DeleteItem(nIndex);
        this->m_EngineList.RemoveAt(m_EngineList.FindIndex(nIndex));
    }
}

bool CEncWAVtoAC3EngDlg::InsertProgramEngines()
{
    int nSize = this->m_EngineList.GetSize();

    // no engine return error
    if(nSize == 0)
        return false;

    POSITION pos = this->m_EngineList.GetHeadPosition();
    for(INT_PTR i = 0; i < nSize; i++)
    {
        ConfigEntry ce;

        // get next entry in configuration list
        ce = this->m_EngineList.GetNext(pos);

        // insert all items to engines list
        // ce.szKey  - name of engine   
        // ce.szData - path to libaften.dll
        this->m_LstEngines.InsertItem(i, ce.szKey);
        this->m_LstEngines.SetItemText(i, 1, ce.szData);
    }

    // always selectd first item in the list
    this->m_LstEngines.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

    return true;
}

bool CEncWAVtoAC3EngDlg::LoadProgramEngines(CString szFileName)
{
    // init engines configuration
    this->m_EngineList.RemoveAll();
    this->m_LstEngines.DeleteAllItems();

    if(::LoadConfig(szFileName, this->m_EngineList) == true)
    {
        return InsertProgramEngines();
    }

    return false;
}

bool CEncWAVtoAC3EngDlg::SaveProgramEngines(CString szFileName)
{
    // save engines configuration
    return ::SaveConfig(szFileName, this->m_EngineList);
}

void CEncWAVtoAC3EngDlg::OnLvnItemchangedListEngines(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if(bUpdateList == true)
    {
    POSITION pos = m_LstEngines.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int nItem = m_LstEngines.GetNextSelectedItem(pos);

        ConfigEntry ce = this->m_EngineList.GetAt(this->m_EngineList.FindIndex(nItem));

        this->m_EdtEngineName.SetWindowText(ce.szKey);
        this->m_EdtEnginePath.SetWindowText(ce.szData);
    }
    else
    {
        this->m_EdtEngineName.SetWindowText(_T(""));
        this->m_EdtEnginePath.SetWindowText(_T(""));
    }
    }
    else
    {
        bUpdateList = true;
    }

    *pResult = 0;
}

void CEncWAVtoAC3EngDlg::OnEnChangeEditEngineName()
{
    POSITION pos = this->m_LstEngines.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        CString szText;

        int nIndex = this->m_LstEngines.GetNextSelectedItem(pos);
        this->m_EdtEngineName.GetWindowText(szText);

        POSITION posEngine = this->m_EngineList.FindIndex(nIndex);
        ConfigEntry ce = this->m_EngineList.GetAt(posEngine);
        ce.szKey = szText;
        this->m_EngineList.SetAt(posEngine, ce);

        bUpdateList = false;
        this->m_LstEngines.SetItemText(nIndex, 0, szText);
    }
}

void CEncWAVtoAC3EngDlg::OnEnChangeEditEnginePath()
{
    POSITION pos = this->m_LstEngines.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        CString szText;

        int nIndex = this->m_LstEngines.GetNextSelectedItem(pos);
        this->m_EdtEnginePath.GetWindowText(szText);


        POSITION posEngine = this->m_EngineList.FindIndex(nIndex);
        ConfigEntry ce = this->m_EngineList.GetAt(posEngine);
        ce.szData = szText;
        this->m_EngineList.SetAt(posEngine, ce);

        bUpdateList = false;
        this->m_LstEngines.SetItemText(nIndex, 1, szText);
    }
}

void CEncWAVtoAC3EngDlg::OnLvnKeydownListEngines(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    // handle keyboard events here
    switch(pLVKeyDow->wVKey)
    {
    case VK_DELETE: 
        {
            this->OnBnClickedButtonEnginesRemove();
        }
        break;

    default: break;
    };

    *pResult = 0;
}
