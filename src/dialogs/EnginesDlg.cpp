#include "StdAfx.h"
#include "MainApp.h"
#include "EnginesDlg.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"

IMPLEMENT_DYNAMIC(CEnginesDlg, CDialog)

CEnginesDlg::CEnginesDlg(CWnd* pParent /*=NULL*/)
    : CMyDialogEx(CEnginesDlg::IDD, pParent)
{
    bUpdateList = true;
    nCurrSel = 0;
}

CEnginesDlg::~CEnginesDlg()
{

}

void CEnginesDlg::DoDataExchange(CDataExchange* pDX)
{
    CMyDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ENGINES, m_LstEngines);
    DDX_Control(pDX, IDC_EDIT_ENGINE_NAME, m_EdtEngineName);
    DDX_Control(pDX, IDC_EDIT_ENGINE_PATH, m_EdtEnginePath);
}

BEGIN_MESSAGE_MAP(CEnginesDlg, CMyDialogEx)
    ON_BN_CLICKED(IDOK, &CEnginesDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CEnginesDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_BROWSE, &CEnginesDlg::OnBnClickedButtonEnginesBrowse)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_IMPORT, &CEnginesDlg::OnBnClickedButtonEnginesImport)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_EXPORT, &CEnginesDlg::OnBnClickedButtonEnginesExport)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_ADD, &CEnginesDlg::OnBnClickedButtonEnginesAdd)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES_REMOVE, &CEnginesDlg::OnBnClickedButtonEnginesRemove)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ENGINES, &CEnginesDlg::OnLvnItemchangedListEngines)
    ON_EN_CHANGE(IDC_EDIT_ENGINE_NAME, &CEnginesDlg::OnEnChangeEditEngineName)
    ON_EN_CHANGE(IDC_EDIT_ENGINE_PATH, &CEnginesDlg::OnEnChangeEditEnginePath)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_ENGINES, &CEnginesDlg::OnLvnKeydownListEngines)
END_MESSAGE_MAP()

BOOL CEnginesDlg::OnInitDialog()
{
    CMyDialogEx::OnInitDialog();

    // set style of the engines list
    this->m_LstEngines.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // add columns to engines list
    this->m_LstEngines.InsertColumn(0,
        theApp.HaveLangStrings() ? theApp.GetLangString(0x00B0100C) : _T("Name"),
        0, 150);
    this->m_LstEngines.InsertColumn(1,
        theApp.HaveLangStrings() ? theApp.GetLangString(0x00B0100D) : _T("Path"),
        0, 440);

    // populate engines list
    this->InsertProgramEngines();

    // select currently used engine
    this->m_LstEngines.SetItemState(-1, 0, LVIS_SELECTED);
    this->m_LstEngines.SetItemState(this->nCurrSel, LVIS_SELECTED, LVIS_SELECTED);

    InitLang();

    return TRUE;
}

void CEnginesDlg::OnBnClickedOk()
{
    OnOK();
}

void CEnginesDlg::OnBnClickedCancel()
{
    OnCancel();
}

void CEnginesDlg::OnBnClickedButtonEnginesBrowse()
{
    CFileDialog fd(TRUE,
        _T("dll"),
        _T(""),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        theApp.HaveLangStrings() ? theApp.GetLangString(0x00B0100E) : _T("DLL Files (*.dll)|*.dll|All Files (*.*)|*.*||"),
        this);

    if (fd.DoModal() == IDOK)
    {
        // get full path from file dialog
        CString szFileName = fd.GetPathName();

        // set engine path
        this->m_EdtEnginePath.SetWindowText(szFileName);
    }
}

void CEnginesDlg::OnBnClickedButtonEnginesImport()
{
    CFileDialog fd(TRUE,
        _T("engines"),
        _T(""),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        theApp.HaveLangStrings() ? theApp.GetLangString(0x00B0100F) : _T("Aften Engines (*.engines)|*.engines|All Files (*.*)|*.*||"),
        this);

    if (fd.DoModal() == IDOK)
    {
        // get full path from file dialog
        CString szFileName = fd.GetPathName();

        // load engines from file
        this->LoadProgramEngines(szFileName);
    }
}

void CEnginesDlg::OnBnClickedButtonEnginesExport()
{
    CFileDialog fd(FALSE,
        _T("engines"),
        _T(""),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        theApp.HaveLangStrings() ? theApp.GetLangString(0x00B0100F) : _T("Aften Engines (*.engines)|*.engines|All Files (*.*)|*.*||"),
        this);

    if (fd.DoModal() == IDOK)
    {
        // get full path from file dialog
        CString szFileName = fd.GetPathName();

        // save engines from file
        this->SaveProgramEngines(szFileName);
    }
}

void CEnginesDlg::OnBnClickedButtonEnginesAdd()
{
    int nSize = this->m_EngineList.GetSize();
    ConfigEntry ce;

    this->m_EdtEngineName.GetWindowText(ce.szKey);
    this->m_EdtEnginePath.GetWindowText(ce.szData);

    this->m_EngineList.AddTail(ce);

    this->m_LstEngines.InsertItem(nSize, ce.szKey);
    this->m_LstEngines.SetItemText(nSize, 1, ce.szData);
}

void CEnginesDlg::OnBnClickedButtonEnginesRemove()
{
    CList<int, int> list;
    POSITION pos;

    // get all selected items
    pos = this->m_LstEngines.GetFirstSelectedItemPosition();
    while (pos != NULL)
    {
        list.AddTail(this->m_LstEngines.GetNextSelectedItem(pos));
    }

    // remove all selected items
    pos = list.GetTailPosition();
    while (pos != NULL)
    {
        int nIndex = list.GetPrev(pos);
        this->m_LstEngines.DeleteItem(nIndex);
        this->m_EngineList.RemoveAt(m_EngineList.FindIndex(nIndex));
    }
}

bool CEnginesDlg::InsertProgramEngines()
{
    int nSize = this->m_EngineList.GetSize();

    // no engine return error
    if (nSize == 0)
        return false;

    POSITION pos = this->m_EngineList.GetHeadPosition();
    for (INT_PTR i = 0; i < nSize; i++)
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

    // always selected first item in the list
    this->m_LstEngines.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

    return true;
}

bool CEnginesDlg::LoadProgramEngines(CString szFileName)
{
    // init engines configuration
    this->m_EngineList.RemoveAll();
    this->m_LstEngines.DeleteAllItems();

    if (theApp.LoadConfig(szFileName, this->m_EngineList) == true)
    {
        return InsertProgramEngines();
    }

    return false;
}

bool CEnginesDlg::SaveProgramEngines(CString szFileName)
{
    // save engines configuration
    return theApp.SaveConfig(szFileName, this->m_EngineList);
}

void CEnginesDlg::OnLvnItemchangedListEngines(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (bUpdateList == true)
    {
        POSITION pos = m_LstEngines.GetFirstSelectedItemPosition();
        if (pos != NULL)
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

void CEnginesDlg::OnEnChangeEditEngineName()
{
    POSITION pos = this->m_LstEngines.GetFirstSelectedItemPosition();
    if (pos != NULL)
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

void CEnginesDlg::OnEnChangeEditEnginePath()
{
    POSITION pos = this->m_LstEngines.GetFirstSelectedItemPosition();
    if (pos != NULL)
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

void CEnginesDlg::OnLvnKeydownListEngines(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    // handle keyboard events here
    switch (pLVKeyDow->wVKey)
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

void CEnginesDlg::InitLang()
{
    if (theApp.HaveLangStrings())
    {
        this->SetWindowText(_T("WAV to AC3 Encoder - ") + theApp.GetLangString(0x00B01001));

        this->GetDlgItem(IDC_STATIC_GROUP_ENGINE)->SetWindowText(theApp.GetLangString(0x00B01002));
        this->GetDlgItem(IDC_STATIC_TEXT_ENGINE_NAME)->SetWindowText(theApp.GetLangString(0x00B01003));
        this->GetDlgItem(IDC_STATIC_TEXT_ENGINE_PATH)->SetWindowText(theApp.GetLangString(0x00B01004));

        this->GetDlgItem(IDC_BUTTON_ENGINES_BROWSE)->SetWindowText(theApp.GetLangString(0x00B01005));
        this->GetDlgItem(IDC_BUTTON_ENGINES_IMPORT)->SetWindowText(theApp.GetLangString(0x00B01006));
        this->GetDlgItem(IDC_BUTTON_ENGINES_EXPORT)->SetWindowText(theApp.GetLangString(0x00B01007));
        this->GetDlgItem(IDC_BUTTON_ENGINES_ADD)->SetWindowText(theApp.GetLangString(0x00B01008));
        this->GetDlgItem(IDC_BUTTON_ENGINES_REMOVE)->SetWindowText(theApp.GetLangString(0x00B01009));
        this->GetDlgItem(IDOK)->SetWindowText(theApp.GetLangString(0x00B0100A));
        this->GetDlgItem(IDCANCEL)->SetWindowText(theApp.GetLangString(0x00B0100B));
    }
}
