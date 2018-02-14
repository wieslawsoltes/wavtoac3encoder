#include "StdAfx.h"
#include "MainApp.h"
#include "MuxDlg.h"
#include "utilities\Utilities.h"

namespace app
{
    const int nNumChannelConfig = 8;

    const std::wstring szChannelConfig[nNumChannelConfig] =
    {
        L"1+1 = (Ch1,Ch2)",
        L"1/0 = (C)",
        L"2/0 = (L,R)",
        L"3/0 = (L,R,C)",
        L"2/1 = (L,R,S)",
        L"3/1 = (L,R,C,S)",
        L"2/2 = (L,R,SL,SR)",
        L"3/2 = (L,R,C,SL,SR)"
    };

    const int nNumInputFiles[nNumChannelConfig] =
    {
        2, 1, 2, 3, 3, 4, 4, 5
    };

    const int nChannelConfigStates[nNumChannelConfig][6] =
    {
        // FL FR FC S  SL SR
        {  1, 1, 0, 0, 0, 0 }, // 1+1
        {  0, 0, 1, 0, 0, 0 }, // 1/0
        {  1, 1, 0, 0, 0, 0 }, // 2/0
        {  1, 1, 1, 0, 0, 0 }, // 3/0
        {  1, 1, 0, 1, 0, 0 }, // 2/1
        {  1, 1, 1, 1, 0, 0 }, // 3/1
        {  1, 1, 0, 0, 1, 1 }, // 2/2
        {  1, 1, 1, 0, 1, 1 }  // 3/2
    };

    const std::wstring szChannelConfigNames[nNumChannelConfig][6] =
    {
        // FL FR FC S  SL SR
        {  L"Ch1", L"Ch2", L"-", L"-", L"-",   L"-"  }, // 1+1
        {  L"-",   L"-",   L"C", L"-", L"-",   L"-"  }, // 1/0
        {  L"L",   L"R",   L"-", L"-", L"-",   L"-"  }, // 2/0
        {  L"L",   L"R",   L"C", L"-", L"-",   L"-"  }, // 3/0
        {  L"L",   L"R",   L"-", L"S", L"-",   L"-"  }, // 2/1
        {  L"L",   L"R",   L"C", L"S", L"-",   L"-"  }, // 3/1
        {  L"L",   L"R",   L"-", L"-", L"SL",  L"SR" }, // 2/2
        {  L"L",   L"R",   L"C", L"-", L"SL",  L"SR" }  // 3/2
    };

    IMPLEMENT_DYNAMIC(CMuxDlg, CDialog)
    CMuxDlg::CMuxDlg(CWnd* pParent /*=nullptr*/)
        : CMyDialogEx(CMuxDlg::IDD, pParent)
    {
        for (int i = 0; i < 6; i++)
        {
            this->szInputFiles[i] = _T("");
            this->szTmpInputFiles[i] = _T("");
        }

        this->nChannelConfig = nNumChannelConfig - 1;
        this->bLFE = true;
    }

    CMuxDlg::~CMuxDlg()
    {
    }

    void CMuxDlg::DoDataExchange(CDataExchange* pDX)
    {
        CMyDialogEx::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_BUTTON_FL, m_BtnChannelFL);
        DDX_Control(pDX, IDC_BUTTON_FR, m_BtnChannelFR);
        DDX_Control(pDX, IDC_BUTTON_FC, m_BtnChannelFC);
        DDX_Control(pDX, IDC_BUTTON_LFE, m_BtnChannelLFE);
        DDX_Control(pDX, IDC_BUTTON_SL, m_BtnChannelSL);
        DDX_Control(pDX, IDC_BUTTON_SR, m_BtnChannelSR);
        DDX_Control(pDX, IDC_BUTTON_S, m_BtnChannelS);
        DDX_Control(pDX, IDC_STATIC_LABEL_FL, m_StcLabelFL);
        DDX_Control(pDX, IDC_STATIC_LABEL_FR, m_StcLabelFR);
        DDX_Control(pDX, IDC_STATIC_LABEL_FC, m_StcLabelFC);
        DDX_Control(pDX, IDC_STATIC_LABEL_LFE, m_StcLabelLFE);
        DDX_Control(pDX, IDC_STATIC_LABEL_SL, m_StcLabelSL);
        DDX_Control(pDX, IDC_STATIC_LABEL_SR, m_StcLabelSR);
        DDX_Control(pDX, IDC_COMBO_CHANNEL_CONFIG, m_CmbChannelConfig);
        DDX_Control(pDX, IDC_CHECK_CHANNEL_CONFIG_LFE, m_ChkChannelConfigLFE);
        DDX_Control(pDX, IDC_EDIT_FL, m_EdtChannelFL);
        DDX_Control(pDX, IDC_EDIT_FR, m_EdtChannelFR);
        DDX_Control(pDX, IDC_EDIT_FC, m_EdtChannelFC);
        DDX_Control(pDX, IDC_EDIT_LFE, m_EdtChannelLFE);
        DDX_Control(pDX, IDC_EDIT_SL, m_EdtChannelSL);
        DDX_Control(pDX, IDC_EDIT_SR, m_EdtChannelSR);
    }

    BEGIN_MESSAGE_MAP(CMuxDlg, CMyDialogEx)
        ON_BN_CLICKED(IDC_BUTTON_FL, &CMuxDlg::OnBnClickedButtonFl)
        ON_BN_CLICKED(IDC_BUTTON_FR, &CMuxDlg::OnBnClickedButtonFr)
        ON_BN_CLICKED(IDC_BUTTON_FC, &CMuxDlg::OnBnClickedButtonFc)
        ON_BN_CLICKED(IDC_BUTTON_LFE, &CMuxDlg::OnBnClickedButtonLfe)
        ON_BN_CLICKED(IDC_BUTTON_SL, &CMuxDlg::OnBnClickedButtonSl)
        ON_BN_CLICKED(IDC_BUTTON_SR, &CMuxDlg::OnBnClickedButtonSr)
        ON_BN_CLICKED(IDC_BUTTON_S, &CMuxDlg::OnBnClickedButtonS)
        ON_BN_CLICKED(IDC_BUTTON_CLEAR_FL, &CMuxDlg::OnBnClickedButtonClearFl)
        ON_BN_CLICKED(IDC_BUTTON_CLEAR_FC, &CMuxDlg::OnBnClickedButtonClearFc)
        ON_BN_CLICKED(IDC_BUTTON_CLEAR_FR, &CMuxDlg::OnBnClickedButtonClearFr)
        ON_BN_CLICKED(IDC_BUTTON_CLEAR_LFE, &CMuxDlg::OnBnClickedButtonClearLfe)
        ON_BN_CLICKED(IDC_BUTTON_CLEAR_SL, &CMuxDlg::OnBnClickedButtonClearSl)
        ON_BN_CLICKED(IDC_BUTTON_CLEAR_SR, &CMuxDlg::OnBnClickedButtonClearSr)
        ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CMuxDlg::OnBnClickedButtonExport)
        ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CMuxDlg::OnBnClickedButtonImport)
        ON_BN_CLICKED(IDCANCEL, &CMuxDlg::OnBnClickedCancel)
        ON_BN_CLICKED(IDOK, &CMuxDlg::OnBnClickedOk)
        ON_BN_CLICKED(IDC_CHECK_CHANNEL_CONFIG_LFE, &CMuxDlg::OnBnClickedCheckChannelConfigLfe)
        ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL_CONFIG, &CMuxDlg::OnCbnSelchangeComboChannelConfig)
    END_MESSAGE_MAP()

    BOOL CMuxDlg::OnInitDialog()
    {
        CMyDialogEx::OnInitDialog();

        InitCtrls();
        InitLang();
        this->RemapFilesToChannels();
        this->SetFilePaths();
        this->SetControlsState();

        return TRUE;
    }

    void CMuxDlg::InitCtrls()
    {
        for (int i = 0; i < nNumChannelConfig; i++)
            this->m_CmbChannelConfig.InsertString(i, szChannelConfig[i].c_str());

        this->m_CmbChannelConfig.SetCurSel(0);

        std::wstring szTmpText;
        szTmpText = this->pConfig->GetString(0x00C01008);

        this->m_CmbChannelConfig.SetTooltipText(szTmpText.c_str());
        this->m_ChkChannelConfigLFE.SetTooltipText(this->pConfig->GetString(0x00C01009).c_str());

        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_CHANNEL_CONFIG, 15);

        this->m_BtnChannelFL.SetTooltipText(this->pConfig->GetString(0x00C0100A).c_str());
        this->m_BtnChannelFR.SetTooltipText(this->pConfig->GetString(0x00C0100B).c_str());
        this->m_BtnChannelFC.SetTooltipText(this->pConfig->GetString(0x00C0100C).c_str());
        this->m_BtnChannelLFE.SetTooltipText(this->pConfig->GetString(0x00C0100D).c_str());
        this->m_BtnChannelSL.SetTooltipText(this->pConfig->GetString(0x00C0100E).c_str());
        this->m_BtnChannelSR.SetTooltipText(this->pConfig->GetString(0x00C0100F).c_str());
        this->m_BtnChannelS.SetTooltipText(this->pConfig->GetString(0x00C01010).c_str());

        this->m_CmbChannelConfig.SetCurSel(this->nChannelConfig);
    }

    void CMuxDlg::InitLang()
    {
        this->SetWindowText((L"WAV to AC3 Encoder - " + this->pConfig->GetString(0x00C01001)).c_str());
        this->GetDlgItem(IDC_STATIC_TEXT_CHANNEL_CONFIG)->SetWindowText(this->pConfig->GetString(0x00C01002).c_str());
        this->GetDlgItem(IDC_BUTTON_IMPORT)->SetWindowText(this->pConfig->GetString(0x00C01003).c_str());
        this->GetDlgItem(IDC_BUTTON_EXPORT)->SetWindowText(this->pConfig->GetString(0x00C01004).c_str());
        this->GetDlgItem(IDOK)->SetWindowText(this->pConfig->GetString(0x00C01005).c_str());
        this->GetDlgItem(IDCANCEL)->SetWindowText(this->pConfig->GetString(0x00C01006).c_str());
        this->GetDlgItem(IDC_BUTTON_CLEAR_FL)->SetWindowText(this->pConfig->GetString(0x00C01007).c_str());
        this->GetDlgItem(IDC_BUTTON_CLEAR_FC)->SetWindowText(this->pConfig->GetString(0x00C01007).c_str());
        this->GetDlgItem(IDC_BUTTON_CLEAR_FR)->SetWindowText(this->pConfig->GetString(0x00C01007).c_str());
        this->GetDlgItem(IDC_BUTTON_CLEAR_LFE)->SetWindowText(this->pConfig->GetString(0x00C01007).c_str());
        this->GetDlgItem(IDC_BUTTON_CLEAR_SL)->SetWindowText(this->pConfig->GetString(0x00C01007).c_str());
        this->GetDlgItem(IDC_BUTTON_CLEAR_SR)->SetWindowText(this->pConfig->GetString(0x00C01007).c_str());
    }

    void CMuxDlg::RemapFilesToChannels()
    {
        for (int i = 0; i < 6; i++)
            this->szInputFiles[i] = _T("");

        switch (this->nChannelConfig)
        {
        case 0:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];
            this->szInputFiles[3] = szTmpInputFiles[2];
            break;
        case 1:
            this->szInputFiles[2] = szTmpInputFiles[0];
            this->szInputFiles[3] = szTmpInputFiles[1];
            break;
        case 2:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];
            this->szInputFiles[3] = szTmpInputFiles[2];
            break;
        case 3:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];
            this->szInputFiles[2] = szTmpInputFiles[2];
            this->szInputFiles[3] = szTmpInputFiles[3];
            break;
        case 4:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];

            if (this->bLFE == true)
            {
                this->szInputFiles[3] = szTmpInputFiles[2];
                this->szInputFiles[4] = szTmpInputFiles[3];
            }
            else
            {
                this->szInputFiles[4] = szTmpInputFiles[2];
            }
            break;
        case 5:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];
            this->szInputFiles[2] = szTmpInputFiles[2];

            if (this->bLFE == true)
            {
                this->szInputFiles[3] = szTmpInputFiles[3];
                this->szInputFiles[4] = szTmpInputFiles[4];
            }
            else
            {
                this->szInputFiles[4] = szTmpInputFiles[3];
            }
            break;
        case 6:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];

            if (this->bLFE == true)
            {
                this->szInputFiles[3] = szTmpInputFiles[4];
                this->szInputFiles[4] = szTmpInputFiles[2];
                this->szInputFiles[5] = szTmpInputFiles[3];
            }
            else
            {
                this->szInputFiles[4] = szTmpInputFiles[2];
                this->szInputFiles[5] = szTmpInputFiles[3];
            }
            break;
        case 7:
            this->szInputFiles[0] = szTmpInputFiles[0];
            this->szInputFiles[1] = szTmpInputFiles[1];
            this->szInputFiles[2] = szTmpInputFiles[2];

            if (this->bLFE == true)
            {
                this->szInputFiles[3] = szTmpInputFiles[3];
                this->szInputFiles[4] = szTmpInputFiles[4];
                this->szInputFiles[5] = szTmpInputFiles[5];
            }
            else
            {
                this->szInputFiles[4] = szTmpInputFiles[3];
                this->szInputFiles[5] = szTmpInputFiles[4];
            }
            break;
        };
    }

    void CMuxDlg::SetFilePaths()
    {
        this->m_EdtChannelFL.SetWindowText(szInputFiles[0].c_str());
        this->m_EdtChannelFR.SetWindowText(szInputFiles[1].c_str());
        this->m_EdtChannelFC.SetWindowText(szInputFiles[2].c_str());
        this->m_EdtChannelLFE.SetWindowText(szInputFiles[3].c_str());
        this->m_EdtChannelSL.SetWindowText(szInputFiles[4].c_str());
        this->m_EdtChannelSR.SetWindowText(szInputFiles[5].c_str());
    }

    void CMuxDlg::SetControlsState()
    {
        this->m_BtnChannelFL.EnableWindow(nChannelConfigStates[this->nChannelConfig][0] == 1 ? TRUE : FALSE);
        this->m_BtnChannelFR.EnableWindow(nChannelConfigStates[this->nChannelConfig][1] == 1 ? TRUE : FALSE);
        this->m_BtnChannelFC.EnableWindow(nChannelConfigStates[this->nChannelConfig][2] == 1 ? TRUE : FALSE);
        this->m_BtnChannelS.EnableWindow(nChannelConfigStates[this->nChannelConfig][3] == 1 ? TRUE : FALSE);
        this->m_BtnChannelSL.EnableWindow(nChannelConfigStates[this->nChannelConfig][4] == 1 ? TRUE : FALSE);
        this->m_BtnChannelSR.EnableWindow(nChannelConfigStates[this->nChannelConfig][5] == 1 ? TRUE : FALSE);
        this->m_BtnChannelLFE.EnableWindow(this->bLFE ? TRUE : FALSE);

        this->m_BtnChannelFL.SetWindowText(szChannelConfigNames[this->nChannelConfig][0].c_str());
        this->m_BtnChannelFR.SetWindowText(szChannelConfigNames[this->nChannelConfig][1].c_str());
        this->m_BtnChannelFC.SetWindowText(szChannelConfigNames[this->nChannelConfig][2].c_str());
        this->m_BtnChannelS.SetWindowText(szChannelConfigNames[this->nChannelConfig][3].c_str());
        this->m_BtnChannelSL.SetWindowText(szChannelConfigNames[this->nChannelConfig][4].c_str());
        this->m_BtnChannelSR.SetWindowText(szChannelConfigNames[this->nChannelConfig][5].c_str());
        this->m_BtnChannelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));

        this->m_StcLabelFL.SetWindowText(szChannelConfigNames[this->nChannelConfig][0].c_str());
        this->m_StcLabelFL.SetBold(nChannelConfigStates[this->nChannelConfig][0] == 1 ? true : false);
        this->m_StcLabelFR.SetWindowText(szChannelConfigNames[this->nChannelConfig][1].c_str());
        this->m_StcLabelFR.SetBold(nChannelConfigStates[this->nChannelConfig][1] == 1 ? true : false);
        this->m_StcLabelFC.SetWindowText(szChannelConfigNames[this->nChannelConfig][2].c_str());
        this->m_StcLabelFC.SetBold(nChannelConfigStates[this->nChannelConfig][2] == 1 ? true : false);
        this->m_StcLabelSL.SetWindowText(szChannelConfigNames[this->nChannelConfig][(nChannelConfigStates[this->nChannelConfig][3] == 1) ? 3 : 4].c_str());
        this->m_StcLabelSL.SetBold(nChannelConfigStates[this->nChannelConfig][(nChannelConfigStates[this->nChannelConfig][3] == 1) ? 3 : 4] ? true : false);
        this->m_StcLabelSR.SetWindowText(szChannelConfigNames[this->nChannelConfig][5].c_str());
        this->m_StcLabelSR.SetBold(nChannelConfigStates[this->nChannelConfig][5] == 1 ? true : false);
        this->m_StcLabelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));
        this->m_StcLabelLFE.SetBold(this->bLFE);

        this->m_ChkChannelConfigLFE.SetCheck(this->bLFE ? BST_CHECKED : BST_UNCHECKED);

        this->m_BtnChannelFL.SetBold((szInputFiles[0].length() > 0) ? true : false);
        this->m_BtnChannelFR.SetBold((szInputFiles[1].length() > 0) ? true : false);
        this->m_BtnChannelFC.SetBold((szInputFiles[2].length() > 0) ? true : false);

        this->m_BtnChannelLFE.SetBold((szInputFiles[3].length() > 0) ? true : false);

        if (nChannelConfigStates[nChannelConfig][3] == 1)
        {
            this->m_BtnChannelS.SetBold((szInputFiles[4].length() > 0) ? true : false);
            this->m_BtnChannelSL.SetBold(false);
            this->m_BtnChannelSL.SetWindowText(_T("-"));
            this->m_BtnChannelSL.EnableWindow(FALSE);
        }
        else
        {
            this->m_BtnChannelSL.SetBold((szInputFiles[4].length() > 0) ? true : false);
            this->m_BtnChannelS.SetBold(false);
            this->m_BtnChannelS.SetWindowText(_T("-"));
            this->m_BtnChannelS.EnableWindow(FALSE);
        }

        this->m_BtnChannelSR.SetBold((szInputFiles[5].length() > 0) ? true : false);
    }

    bool CMuxDlg::LoadFilesList(std::wstring &szFileName)
    {
        std::vector<std::wstring> fl;
        if (this->pConfig->LoadFiles(szFileName, fl) == false)
            return false;

        for (int i = 0; i < 6; i++)
        {
            szTmpInputFiles[i] = _T("");
        }

        int i = 0;
        for (; i < (int)fl.size(); i++)
        {
            if (i >= 6)
                return true;

            std::wstring szPath = fl[i];
            util::StringHelper::Trim(szPath, '"');
            szTmpInputFiles[i] = szPath;
        }

        int nReqNumOfFiles = nNumInputFiles[this->nChannelConfig] + (this->bLFE == true ? 1 : 0);
        if (i != nReqNumOfFiles)
        {
            return false;
        }
        else
        {
            this->RemapFilesToChannels();
            return true;
        }
    }

    bool CMuxDlg::SaveFilesList(std::wstring &szFileName, int nFormat)
    {
        try
        {
            std::vector<std::wstring> fl;
            std::wstring szBuffer;

            #define AddFile(index) \
                    szBuffer = (nFormat == 0 ? L"" : L"\"") + szInputFiles[index] + (nFormat == 0 ? L"" : L"\"") + L"\n"; \
                    fl.emplace_back(szBuffer);

            switch (this->nChannelConfig)
            {
            case 0:
                AddFile(0);
                AddFile(1);
                AddFile(3);
                break;
            case 1:
                AddFile(2);
                AddFile(3);
                break;
            case 2:
                AddFile(0);
                AddFile(1);
                AddFile(3);
                break;
            case 3:
                AddFile(0);
                AddFile(1);
                AddFile(2);
                AddFile(3);
                break;
            case 4:
                AddFile(0);
                AddFile(1);
                if (this->bLFE == true)
                {
                    AddFile(3);
                    AddFile(4);
                }
                else
                {
                    AddFile(4);
                }
                break;
            case 5:
                AddFile(0);
                AddFile(1);
                AddFile(2)
                    if (this->bLFE == true)
                    {
                        AddFile(3);
                        AddFile(4);
                    }
                    else
                    {
                        AddFile(4);
                    }
                break;
            case 6:
                AddFile(0);
                AddFile(1);
                if (this->bLFE == true)
                {
                    AddFile(4);
                    AddFile(5);
                    AddFile(3);
                }
                else
                {
                    AddFile(4);
                    AddFile(5);
                }
                break;
            case 7:
                AddFile(0);
                AddFile(1);
                AddFile(2);
                if (this->bLFE == true)
                {
                    AddFile(3);
                    AddFile(4);
                    AddFile(5);
                }
                else
                {
                    AddFile(4);
                    AddFile(5);
                }
                break;
            };

            #undef AddFile

            return this->pConfig->SaveFiles(szFileName, fl, nFormat);
        }
        catch (...)
        {
            return false;
        }
    }

    void CMuxDlg::ShowOpenFileDlg(int nID, controls::CMyButton *m_BtnCurrent, controls::CMyEdit *m_EdtCurrent)
    {
        if (m_BtnCurrent == nullptr || m_EdtCurrent == nullptr)
            return;

        CString szCurrentFileName;
        m_EdtCurrent->GetWindowText(szCurrentFileName);
        std::wstring szCurrentFileNameStr = szCurrentFileName;
        std::wstring szFileName = util::Utilities::GetFileName(szCurrentFileNameStr);
        std::wstring szFilter = this->pConfig->GetSupportedInputFilesFilter();

        CFileDialog fd(TRUE,
            this->pConfig->m_EncoderOptions.szSupportedInputExt[0].c_str(),
            szFileName.c_str(),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            szFilter.c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            m_EdtCurrent->SetWindowText(szFileName.c_str());
            this->szInputFiles[nID] = szFileName;
            m_BtnCurrent->SetBold(true);
        }
    }

    void CMuxDlg::OnBnClickedButtonFl()
    {
        this->ShowOpenFileDlg(0, &this->m_BtnChannelFL, &this->m_EdtChannelFL);
    }

    void CMuxDlg::OnBnClickedButtonFr()
    {
        this->ShowOpenFileDlg(1, &this->m_BtnChannelFR, &this->m_EdtChannelFR);
    }

    void CMuxDlg::OnBnClickedButtonFc()
    {
        this->ShowOpenFileDlg(2, &this->m_BtnChannelFC, &this->m_EdtChannelFC);
    }

    void CMuxDlg::OnBnClickedButtonLfe()
    {
        this->ShowOpenFileDlg(3, &this->m_BtnChannelLFE, &this->m_EdtChannelLFE);
    }

    void CMuxDlg::OnBnClickedButtonSl()
    {
        this->ShowOpenFileDlg(4, &this->m_BtnChannelSL, &this->m_EdtChannelSL);
    }

    void CMuxDlg::OnBnClickedButtonSr()
    {
        this->ShowOpenFileDlg(5, &this->m_BtnChannelSR, &this->m_EdtChannelSR);
    }

    void CMuxDlg::OnBnClickedButtonS()
    {
        this->ShowOpenFileDlg(4, &this->m_BtnChannelS, &this->m_EdtChannelSL);
    }

    void CMuxDlg::OnBnClickedButtonClearFl()
    {
        this->m_BtnChannelFL.SetBold(false);
        this->m_EdtChannelFL.SetWindowText(_T(""));
        this->szInputFiles[0] = _T("");
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedButtonClearFc()
    {
        this->m_BtnChannelFC.SetBold(false);
        this->m_EdtChannelFC.SetWindowText(_T(""));
        this->szInputFiles[2] = _T("");
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedButtonClearFr()
    {
        this->m_BtnChannelFR.SetBold(false);
        this->m_EdtChannelFR.SetWindowText(_T(""));
        this->szInputFiles[1] = _T("");
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedButtonClearLfe()
    {
        this->m_BtnChannelLFE.SetBold(false);
        this->m_EdtChannelLFE.SetWindowText(_T(""));
        this->szInputFiles[3] = _T("");
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedButtonClearSl()
    {
        if (nChannelConfigStates[nChannelConfig][3] == 1)
        {
            this->m_BtnChannelS.SetBold(false);
        }
        else
        {
            this->m_BtnChannelSL.SetBold(false);
        }

        this->m_EdtChannelSL.SetWindowText(_T(""));
        this->szInputFiles[4] = _T("");
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedButtonClearSr()
    {
        this->m_BtnChannelSR.SetBold(false);
        this->m_EdtChannelSR.SetWindowText(_T(""));
        this->szInputFiles[5] = _T("");
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedButtonExport()
    {
        CFileDialog fd(FALSE,
            _T("files"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            this->pConfig->GetString(0x00C01011).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();

            int nFormat = 0;
            if (fd.GetFileExt().CompareNoCase(_T("files")) == 0)
                nFormat = 0;
            else if (fd.GetFileExt().CompareNoCase(_T("mux")) == 0)
                nFormat = 1;

            this->SaveFilesList(szFileName, nFormat);
        }
    }

    void CMuxDlg::OnBnClickedButtonImport()
    {
        CFileDialog fd(TRUE,
            _T("files"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            this->pConfig->GetString(0x00C01011).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            this->LoadFilesList(szFileName);
            this->SetFilePaths();
            this->SetControlsState();
        }
    }

    void CMuxDlg::OnBnClickedCheckChannelConfigLfe()
    {
        const int bCheck = this->m_ChkChannelConfigLFE.GetCheck();
        this->bLFE = (bCheck == BST_CHECKED) ? true : false;
        this->m_BtnChannelLFE.EnableWindow(this->bLFE ? TRUE : FALSE);
        this->m_BtnChannelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));
        this->m_StcLabelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));
        this->m_StcLabelLFE.SetBold(this->bLFE);
        this->SetControlsState();
    }

    void CMuxDlg::OnBnClickedCancel()
    {
        for (int i = 0; i < 6; i++)
        {
            this->szInputFiles[i] = _T("");
            this->szTmpInputFiles[i] = _T("");
        }

        OnCancel();
    }

    void CMuxDlg::OnBnClickedOk()
    {
        switch (this->nChannelConfig)
        {
        case 0:
            this->szInputFiles[2] = _T("");
            this->szInputFiles[4] = _T("");
            this->szInputFiles[5] = _T("");
            break;
        case 1:
            this->szInputFiles[0] = _T("");
            this->szInputFiles[1] = _T("");
            this->szInputFiles[4] = _T("");
            this->szInputFiles[5] = _T("");
            break;
        case 2:
            this->szInputFiles[2] = _T("");
            this->szInputFiles[4] = _T("");
            this->szInputFiles[5] = _T("");
            break;
        case 3:
            this->szInputFiles[4] = _T("");
            this->szInputFiles[5] = _T("");
            break;
        case 4:
            if (this->bLFE == true)
            {
                this->szInputFiles[2] = _T("");
                this->szInputFiles[5] = _T("");
            }
            else
            {
                this->szInputFiles[2] = _T("");
                this->szInputFiles[3] = _T("");
                this->szInputFiles[5] = _T("");
            }
            break;
        case 5:
            if (this->bLFE == true)
            {
                this->szInputFiles[5] = _T("");
            }
            else
            {
                this->szInputFiles[3] = _T("");
                this->szInputFiles[5] = _T("");
            }
            break;
        case 6:
            if (this->bLFE == true)
            {
                this->szInputFiles[2] = _T("");
            }
            else
            {
                this->szInputFiles[2] = _T("");
                this->szInputFiles[3] = _T("");
            }
            break;
        case 7:
            if (this->bLFE == false)
            {
                this->szInputFiles[3] = _T("");
            }
            break;
        };

        OnOK();
    }

    void CMuxDlg::OnCbnSelchangeComboChannelConfig()
    {
        int nSel = this->m_CmbChannelConfig.GetCurSel();
        if (nSel != CB_ERR)
        {
            this->nChannelConfig = nSel;
            this->SetControlsState();
        }
    }
}
