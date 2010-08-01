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
#include "EncWAVtoAC3MuxDlg.h"
#include "Utilities.h"
#include "MyFile.h"

// Correct channel to file mapping:
// FL    -> szInputFiles[0]
// FR    -> szInputFiles[1]
// C     -> szInputFiles[2]
// LFE   -> szInputFiles[3]
// SL, S -> szInputFiles[4]
// SR    -> szInputFiles[5]

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

// currect number of input file for each channel config + LFE channel
const int nNumInputFiles[nNumChannelConfig] =
{
    2, 1, 2, 3, 3, 4, 4, 5
};

const int nChannelConfigStates[nNumChannelConfig][NUM_MAX_INPUT_FILES] = 
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

const CString szChannelConfigNames[nNumChannelConfig][NUM_MAX_INPUT_FILES] = 
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

IMPLEMENT_DYNAMIC(CEncWAVtoAC3MuxDlg, CDialogEx)

CEncWAVtoAC3MuxDlg::CEncWAVtoAC3MuxDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CEncWAVtoAC3MuxDlg::IDD, pParent)
{
    for(int i = 0; i < NUM_MAX_INPUT_FILES; i++)
    {
        this->szInputFiles[i] = _T("");
        this->szTmpInputFiles[i] = _T("");
    }

    this->nChannelConfig = nNumChannelConfig - 1;
    this->bLFE = true;
}

CEncWAVtoAC3MuxDlg::~CEncWAVtoAC3MuxDlg()
{

}

void CEncWAVtoAC3MuxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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

BEGIN_MESSAGE_MAP(CEncWAVtoAC3MuxDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_FL, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonFl)
    ON_BN_CLICKED(IDC_BUTTON_FR, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonFr)
    ON_BN_CLICKED(IDC_BUTTON_FC, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonFc)
    ON_BN_CLICKED(IDC_BUTTON_LFE, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonLfe)
    ON_BN_CLICKED(IDC_BUTTON_SL, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonSl)
    ON_BN_CLICKED(IDC_BUTTON_SR, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonSr)
    ON_BN_CLICKED(IDC_BUTTON_S, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonS)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_FL, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearFl)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_FC, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearFc)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_FR, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearFr)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_LFE, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearLfe)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_SL, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearSl)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_SR, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearSr)
    ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonExport)
    ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CEncWAVtoAC3MuxDlg::OnBnClickedButtonImport)
    ON_BN_CLICKED(IDCANCEL, &CEncWAVtoAC3MuxDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDOK, &CEncWAVtoAC3MuxDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_CHECK_CHANNEL_CONFIG_LFE, &CEncWAVtoAC3MuxDlg::OnBnClickedCheckChannelConfigLfe)
    ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL_CONFIG, &CEncWAVtoAC3MuxDlg::OnCbnSelchangeComboChannelConfig)
END_MESSAGE_MAP()

BOOL CEncWAVtoAC3MuxDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // init channel configuration ComboBox
    for(int i = 0; i < nNumChannelConfig; i++)
        this->m_CmbChannelConfig.InsertString(i, szChannelConfig[i]);

    this->m_CmbChannelConfig.SetCurSel(0);

    // set tooltips
    CString szTmpText;

    // channel config ComboBox
    szTmpText = 
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

    // +LFE CheckBox
    this->m_ChkChannelConfigLFE.SetTooltipText(_T("Indicates use of the LFE channel."));

    // set fixed height of combobox controls
    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_CHANNEL_CONFIG);

    // channel buttons
    this->m_BtnChannelFL.SetTooltipText(_T("Front Left Channel"));
    this->m_BtnChannelFR.SetTooltipText(_T("Front Right Channel"));
    this->m_BtnChannelFC.SetTooltipText(_T("Front Center Channel"));
    this->m_BtnChannelLFE.SetTooltipText(_T("Low Frequency Effect Channel"));
    this->m_BtnChannelSL.SetTooltipText(_T("Surround Left Channel"));
    this->m_BtnChannelSR.SetTooltipText(_T("Surround Right Channel"));
    this->m_BtnChannelS.SetTooltipText(_T("Surround Channel"));

    this->m_CmbChannelConfig.SetCurSel(this->nChannelConfig);

    // set file paths
    this->RemapFilesToChannels();
    this->SetFilePaths();

    // update state of all controls
    this->SetControlsState();

    return TRUE;
}

void CEncWAVtoAC3MuxDlg::RemapFilesToChannels()
{
    for(int i = 0; i < NUM_MAX_INPUT_FILES; i++)
        this->szInputFiles[i] = _T("");

    switch(this->nChannelConfig)
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

        if(this->bLFE == true)
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

        if(this->bLFE == true)
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

        if(this->bLFE == true)
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

        if(this->bLFE == true)
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

void CEncWAVtoAC3MuxDlg::SetFilePaths()
{
    this->m_EdtChannelFL.SetWindowText(szInputFiles[0]);
    this->m_EdtChannelFR.SetWindowText(szInputFiles[1]);
    this->m_EdtChannelFC.SetWindowText(szInputFiles[2]);
    this->m_EdtChannelLFE.SetWindowText(szInputFiles[3]);
    this->m_EdtChannelSL.SetWindowText(szInputFiles[4]);
    this->m_EdtChannelSR.SetWindowText(szInputFiles[5]);
}

void CEncWAVtoAC3MuxDlg::SetControlsState()
{
    // enable/disable buttons
    this->m_BtnChannelFL.EnableWindow(nChannelConfigStates[this->nChannelConfig][0] == 1 ? TRUE : FALSE);
    this->m_BtnChannelFR.EnableWindow(nChannelConfigStates[this->nChannelConfig][1] == 1 ? TRUE : FALSE);
    this->m_BtnChannelFC.EnableWindow(nChannelConfigStates[this->nChannelConfig][2] == 1 ? TRUE : FALSE);
    this->m_BtnChannelS.EnableWindow(nChannelConfigStates[this->nChannelConfig][3] == 1 ? TRUE : FALSE);
    this->m_BtnChannelSL.EnableWindow(nChannelConfigStates[this->nChannelConfig][4] == 1 ? TRUE : FALSE);
    this->m_BtnChannelSR.EnableWindow(nChannelConfigStates[this->nChannelConfig][5] == 1 ? TRUE : FALSE);
    this->m_BtnChannelLFE.EnableWindow(this->bLFE ? TRUE : FALSE);

    // set buttons text
    this->m_BtnChannelFL.SetWindowText(szChannelConfigNames[this->nChannelConfig][0]);
    this->m_BtnChannelFR.SetWindowText(szChannelConfigNames[this->nChannelConfig][1]);
    this->m_BtnChannelFC.SetWindowText(szChannelConfigNames[this->nChannelConfig][2]);
    this->m_BtnChannelS.SetWindowText(szChannelConfigNames[this->nChannelConfig][3]);
    this->m_BtnChannelSL.SetWindowText(szChannelConfigNames[this->nChannelConfig][4]);
    this->m_BtnChannelSR.SetWindowText(szChannelConfigNames[this->nChannelConfig][5]);
    this->m_BtnChannelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));

    // set labels text
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

    // set LFE channel state
    this->m_ChkChannelConfigLFE.SetCheck(this->bLFE ? BST_CHECKED : BST_UNCHECKED);

    // set bold state
    this->m_BtnChannelFL.SetBold((szInputFiles[0].GetLength() > 0) ? true : false);
    this->m_BtnChannelFR.SetBold((szInputFiles[1].GetLength() > 0) ? true : false);
    this->m_BtnChannelFC.SetBold((szInputFiles[2].GetLength() > 0) ? true : false);

    this->m_BtnChannelLFE.SetBold((szInputFiles[3].GetLength() > 0) ? true : false);

    if(nChannelConfigStates[nChannelConfig][3] == 1)
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

bool CEncWAVtoAC3MuxDlg::LoadFilesList(CString &szFileName)
{
    // reset input files
    for(int i = 0; i < NUM_MAX_INPUT_FILES; i++)
        szTmpInputFiles[i] = _T("");

    try
    {
        CMyFile fp;
        if(fp.FOpen(szFileName, false) == false)
            return false;

        ULONGLONG  nRead = 0, nLength = fp.FSize();
        if(nLength == 0)
        {
            fp.FClose();
            return true;
        }

        TCHAR Buffer;
        CString szBuffer = _T("");
        int nFileCounter = 0;

        while(fp.FRead(Buffer) == true)
        {
            if((Buffer != '\r') && (Buffer != '\n'))
                szBuffer += Buffer;

            if(Buffer == '\n' || nRead == nLength - (fp.FMode() == 1 ? 1: sizeof(TCHAR)))
            {
                szBuffer += _T("\0");

                // terminate reading if max of input files is reached
                if(nFileCounter >= NUM_MAX_INPUT_FILES)
                {
                    fp.FClose();
                    return true;
                }

                // remove leading nad trailing quotes (used for *.mux file format)
                szBuffer.TrimLeft('"');
                szBuffer.TrimRight('"');

                szTmpInputFiles[nFileCounter] = szBuffer;

                // update file counter
                nFileCounter++;

                // reset buffer
                szBuffer = _T("");
            }

            nRead++;
        }

        int nReqNumOfFiles = nNumInputFiles[this->nChannelConfig] + (this->bLFE == true ? 1 : 0);
        if(nFileCounter != nReqNumOfFiles)
        {
            fp.FClose();
            return false;
        }
        else
        {
            this->RemapFilesToChannels();

            fp.FClose();
            return true;
        }
    }
    catch(...)
    {
        return false;
    }
}

bool CEncWAVtoAC3MuxDlg::SaveFilesList(CString &szFileName, int nFormat)
{
    // format
    // 0 (*.files) -> file_path+ext
    // 1 (*.mux)   -> "file_path+ext"
    try
    {
        CMyFile fp;

        // write *.mux files always in Ansi format
        if(nFormat == 1)
        {
#ifdef _UNICODE
            fp.FSetMode(2);
#else
            fp.FSetMode(3);
#endif
        }

        if(fp.FOpen(szFileName, true) == false)
            return false;

        CString szBuffer;
        CString szFileName;

        #define WriteToFile(index) \
            szBuffer.Format(_T("%s%s%s\r\n"), \
                nFormat == 0 ? _T("") : _T("\""), \
                this->szInputFiles[index], nFormat == 0 ? _T("") : _T("\"")); \
            fp.FWriteString(szBuffer.GetBuffer(), szBuffer.GetLength()); \
            szBuffer.ReleaseBuffer();

        switch(this->nChannelConfig)
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
            if(this->bLFE == true)
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
                if(this->bLFE == true)
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
            if(this->bLFE == true)
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
            if(this->bLFE == true)
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

        fp.FClose();
    }
    catch(...)
    {
        return false;
    }

    return true;
}

void CEncWAVtoAC3MuxDlg::ShowOpenFileDlg(int nID, CMyButton *m_BtnCurrent, CMyEdit *m_EdtCurrent)
{
    if(m_BtnCurrent == NULL || m_EdtCurrent == NULL)
        return;

    CString szCurrentFileName;
    
    m_EdtCurrent->GetWindowText(szCurrentFileName);
    szCurrentFileName = GetFileName(szCurrentFileName);

    // get input file filter
    CString szFilter = GetSupportedInputFilesFilter();

    CFileDialog fd(TRUE, 
        szSupportedInputExt[0], 
        szCurrentFileName, 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        szFilter, 
        this);

    if(fd.DoModal() == IDOK)
    {
        CString szFileName = fd.GetPathName();

        m_EdtCurrent->SetWindowText(szFileName);
        this->szInputFiles[nID] = szFileName;
        m_BtnCurrent->SetBold(true);
    }
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonFl()
{
    this->ShowOpenFileDlg(0, &this->m_BtnChannelFL, &this->m_EdtChannelFL);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonFr()
{
    this->ShowOpenFileDlg(1, &this->m_BtnChannelFR, &this->m_EdtChannelFR);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonFc()
{
    this->ShowOpenFileDlg(2, &this->m_BtnChannelFC, &this->m_EdtChannelFC);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonLfe()
{
    this->ShowOpenFileDlg(3, &this->m_BtnChannelLFE, &this->m_EdtChannelLFE);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonSl()
{
    this->ShowOpenFileDlg(4, &this->m_BtnChannelSL, &this->m_EdtChannelSL);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonSr()
{
    this->ShowOpenFileDlg(5, &this->m_BtnChannelSR, &this->m_EdtChannelSR);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonS()
{
    this->ShowOpenFileDlg(4, &this->m_BtnChannelS, &this->m_EdtChannelSL);
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearFl()
{
    this->m_BtnChannelFL.SetBold(false);
    this->m_EdtChannelFL.SetWindowText(_T(""));
    this->szInputFiles[0] = _T("");

    this->SetControlsState();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearFc()
{
    this->m_BtnChannelFC.SetBold(false);
    this->m_EdtChannelFC.SetWindowText(_T(""));
    this->szInputFiles[2] = _T("");

    this->SetControlsState();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearFr()
{
    this->m_BtnChannelFR.SetBold(false);
    this->m_EdtChannelFR.SetWindowText(_T(""));
    this->szInputFiles[1] = _T("");

    this->SetControlsState();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearLfe()
{
    this->m_BtnChannelLFE.SetBold(false);
    this->m_EdtChannelLFE.SetWindowText(_T(""));
    this->szInputFiles[3] = _T("");

    this->SetControlsState();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearSl()
{

    if(nChannelConfigStates[nChannelConfig][3] == 1)
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

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonClearSr()
{
    this->m_BtnChannelSR.SetBold(false);
    this->m_EdtChannelSR.SetWindowText(_T(""));
    this->szInputFiles[5] = _T("");

    this->SetControlsState();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonExport()
{
    CFileDialog fd(FALSE, 
        _T("files"), 
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        CString szFileName = fd.GetPathName();

        int nFormat = 0;
        if(fd.GetFileExt().CompareNoCase(_T("files")) == 0)
            nFormat = 0;
        else if(fd.GetFileExt().CompareNoCase(_T("mux")) == 0)
            nFormat = 1;

        this->SaveFilesList(szFileName, nFormat);
    }
}

void CEncWAVtoAC3MuxDlg::OnBnClickedButtonImport()
{
    CFileDialog fd(TRUE, 
        _T("files"), 
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        CString szFileName = fd.GetPathName();

        this->LoadFilesList(szFileName);
        this->SetFilePaths();
        this->SetControlsState();
    }
}

void CEncWAVtoAC3MuxDlg::OnBnClickedCheckChannelConfigLfe()
{
    const int bCheck = this->m_ChkChannelConfigLFE.GetCheck();

    this->bLFE = (bCheck == BST_CHECKED) ? true : false;

    this->m_BtnChannelLFE.EnableWindow(this->bLFE ? TRUE : FALSE);
    this->m_BtnChannelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));
    this->m_StcLabelLFE.SetWindowText(this->bLFE ? _T("LFE") : _T("-"));
    this->m_StcLabelLFE.SetBold(this->bLFE);

    this->SetControlsState();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedCancel()
{
    for(int i = 0; i < NUM_MAX_INPUT_FILES; i++)
    {
        this->szInputFiles[i] = _T("");
        this->szTmpInputFiles[i] = _T("");
    }

    OnCancel();
}

void CEncWAVtoAC3MuxDlg::OnBnClickedOk()
{
    // clear unused channels
    switch(this->nChannelConfig)
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
        if(this->bLFE == true)
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
        if(this->bLFE == true)
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
        if(this->bLFE == true)
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
        if(this->bLFE == false)
        {
            this->szInputFiles[3] = _T("");
        }
        break;
    };

    OnOK();
}

void CEncWAVtoAC3MuxDlg::OnCbnSelchangeComboChannelConfig()
{
    int nSel = this->m_CmbChannelConfig.GetCurSel();
    if(nSel != CB_ERR)
    {
        this->nChannelConfig = nSel;

        this->SetControlsState();
    }
}
