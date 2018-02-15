#include "StdAfx.h"
#include "MainApp.h"
#include "EnginesDlg.h"
#include "utilities\Utilities.h"

namespace app
{
    IMPLEMENT_DYNAMIC(CEnginesDlg, CDialog)
    CEnginesDlg::CEnginesDlg(CWnd* pParent /*=nullptr*/)
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

        this->m_LstEngines.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        this->m_LstEngines.InsertColumn(0, this->pConfig->GetString(0x00B0100C).c_str(), 0, 150);
        this->m_LstEngines.InsertColumn(1, this->pConfig->GetString(0x00B0100D).c_str(), 0, 440);

        this->InsertProgramEngines();

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
            this->pConfig->GetString(0x00B0100E).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            this->m_EdtEnginePath.SetWindowText(szFileName.c_str());
        }
    }

    void CEnginesDlg::OnBnClickedButtonEnginesImport()
    {
        CFileDialog fd(TRUE,
            _T("engines"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            this->pConfig->GetString(0x00B0100F).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            this->LoadProgramEngines(szFileName);
        }
    }

    void CEnginesDlg::OnBnClickedButtonEnginesExport()
    {
        CFileDialog fd(FALSE,
            _T("engines"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            this->pConfig->GetString(0x00B0100F).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            this->SaveProgramEngines(szFileName);
        }
    }

    void CEnginesDlg::OnBnClickedButtonEnginesAdd()
    {
        int nSize = (int)this->m_Engines.size();

        CString szKey;
        CString szValue;
        this->m_EdtEngineName.GetWindowText(szKey);
        this->m_EdtEnginePath.GetWindowText(szValue);

        std::wstring szName = szKey;
        std::wstring szPath = szValue;
        this->m_Engines.emplace_back(config::CEngine(szName, szPath));

        this->m_LstEngines.InsertItem(nSize, szName.c_str());
        this->m_LstEngines.SetItemText(nSize, 1, szPath.c_str());
    }

    void CEnginesDlg::OnBnClickedButtonEnginesRemove()
    {
        std::vector<int> list;
        POSITION pos;

        pos = this->m_LstEngines.GetFirstSelectedItemPosition();
        while (pos != nullptr)
        {
            int nItem = this->m_LstEngines.GetNextSelectedItem(pos);
            list.emplace_back(nItem);
        }

        for (int i = (int)list.size() - 1; i >= 0; i--)
        {
            int nIndex = list[i];
            this->m_LstEngines.DeleteItem(nIndex);
            this->m_Engines.erase(this->m_Engines.begin() + nIndex);
        }
    }

    bool CEnginesDlg::InsertProgramEngines()
    {
        int nSize = (int)this->m_Engines.size();
        if (nSize == 0)
            return false;

        for (int i = 0; i < nSize; i++)
        {
            auto& ce = this->m_Engines[i];
            this->m_LstEngines.InsertItem(i, ce.szName.c_str());
            this->m_LstEngines.SetItemText(i, 1, ce.szPath.c_str());
        }

        this->m_LstEngines.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

        return true;
    }

    bool CEnginesDlg::LoadProgramEngines(std::wstring szFileName)
    {
        std::vector<config::Entry> engines;
        if (this->pConfig->LoadEngines(szFileName, engines) == true)
        {
            this->m_Engines = engines;
            this->m_LstEngines.DeleteAllItems();
            return InsertProgramEngines();
        }
        return false;
    }

    bool CEnginesDlg::SaveProgramEngines(std::wstring szFileName)
    {
        return this->pConfig->SaveEngines(szFileName, this->m_Engines);
    }

    void CEnginesDlg::OnLvnItemchangedListEngines(NMHDR *pNMHDR, LRESULT *pResult)
    {
        LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

        if (bUpdateList == true)
        {
            POSITION pos = m_LstEngines.GetFirstSelectedItemPosition();
            if (pos != nullptr)
            {
                int nItem = m_LstEngines.GetNextSelectedItem(pos);
                auto& engine = this->m_Engines[nItem];
                this->m_EdtEngineName.SetWindowText(ce.szName.c_str());
                this->m_EdtEnginePath.SetWindowText(ce.szPath.c_str());
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
        if (pos != nullptr)
        {
            CString szText;
            int nIndex = this->m_LstEngines.GetNextSelectedItem(pos);
            this->m_EdtEngineName.GetWindowText(szText);
            auto& engine = this->m_Engines[nIndex];
            ce.szName = szText;
            bUpdateList = false;
            this->m_LstEngines.SetItemText(nIndex, 0, szText);
        }
    }

    void CEnginesDlg::OnEnChangeEditEnginePath()
    {
        POSITION pos = this->m_LstEngines.GetFirstSelectedItemPosition();
        if (pos != nullptr)
        {
            CString szText;
            int nIndex = this->m_LstEngines.GetNextSelectedItem(pos);
            this->m_EdtEnginePath.GetWindowText(szText);
            auto& engine = this->m_Engines[nIndex];
            ce.szPath = szText;
            bUpdateList = false;
            this->m_LstEngines.SetItemText(nIndex, 1, szText);
        }
    }

    void CEnginesDlg::OnLvnKeydownListEngines(NMHDR *pNMHDR, LRESULT *pResult)
    {
        LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
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
        this->SetWindowText((L"WAV to AC3 Encoder - " + this->pConfig->GetString(0x00B01001)).c_str());
        this->GetDlgItem(IDC_STATIC_GROUP_ENGINE)->SetWindowText(this->pConfig->GetString(0x00B01002).c_str());
        this->GetDlgItem(IDC_STATIC_TEXT_ENGINE_NAME)->SetWindowText(this->pConfig->GetString(0x00B01003).c_str());
        this->GetDlgItem(IDC_STATIC_TEXT_ENGINE_PATH)->SetWindowText(this->pConfig->GetString(0x00B01004).c_str());
        this->GetDlgItem(IDC_BUTTON_ENGINES_BROWSE)->SetWindowText(this->pConfig->GetString(0x00B01005).c_str());
        this->GetDlgItem(IDC_BUTTON_ENGINES_IMPORT)->SetWindowText(this->pConfig->GetString(0x00B01006).c_str());
        this->GetDlgItem(IDC_BUTTON_ENGINES_EXPORT)->SetWindowText(this->pConfig->GetString(0x00B01007).c_str());
        this->GetDlgItem(IDC_BUTTON_ENGINES_ADD)->SetWindowText(this->pConfig->GetString(0x00B01008).c_str());
        this->GetDlgItem(IDC_BUTTON_ENGINES_REMOVE)->SetWindowText(this->pConfig->GetString(0x00B01009).c_str());
        this->GetDlgItem(IDOK)->SetWindowText(this->pConfig->GetString(0x00B0100A).c_str());
        this->GetDlgItem(IDCANCEL)->SetWindowText(this->pConfig->GetString(0x00B0100B).c_str());
    }
}
