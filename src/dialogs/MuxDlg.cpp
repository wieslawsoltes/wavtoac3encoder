#include "StdAfx.h"
#include "MainApp.h"
#include "MuxDlg.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"

const int nNumChannelConfig = 8;

const CString szChannelConfig[nNumChannelConfig] =
{
    _T("1+1 = (Ch1,Ch2)"),
    _T("1/0 = (C)"),
    _T("2/0 = (L,R)"),
    _T("3/0 = (L,R,C)"),
    _T("2/1 = (L,R,S)"),
    _T("3/1 = (L,R,C,S)"),
    _T("2/2 = (L,R,SL,SR)"),
    _T("3/2 = (L,R,C,SL,SR)")
};

const int nNumInputFiles[nNumChannelConfig] =
{
    2, 1, 2, 3, 3, 4, 4, 5
};

const int nChannelConfigStates[nNumChannelConfig][CEncoderDefaults::nNumMaxInputFiles] =
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

const CString szChannelConfigNames[nNumChannelConfig][CEncoderDefaults::nNumMaxInputFiles] =
{
    // FL FR FC S  SL SR
    {  _T("Ch1"), _T("Ch2"), _T("-"), _T("-"), _T("-"),   _T("-")  }, // 1+1
    {  _T("-"),   _T("-"),   _T("C"), _T("-"), _T("-"),   _T("-")  }, // 1/0
    {  _T("L"),   _T("R"),   _T("-"), _T("-"), _T("-"),   _T("-")  }, // 2/0
    {  _T("L"),   _T("R"),   _T("C"), _T("-"), _T("-"),   _T("-")  }, // 3/0
    {  _T("L"),   _T("R"),   _T("-"), _T("S"), _T("-"),   _T("-")  }, // 2/1
    {  _T("L"),   _T("R"),   _T("C"), _T("S"), _T("-"),   _T("-")  }, // 3/1
    {  _T("L"),   _T("R"),   _T("-"), _T("-"), _T("SL"),  _T("SR") }, // 2/2
    {  _T("L"),   _T("R"),   _T("C"), _T("-"), _T("SL"),  _T("SR") }  // 3/2
};

IMPLEMENT_DYNAMIC(CMuxDlg, CDialog)

CMuxDlg::CMuxDlg(CWnd* pParent /*=nullptr*/)
    : CMyDialogEx(CMuxDlg::IDD, pParent)
{
    for (int i = 0; i < CEncoderDefaults::nNumMaxInputFiles; i++)
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
        this->m_CmbChannelConfig.InsertString(i, szChannelConfig[i]);

    this->m_CmbChannelConfig.SetCurSel(0);

    CString szTmpText;
    szTmpText = theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C01008) :
        _T("Specify channel configuration:\n")
        _T("1+1 = (Ch1,Ch2)\n")
        _T("1/0 = (C)\n")
        _T("2/0 = (L,R)\n")
        _T("3/0 = (L,R,C)\n")
        _T("2/1 = (L,R,S)\n")
        _T("3/1 = (L,R,C,S)\n")
        _T("2/2 = (L,R,SL,SR)\n")
        _T("3/2 = (L,R,C,SL,SR)");

    this->m_CmbChannelConfig.SetTooltipText(szTmpText);
    this->m_ChkChannelConfigLFE.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C01009) : _T("Indicates use of the LFE channel."));

    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_CHANNEL_CONFIG, 15);

    this->m_BtnChannelFL.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C0100A) : _T("Front Left Channel"));
    this->m_BtnChannelFR.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C0100B) : _T("Front Right Channel"));
    this->m_BtnChannelFC.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C0100C) : _T("Front Center Channel"));
    this->m_BtnChannelLFE.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C0100D) : _T("Low Frequency Effect Channel"));
    this->m_BtnChannelSL.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C0100E) : _T("Surround Left Channel"));
    this->m_BtnChannelSR.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C0100F) : _T("Surround Right Channel"));
    this->m_BtnChannelS.SetTooltipText(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C01010) : _T("Surround Channel"));

    this->m_CmbChannelConfig.SetCurSel(this->nChannelConfig);
}

void CMuxDlg::InitLang()
{
    if (theApp.m_Config.HaveLangStrings())
    {
        this->SetWindowText(_T("WAV to AC3 Encoder - ") + theApp.m_Config.GetLangString(0x00C01001));
        this->GetDlgItem(IDC_STATIC_TEXT_CHANNEL_CONFIG)->SetWindowText(theApp.m_Config.GetLangString(0x00C01002));
        this->GetDlgItem(IDC_BUTTON_IMPORT)->SetWindowText(theApp.m_Config.GetLangString(0x00C01003));
        this->GetDlgItem(IDC_BUTTON_EXPORT)->SetWindowText(theApp.m_Config.GetLangString(0x00C01004));
        this->GetDlgItem(IDOK)->SetWindowText(theApp.m_Config.GetLangString(0x00C01005));
        this->GetDlgItem(IDCANCEL)->SetWindowText(theApp.m_Config.GetLangString(0x00C01006));
        this->GetDlgItem(IDC_BUTTON_CLEAR_FL)->SetWindowText(theApp.m_Config.GetLangString(0x00C01007));
        this->GetDlgItem(IDC_BUTTON_CLEAR_FC)->SetWindowText(theApp.m_Config.GetLangString(0x00C01007));
        this->GetDlgItem(IDC_BUTTON_CLEAR_FR)->SetWindowText(theApp.m_Config.GetLangString(0x00C01007));
        this->GetDlgItem(IDC_BUTTON_CLEAR_LFE)->SetWindowText(theApp.m_Config.GetLangString(0x00C01007));
        this->GetDlgItem(IDC_BUTTON_CLEAR_SL)->SetWindowText(theApp.m_Config.GetLangString(0x00C01007));
        this->GetDlgItem(IDC_BUTTON_CLEAR_SR)->SetWindowText(theApp.m_Config.GetLangString(0x00C01007));
    }
}

void CMuxDlg::RemapFilesToChannels()
{
    for (int i = 0; i < CEncoderDefaults::nNumMaxInputFiles; i++)
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
    this->m_EdtChannelFL.SetWindowText(szInputFiles[0]);
    this->m_EdtChannelFR.SetWindowText(szInputFiles[1]);
    this->m_EdtChannelFC.SetWindowText(szInputFiles[2]);
    this->m_EdtChannelLFE.SetWindowText(szInputFiles[3]);
    this->m_EdtChannelSL.SetWindowText(szInputFiles[4]);
    this->m_EdtChannelSR.SetWindowText(szInputFiles[5]);
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

    this->m_BtnChannelFL.SetWindowText(szChannelConfigNames[this->nChannelConfig][0]);
    this->m_BtnChannelFR.SetWindowText(szChannelConfigNames[this->nChannelConfig][1]);
    this->m_BtnChannelFC.SetWindowText(szChannelConfigNames[this->nChannelConfig][2]);
    this->m_BtnChannelS.SetWindowText(szChannelConfigNames[this->nChannelConfig][3]);
    this->m_BtnChannelSL.SetWindowText(szChannelConfigNames[this->nChannelConfig][4]);
    this->m_BtnChannelSR.SetWindowText(szChannelConfigNames[this->nChannelConfig][5]);
    this->m_BtnChannelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));

    this->m_StcLabelFL.SetWindowText(szChannelConfigNames[this->nChannelConfig][0]);
    this->m_StcLabelFL.SetBold(nChannelConfigStates[this->nChannelConfig][0] == 1 ? true : false);
    this->m_StcLabelFR.SetWindowText(szChannelConfigNames[this->nChannelConfig][1]);
    this->m_StcLabelFR.SetBold(nChannelConfigStates[this->nChannelConfig][1] == 1 ? true : false);
    this->m_StcLabelFC.SetWindowText(szChannelConfigNames[this->nChannelConfig][2]);
    this->m_StcLabelFC.SetBold(nChannelConfigStates[this->nChannelConfig][2] == 1 ? true : false);
    this->m_StcLabelSL.SetWindowText(szChannelConfigNames[this->nChannelConfig][(nChannelConfigStates[this->nChannelConfig][3] == 1) ? 3 : 4]);
    this->m_StcLabelSL.SetBold(nChannelConfigStates[this->nChannelConfig][(nChannelConfigStates[this->nChannelConfig][3] == 1) ? 3 : 4] ? true : false);
    this->m_StcLabelSR.SetWindowText(szChannelConfigNames[this->nChannelConfig][5]);
    this->m_StcLabelSR.SetBold(nChannelConfigStates[this->nChannelConfig][5] == 1 ? true : false);
    this->m_StcLabelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));
    this->m_StcLabelLFE.SetBold(this->bLFE);

    this->m_ChkChannelConfigLFE.SetCheck(this->bLFE ? BST_CHECKED : BST_UNCHECKED);

    this->m_BtnChannelFL.SetBold((szInputFiles[0].GetLength() > 0) ? true : false);
    this->m_BtnChannelFR.SetBold((szInputFiles[1].GetLength() > 0) ? true : false);
    this->m_BtnChannelFC.SetBold((szInputFiles[2].GetLength() > 0) ? true : false);

    this->m_BtnChannelLFE.SetBold((szInputFiles[3].GetLength() > 0) ? true : false);

    if (nChannelConfigStates[nChannelConfig][3] == 1)
    {
        this->m_BtnChannelS.SetBold((szInputFiles[4].GetLength() > 0) ? true : false);
        this->m_BtnChannelSL.SetBold(false);
        this->m_BtnChannelSL.SetWindowText(_T("-"));
        this->m_BtnChannelSL.EnableWindow(FALSE);
    }
    else
    {
        this->m_BtnChannelSL.SetBold((szInputFiles[4].GetLength() > 0) ? true : false);
        this->m_BtnChannelS.SetBold(false);
        this->m_BtnChannelS.SetWindowText(_T("-"));
        this->m_BtnChannelS.EnableWindow(FALSE);
    }

    this->m_BtnChannelSR.SetBold((szInputFiles[5].GetLength() > 0) ? true : false);
}

bool CMuxDlg::LoadFilesList(CString &szFileName)
{
    for (int i = 0; i < CEncoderDefaults::nNumMaxInputFiles; i++)
        szTmpInputFiles[i] = _T("");

    try
    {
        FILE *fs;
        errno_t error = _tfopen_s(&fs, szFileName, _T("rt, ccs=UTF-8"));
        if (error != 0)
            return false;

        CStdioFile fp(fs);
        CString szBuffer = _T("");
        int nFileCounter = 0;

        while (fp.ReadString(szBuffer))
        {
            if (nFileCounter >= CEncoderDefaults::nNumMaxInputFiles)
            {
                fp.Close();
                return true;
            }

            szBuffer.TrimLeft('"');
            szBuffer.TrimRight('"');
            szTmpInputFiles[nFileCounter] = szBuffer;
            nFileCounter++;
            szBuffer = _T("");
        }

        int nReqNumOfFiles = nNumInputFiles[this->nChannelConfig] + (this->bLFE == true ? 1 : 0);
        if (nFileCounter != nReqNumOfFiles)
        {
            fp.Close();
            return false;
        }
        else
        {
            this->RemapFilesToChannels();
            fp.Close();
            return true;
        }
    }
    catch (...)
    {
        return false;
    }
}

bool CMuxDlg::SaveFilesList(CString &szFileName, int nFormat)
{
    try
    {
        FILE *fs;
        errno_t error = _tfopen_s(&fs, szFileName, _T("wt, ccs=UTF-8"));
        if (error != 0)
            return false;

        CStdioFile fp(fs);
        CString szBuffer;
        CString szFileName;

#define WriteToFile(index) \
        szBuffer.Format(_T("%s%s%s\r\n"), \
            nFormat == 0 ? _T("") : _T("\""), \
            this->szInputFiles[index], nFormat == 0 ? _T("") : _T("\"")); \
        fp.WriteString(szBuffer);

        switch (this->nChannelConfig)
        {
        case 0:
            WriteToFile(0);
            WriteToFile(1);
            WriteToFile(3);
            break;
        case 1:
            WriteToFile(2);
            WriteToFile(3);
            break;
        case 2:
            WriteToFile(0);
            WriteToFile(1);
            WriteToFile(3);
            break;
        case 3:
            WriteToFile(0);
            WriteToFile(1);
            WriteToFile(2);
            WriteToFile(3);
            break;
        case 4:
            WriteToFile(0);
            WriteToFile(1);
            if (this->bLFE == true)
            {
                WriteToFile(3);
                WriteToFile(4);
            }
            else
            {
                WriteToFile(4);
            }
            break;
        case 5:
            WriteToFile(0);
            WriteToFile(1);
            WriteToFile(2)
                if (this->bLFE == true)
                {
                    WriteToFile(3);
                    WriteToFile(4);
                }
                else
                {
                    WriteToFile(4);
                }
            break;
        case 6:
            WriteToFile(0);
            WriteToFile(1);
            if (this->bLFE == true)
            {
                WriteToFile(4);
                WriteToFile(5);
                WriteToFile(3);
            }
            else
            {
                WriteToFile(4);
                WriteToFile(5);
            }
            break;
        case 7:
            WriteToFile(0);
            WriteToFile(1);
            WriteToFile(2);
            if (this->bLFE == true)
            {
                WriteToFile(3);
                WriteToFile(4);
                WriteToFile(5);
            }
            else
            {
                WriteToFile(4);
                WriteToFile(5);
            }
            break;
        };

        fp.Close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void CMuxDlg::ShowOpenFileDlg(int nID, CMyButton *m_BtnCurrent, CMyEdit *m_EdtCurrent)
{
    if (m_BtnCurrent == nullptr || m_EdtCurrent == nullptr)
        return;

    CString szCurrentFileName;

    m_EdtCurrent->GetWindowText(szCurrentFileName);
    szCurrentFileName = GetFileName(szCurrentFileName);

    CFileDialog fd(TRUE,
        CEncoderDefaults::szSupportedInputExt[0],
        szCurrentFileName,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        CEncoderDefaults::GetSupportedInputFilesFilter(),
        this);

    if (fd.DoModal() == IDOK)
    {
        CString szFileName = fd.GetPathName();
        m_EdtCurrent->SetWindowText(szFileName);
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
        theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C01011) :
        _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"),
        this);

    if (fd.DoModal() == IDOK)
    {
        CString szFileName = fd.GetPathName();

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
        theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00C01011) :
        _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"),
        this);

    if (fd.DoModal() == IDOK)
    {
        CString szFileName = fd.GetPathName();
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
    for (int i = 0; i < CEncoderDefaults::nNumMaxInputFiles; i++)
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
