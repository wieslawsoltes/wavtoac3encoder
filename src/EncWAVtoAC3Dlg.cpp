//
// WAV to AC3 Encoder
// Copyright (C) 2007-2016 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

CEncWAVtoAC3Dlg::CEncWAVtoAC3Dlg(CWnd* pParent /*=NULL*/)
: CResizeDialog(CEncWAVtoAC3Dlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // file list sort flags
    this->nSortOrder[0] = true;
    this->nSortOrder[1] = true;

    // output file/path
    this->szOutputPath = _T("");
    this->szOutputFile = _T("");

    // multiple mono input
    this->bMultipleMonoInput = false;

    // visibility of main window
    this->bVisible = false;

    // disable all warning messages
    this->bDisableAllWarnings = false;

    // save configuration on exit
    this->bSaveConfig = true;
}

void CEncWAVtoAC3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_LstFiles);
	DDX_Control(pDX, IDC_LIST_SETTINGS, m_LstSettings);
	DDX_Control(pDX, IDC_SLIDER_BITRATE, m_SldBitrate);
	DDX_Control(pDX, IDC_STATIC_BITRATE, m_StcBitrate);
	DDX_Control(pDX, IDC_STATIC_QUALITY, m_StcQualityBitrate);
	DDX_Control(pDX, IDC_STATIC_OPTION_VALUE, m_StcSelected);
	DDX_Control(pDX, IDC_COMBO_SETTING, m_CmbValue);
	DDX_Control(pDX, IDC_COMBO_PRESETS, m_CmbPresets);
	DDX_Control(pDX, IDC_COMBO_RAW_SAMPLE_FORMAT, m_CmbRawSampleFormat);
	DDX_Control(pDX, IDC_COMBO_ENGINES, m_CmbEngines);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_PATH, m_EdtOutPath);
	DDX_Control(pDX, IDC_EDIT_THREADS, m_EdtThreads);
	DDX_Control(pDX, IDC_EDIT_RAW_SAMPLE_RATE, m_EdtRawSamplerate);
	DDX_Control(pDX, IDC_EDIT_RAW_CHANNELS, m_EdtRawChannels);
	DDX_Control(pDX, IDC_SPIN_THREADS, m_SpnThreads);
	DDX_Control(pDX, IDC_SPIN_RAW_SAMPLE_RATE, m_SpnRawSampleRate);
	DDX_Control(pDX, IDC_SPIN_RAW_CHANNELS, m_SpnRawChannels);
	DDX_Control(pDX, IDC_CHECK_VBR, m_ChkVbr);
	DDX_Control(pDX, IDC_CHECK_SIMD_MMX, m_ChkSimdMMX);
	DDX_Control(pDX, IDC_CHECK_SIMD_SSE, m_ChkSimdSSE);
	DDX_Control(pDX, IDC_CHECK_SIMD_SSE2, m_ChkSimdSSE2);
	DDX_Control(pDX, IDC_CHECK_SIMD_SSE3, m_ChkSimdSSE3);
	DDX_Control(pDX, IDC_CHECK_MULTIPLE_MONO_INPUT, m_ChkMultipleMonoInput);
	DDX_Control(pDX, IDC_BUTTON_ENCODE, m_BtnEncode);
	DDX_Control(pDX, IDC_BUTTON_PRESETS_DEFAULTS, m_BtnResetCurrent);
	DDX_Control(pDX, IDC_BUTTON_PRESET_DEL, m_BtnRemove);
	DDX_Control(pDX, IDC_BUTTON_PRESET_ADD, m_BtnAddNew);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_BtnAddFiles);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_BtnBrowse);
	DDX_Control(pDX, IDC_BUTTON_MUX_WIZARD, m_BtnMuxWizard);
	DDX_Control(pDX, IDC_BUTTON_ENGINES, m_BtnEngines);
}

BEGIN_MESSAGE_MAP(CEncWAVtoAC3Dlg, CResizeDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_WINDOWPOSCHANGING()
    //}}AFX_MSG_MAP
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_DROPFILES()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_COMMAND(ID_FILE_ADDFILES, &CEncWAVtoAC3Dlg::OnFileAddFiles)
    ON_COMMAND(ID_FILE_ADDDIRECTORY, &CEncWAVtoAC3Dlg::OnFileAddDirectory)
    ON_COMMAND(ID_FILE_MUXWIZARD, &CEncWAVtoAC3Dlg::OnFileMuxWizard)
    ON_COMMAND(ID_FILE_LOADPRESETS, &CEncWAVtoAC3Dlg::OnFileLoadPresets)
    ON_COMMAND(ID_FILE_SAVEPRESETS, &CEncWAVtoAC3Dlg::OnFileSavePresets)
    ON_COMMAND(ID_FILE_LOADFILESLIST, &CEncWAVtoAC3Dlg::OnFileLoadFilesList)
    ON_COMMAND(ID_FILE_SAVEFILESLIST, &CEncWAVtoAC3Dlg::OnFileSaveFilesList)
    ON_COMMAND(ID_FILE_EXIT, &CEncWAVtoAC3Dlg::OnFileExit)
    ON_COMMAND(ID_OPTIONS_DISABLEALLWARNINGS, &CEncWAVtoAC3Dlg::OnOptionsDisableAllWarnings)
    ON_COMMAND(ID_OPTIONS_SAVECONFIGURATIONONEXIT, &CEncWAVtoAC3Dlg::OnOptionsSaveConfigurationOnExit)
    ON_COMMAND(ID_OPTIONS_SAVECONFIGURATION, &CEncWAVtoAC3Dlg::OnOptionsSaveConfiguration)
    ON_COMMAND(ID_OPTIONS_LOADCONFIGURATION, &CEncWAVtoAC3Dlg::OnOptionsLoadConfiguration)
	ON_COMMAND(ID_LANGUAGE_DEFAULT, &CEncWAVtoAC3Dlg::OnLanguageChangeDefault)
	ON_COMMAND_RANGE(ID_LANGUAGE_MENU_START, ID_LANGUAGE_MENU_MAX, &CEncWAVtoAC3Dlg::OnLanguageChange)
    ON_COMMAND(ID_HELP_COMMAND_LINE, &CEncWAVtoAC3Dlg::OnHelpCommandLine)
    ON_COMMAND(ID_HELP_WEBSITE, &CEncWAVtoAC3Dlg::OnHelpWebsite)
    ON_COMMAND(ID_HELP_ABOUT, &CEncWAVtoAC3Dlg::OnHelpAbout)
    ON_COMMAND(ID_LIST_ADDFILES, &CEncWAVtoAC3Dlg::OnListAddFiles)
    ON_COMMAND(ID_LIST_ADDDIRECTORY, &CEncWAVtoAC3Dlg::OnListAddDirectory)
    ON_COMMAND(ID_LIST_MUXWIZARD, &CEncWAVtoAC3Dlg::OnListMuxWizard)
    ON_COMMAND(ID_LIST_LOADLIST, &CEncWAVtoAC3Dlg::OnListLoadList)
    ON_COMMAND(ID_LIST_SAVELIST, &CEncWAVtoAC3Dlg::OnListSavelist)
    ON_COMMAND(ID_LIST_DELFILES, &CEncWAVtoAC3Dlg::OnListDelFiles)
    ON_COMMAND(ID_LIST_CLEARLIST, &CEncWAVtoAC3Dlg::OnListClearList)
    ON_COMMAND(ID_LIST_MOVEUP, &CEncWAVtoAC3Dlg::OnListMoveUp)
    ON_COMMAND(ID_LIST_MOVEDOWN, &CEncWAVtoAC3Dlg::OnListMoveDown)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &CEncWAVtoAC3Dlg::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_ENCODE, &CEncWAVtoAC3Dlg::OnBnClickedButtonEncode)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CEncWAVtoAC3Dlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_PRESETS_DEFAULTS, &CEncWAVtoAC3Dlg::OnBnClickedButtonPresetsDefaults)
    ON_BN_CLICKED(IDC_BUTTON_PRESET_ADD, &CEncWAVtoAC3Dlg::OnBnClickedButtonPresetAdd)
    ON_BN_CLICKED(IDC_BUTTON_PRESET_DEL, &CEncWAVtoAC3Dlg::OnBnClickedButtonPresetDel)
    ON_BN_CLICKED(IDC_CHECK_VBR, &CEncWAVtoAC3Dlg::OnBnClickedCheckVbr)
    ON_BN_CLICKED(IDC_CHECK_SIMD_MMX, &CEncWAVtoAC3Dlg::OnBnClickedCheckSimdMmx)
    ON_BN_CLICKED(IDC_CHECK_SIMD_SSE, &CEncWAVtoAC3Dlg::OnBnClickedCheckSimdSse)
    ON_BN_CLICKED(IDC_CHECK_SIMD_SSE2, &CEncWAVtoAC3Dlg::OnBnClickedCheckSimdSse2)
    ON_BN_CLICKED(IDC_CHECK_SIMD_SSE3, &CEncWAVtoAC3Dlg::OnBnClickedCheckSimdSse3)
    ON_BN_CLICKED(IDC_CHECK_MULTIPLE_MONO_INPUT, &CEncWAVtoAC3Dlg::OnBnClickedCheckMultipleMonoInput)
    ON_BN_CLICKED(IDC_BUTTON_MUX_WIZARD, &CEncWAVtoAC3Dlg::OnBnClickedButtonMuxWizard)
    ON_CBN_SELCHANGE(IDC_COMBO_SETTING, &CEncWAVtoAC3Dlg::OnCbnSelchangeComboSetting)
    ON_CBN_SELCHANGE(IDC_COMBO_PRESETS, &CEncWAVtoAC3Dlg::OnCbnSelchangeComboPresets)
    ON_CBN_SELCHANGE(IDC_COMBO_ENGINES, &CEncWAVtoAC3Dlg::OnCbnSelchangeComboEngines)
    ON_CBN_SELCHANGE(IDC_COMBO_RAW_SAMPLE_FORMAT, &CEncWAVtoAC3Dlg::OnCbnSelchangeComboRawSampleFormat)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SETTINGS, &CEncWAVtoAC3Dlg::OnLvnItemchangedListSettings)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FILES, &CEncWAVtoAC3Dlg::OnLvnKeydownListFiles)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SETTINGS, &CEncWAVtoAC3Dlg::OnLvnKeydownListSettings)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_FILES, &CEncWAVtoAC3Dlg::OnLvnColumnclickListFiles)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_FILES, &CEncWAVtoAC3Dlg::OnNMRclickListFiles)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_SETTINGS, &CEncWAVtoAC3Dlg::OnNMRclickListSettings)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SETTINGS, &CEncWAVtoAC3Dlg::OnNMDblclkListSettings)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILES, &CEncWAVtoAC3Dlg::OnNMDblclkListFiles)
    ON_EN_CHANGE(IDC_EDIT_THREADS, &CEncWAVtoAC3Dlg::OnEnChangeEditThreads)
    ON_EN_CHANGE(IDC_EDIT_RAW_SAMPLE_RATE, &CEncWAVtoAC3Dlg::OnEnChangeEditRawSampleRate)
    ON_EN_CHANGE(IDC_EDIT_RAW_CHANNELS, &CEncWAVtoAC3Dlg::OnEnChangeEditRawChannels)
    ON_EN_CHANGE(IDC_EDIT_OUTPUT_PATH, &CEncWAVtoAC3Dlg::OnEnChangeEditOutputPath)
    ON_EN_SETFOCUS(IDC_EDIT_OUTPUT_PATH, &CEncWAVtoAC3Dlg::OnEnSetfocusEditOutputPath)
    ON_EN_SETFOCUS(IDC_EDIT_RAW_SAMPLE_RATE, &CEncWAVtoAC3Dlg::OnEnSetfocusEditRawSampleRate)
    ON_EN_SETFOCUS(IDC_EDIT_RAW_CHANNELS, &CEncWAVtoAC3Dlg::OnEnSetfocusEditRawChannels)
    ON_EN_SETFOCUS(IDC_EDIT_THREADS, &CEncWAVtoAC3Dlg::OnEnSetfocusEditThreads)
    ON_EN_KILLFOCUS(IDC_EDIT_OUTPUT_PATH, &CEncWAVtoAC3Dlg::OnEnKillfocusEditOutputPath)
    ON_EN_KILLFOCUS(IDC_EDIT_RAW_SAMPLE_RATE, &CEncWAVtoAC3Dlg::OnEnKillfocusEditRawSampleRate)
    ON_EN_KILLFOCUS(IDC_EDIT_RAW_CHANNELS, &CEncWAVtoAC3Dlg::OnEnKillfocusEditRawChannels)
    ON_EN_KILLFOCUS(IDC_EDIT_THREADS, &CEncWAVtoAC3Dlg::OnEnKillfocusEditThreads)
    ON_MESSAGE(WM_MY_EN_CHANGE, EditChangeComboPresets)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES, &CEncWAVtoAC3Dlg::OnBnClickedButtonEngines)
END_MESSAGE_MAP()

void CEncWAVtoAC3Dlg::OnPaint()
{
    if(IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CResizeDialog::OnPaint();
    }
}

HCURSOR CEncWAVtoAC3Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CEncWAVtoAC3Dlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    // by default hide main window
    if(this->bVisible == false)
        lpwndpos->flags &= ~SWP_SHOWWINDOW;

    CResizeDialog::OnWindowPosChanging(lpwndpos);
}

void CEncWAVtoAC3Dlg::UpdateView(int nMode)
{
    // reset dialog anchors
    this->InitDialogAnchors();
}

void CEncWAVtoAC3Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    this->UpdateBitrateText();

    CResizeDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEncWAVtoAC3Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if(pScrollBar->GetDlgCtrlID() == IDC_SPIN_THREADS)
    {
        // update threads number
        CString szBuff;

        if(nPos == 0)
            szBuff = DEFAULT_TEXT_AUTO;
        else
            szBuff.Format(_T("%d"), nPos);

        this->m_EdtThreads.SetWindowText(szBuff);
    }
    if(pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_SAMPLE_RATE)
    {
        // update sample rate number
        CString szBuff;

        if(nPos == 0)
            szBuff = DEFAULT_TEXT_IGNORED;
        else
            szBuff.Format(_T("%d"), nPos);

        this->m_EdtRawSamplerate.SetWindowText(szBuff);
    }
    if(pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_CHANNELS)
    {
        // update channels number
        CString szBuff;

        if(nPos == 0)
            szBuff = DEFAULT_TEXT_IGNORED;
        else
            szBuff.Format(_T("%d"), nPos);

        this->m_EdtRawChannels.SetWindowText(szBuff);
    }
    else
    {
        CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
    }

    CResizeDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEncWAVtoAC3Dlg::OnClose()
{
    // save configuration to disk (does not work in read-only mode)
    if(this->bSaveConfig == true)
        this->SaveAllConfiguration();

    CResizeDialog::OnClose();
}

void CEncWAVtoAC3Dlg::OnDestroy()
{
    CResizeDialog::OnDestroy();

    // clean-up memory used by tooltips
    this->m_LstFiles.DeleteAllItems();
    this->m_LstSettings.DeleteAllItems();
}

BOOL CEncWAVtoAC3Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccelTable)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
			return(TRUE);
	}

	return CResizeDialog::PreTranslateMessage(pMsg);
}
