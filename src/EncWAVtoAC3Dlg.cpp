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

// support functions for drag&drop handler
typedef struct TDRAGANDDROP
{
    CEncWAVtoAC3Dlg *pDlg;
    HDROP hDropInfo;
} DRAGANDDROP, *PDRAGANDDROP;

static volatile bool bHandleDrop = true;
static HANDLE hDDThread;
static DWORD dwDDThreadID;
static DRAGANDDROP m_DDParam;

DWORD WINAPI DragAndDropThread(LPVOID lpParam)
{
    PDRAGANDDROP m_ThreadParam = (PDRAGANDDROP) lpParam;

    m_ThreadParam->pDlg->HandleDropFiles(m_ThreadParam->hDropInfo);
    bHandleDrop = true;

    return ::CloseHandle(hDDThread);
}

int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData)
{
    CEncWAVtoAC3Dlg *pDlg = (CEncWAVtoAC3Dlg *) lParamData;

    LVFINDINFO pInfo1, pInfo2;

    pInfo1.flags = LVFI_PARAM;
    pInfo2.flags = LVFI_PARAM;
    pInfo1.lParam = lParam1;
    pInfo2.lParam = lParam2;

    int nIndex1 = pDlg->m_LstFiles.FindItem(&pInfo1);
    int nIndex2 = pDlg->m_LstFiles.FindItem(&pInfo2);

    CString szItem1 = pDlg->m_LstFiles.GetItemText(nIndex1, pDlg->nSortColumn);
    CString szItem2 = pDlg->m_LstFiles.GetItemText(nIndex2, pDlg->nSortColumn);

    if(pDlg->nSortOrder[pDlg->nSortColumn] == true)
        return szItem1.Compare(szItem2);

    if(pDlg->nSortOrder[pDlg->nSortColumn] == false)
        return szItem2.Compare(szItem1);

    return 0;
}

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

    // text file log
    this->bEnableLog = true;
    this->szLogFile = _T("");

    // visibility of main window
    this->bVisible = false;

    // disable all warning messages
    this->bDisableAllWarnings = false;

    // save configuration on exit
    this->bSaveConfig = true;

    // default options view mode
    this->nViewMode = VIEW_MODE_STANDARD;

    // set logging defaults
    ::LogDefaults(&logCtx);
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
    DDX_Control(pDX, IDC_CHECK_ADVANCED_VIEW, m_ChkAdvancedView);
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
    ON_COMMAND(ID_OPTIONS_ENABLELOGGING, &CEncWAVtoAC3Dlg::OnOptionsEnableLogging)
    ON_COMMAND(ID_OPTIONS_OPENLOGFILE, &CEncWAVtoAC3Dlg::OnOptionsOpenLogFile)
    ON_COMMAND(ID_OPTIONS_SETLOGFILEPATH, &CEncWAVtoAC3Dlg::OnOptionsSetLogFilePath)
    ON_COMMAND(ID_OPTIONS_DISABLEALLWARNINGS, &CEncWAVtoAC3Dlg::OnOptionsDisableAllWarnings)
    ON_COMMAND(ID_OPTIONS_SAVECONFIGURATIONONEXIT, &CEncWAVtoAC3Dlg::OnOptionsSaveConfigurationOnExit)
    ON_COMMAND(ID_OPTIONS_SAVECONFIGURATION, &CEncWAVtoAC3Dlg::OnOptionsSaveConfiguration)
    ON_COMMAND(ID_OPTIONS_LOADCONFIGURATION, &CEncWAVtoAC3Dlg::OnOptionsLoadConfiguration)
    ON_COMMAND(ID_VIEW_STANDARD, &CEncWAVtoAC3Dlg::OnViewStandard)
    ON_COMMAND(ID_VIEW_ADVANCED, &CEncWAVtoAC3Dlg::OnViewAdvanced)
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
    ON_BN_CLICKED(IDC_CHECK_ADVANCED_VIEW, &CEncWAVtoAC3Dlg::OnBnClickedCheckAdvancedView)
END_MESSAGE_MAP()

void CEncWAVtoAC3Dlg::InitDialogAnchors()
{
    CleanUp();

    AddAnchor(IDC_STATIC_QUALITY, AnchorTopLeft);
    AddAnchor(IDC_STATIC_BITRATE, AnchorTopRight);
    AddAnchor(IDC_SLIDER_BITRATE, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_CHECK_VBR, AnchorTopRight);
    AddAnchor(IDC_STATIC_ENGINE, AnchorTopRight);
    AddAnchor(IDC_COMBO_ENGINES, AnchorTopRight);
    AddAnchor(IDC_BUTTON_ENGINES, AnchorTopRight);
    AddAnchor(IDC_STATIC_THREADS, AnchorTopRight);
    AddAnchor(IDC_EDIT_THREADS, AnchorTopRight);
    AddAnchor(IDC_SPIN_THREADS, AnchorTopRight);
    AddAnchor(IDC_LIST_SETTINGS, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_STATIC_OPTION_VALUE, AnchorTopLeft);
    AddAnchor(IDC_COMBO_SETTING, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_COMBO_PRESETS, AnchorTopRight);
    AddAnchor(IDC_CHECK_ADVANCED_VIEW, AnchorTopRight);
    AddAnchor(IDC_BUTTON_PRESETS_DEFAULTS, AnchorTopRight);
    AddAnchor(IDC_BUTTON_PRESET_DEL, AnchorTopRight);
    AddAnchor(IDC_BUTTON_PRESET_ADD, AnchorTopRight);
    AddAnchor(IDC_CHECK_SIMD_MMX, AnchorTopRight);
    AddAnchor(IDC_CHECK_SIMD_SSE, AnchorTopRight);
    AddAnchor(IDC_CHECK_SIMD_SSE2, AnchorTopRight);
    AddAnchor(IDC_CHECK_SIMD_SSE3, AnchorTopRight);
    AddAnchor(IDC_STATIC_SAMPLE_FORMAT, AnchorTopRight);
    AddAnchor(IDC_COMBO_RAW_SAMPLE_FORMAT, AnchorTopRight);
    AddAnchor(IDC_STATIC_SAMPLE_RATE, AnchorTopRight);
    AddAnchor(IDC_EDIT_RAW_SAMPLE_RATE, AnchorTopRight);
    AddAnchor(IDC_SPIN_RAW_SAMPLE_RATE, AnchorTopRight);
    AddAnchor(IDC_STATIC_CHANNELS, AnchorTopRight);
    AddAnchor(IDC_EDIT_RAW_CHANNELS, AnchorTopRight);
    AddAnchor(IDC_SPIN_RAW_CHANNELS, AnchorTopRight);
    AddAnchor(IDC_LIST_FILES, AnchorTopLeft, AnchorBottomRight);
    AddAnchor(IDC_BUTTON_ADD, AnchorBottomLeft);
    AddAnchor(IDC_STATIC_OUTPUT, AnchorBottomLeft);
    AddAnchor(IDC_EDIT_OUTPUT_PATH, AnchorBottomLeft, AnchorBottomRight);
    AddAnchor(IDC_BUTTON_BROWSE, AnchorBottomRight);
    AddAnchor(IDC_CHECK_MULTIPLE_MONO_INPUT, AnchorBottomLeft);
    AddAnchor(IDC_BUTTON_MUX_WIZARD, AnchorBottomLeft);
    AddAnchor(IDC_BUTTON_ENCODE, AnchorBottomRight);
    AddAnchor(IDC_STATUSBAR, AnchorBottomLeft, AnchorBottomRight);
    AddAnchor(IDC_STATIC_SETTINGS, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_STATIC_PARALLEL, AnchorTopRight);
    AddAnchor(IDC_STATIC_RAW, AnchorTopRight);
    AddAnchor(IDC_STATIC_SIMD, AnchorTopRight);
    AddAnchor(IDC_STATIC_PRESETS, AnchorTopRight);
}

BOOL CEncWAVtoAC3Dlg::OnInitDialog()
{
    CResizeDialog::OnInitDialog();

    // set dialog icons
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // set text style to normal or bold for static controls and buttons
    m_StcQualityBitrate.SetBold(false);
    m_StcBitrate.SetBold(true);
    m_StcSelected.SetBold(false);
    m_StcPreconfigured.SetBold(false);
    m_BtnEncode.SetBold(true);

    // create statusbar control
    VERIFY(m_StatusBar.Create(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
        CRect(0, 0, 0, 0), 
        this, 
        IDC_STATUSBAR));

    // status bar has only one pane
    // int nStatusBarParts[3] = { 150, 150, -1 };
    // m_StatusBar.SetParts(3, nStatusBarParts);

    // show program name and version in main dialog title
    CString szDialogTitle = _T("");
    szDialogTitle.Format(_T("WAV to AC3 Encoder %s"), ENCWAVTOAC3_VERSION);
    this->SetWindowText(szDialogTitle);

    // set range for work threads spiner
    m_SpnThreads.SetRange32(0, INT_MAX);
    m_SpnThreads.SetPos(0);

    // setup file listctrl
    // use LVS_SHAREIMAGELISTS to prevent 
    // deletion of image list under win9x
    // set this style under resource editor
    this->m_LstFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    this->m_LstFiles.InsertColumn(0, _T("File path"), LVCFMT_LEFT, 614, 0);
    this->m_LstFiles.InsertColumn(1, _T("File size (bytes)"), LVCFMT_LEFT, 100, 0);

    // initialize the image list for file listctrl
    SHFILEINFO sfi;

    HIMAGELIST m_ilLargeTmp;
    HIMAGELIST m_ilSmallTmp;

    TCHAR szSystemRoot[MAX_PATH + 1];
    GetWindowsDirectory(szSystemRoot, MAX_PATH);
    PathStripToRoot(szSystemRoot);

    m_ilLargeTmp = (HIMAGELIST) SHGetFileInfo(szSystemRoot,
        0,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ICON);

    m_ilSmallTmp = (HIMAGELIST) SHGetFileInfo(szSystemRoot,
        0,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_ICON);

    // set listctrl image lists
    ListView_SetImageList(this->m_LstFiles.GetSafeHwnd(), m_ilLargeTmp, LVSIL_NORMAL);
    ListView_SetImageList(this->m_LstFiles.GetSafeHwnd(), m_ilSmallTmp, LVSIL_SMALL);

	// setup settings listctrl
	HWND listView = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();
	ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT);
	ListView_EnableGroupView(listView, TRUE);

	// add Columns to settings listctrl
	LVCOLUMN lc;
	lc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;

	lc.iSubItem = 0;
	lc.cx = 255;
	lc.pszText = _T("Option");
	ListView_InsertColumn(listView, 0, &lc);

	lc.iSubItem = 1;
	lc.cx = 183;
	lc.pszText = _T("Value");
	ListView_InsertColumn(listView, 1, &lc);

	// add Groups to settings listctrl
	LVGROUP lg;
	lg.cbSize = sizeof(LVGROUP);
	lg.mask = LVGF_HEADER | LVGF_GROUPID;

	for(int i = 0; i < nNumEncoderOptionsGroups; i++)
	{
		lg.pszHeader = pszGroups[i];
		lg.iGroupId = 101 + i;
		ListView_InsertGroup(listView, -1, &lg);
	}

    // setup default values for raw audio input
    for(int i = 0; i < nNumRawSampleFormats; i++)
        this->m_CmbRawSampleFormat.InsertString(i, szRawSampleFormats[i]);

    // set default sample rate number
    this->m_SpnRawSampleRate.SetRange32(0, INT_MAX);
    this->m_SpnRawSampleRate.SetPos(0);

    // set default channels number
    this->m_SpnRawChannels.SetRange32(0, INT_MAX);
    this->m_SpnRawChannels.SetPos(0);

    // set default state for save configuration on exit option
    this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, 
        this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);

    // set output file/path default value
    if(this->bMultipleMonoInput == true)
        this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
    else
        this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);

    // init encoder options
    InitEncoderOptions();

    // configure Default preset
    defaultPreset.szName = _T("Default");

    // set Aften engine defaults values for first start (when there is no config file present)
    // there is Aften api function to get this values: AftenContext aftenCtx; aften_set_defaults(&aftenCtx);
    defaultPreset.nMode = AFTEN_ENC_MODE_CBR; // aftenCtx.params.encoding_mode;
    defaultPreset.nBitrate = 0; // aftenCtx.params.bitrate;
    defaultPreset.nQuality = 240; // aftenCtx.params.quality;

	// add items  to settings listctrl
	int nGroupCounter = -1;
	LVITEM li = {0};
	li.mask = LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS;

	// fill advanced encoder options list
	for(int i = 0; i < nNumEncoderOptions; i++)
	{
		if(encOpt[i].bBeginGroup == true)
			nGroupCounter++;

		if(nGroupCounter >= 0 && nGroupCounter < nNumEncoderOptionsGroups)
		{
			li.pszText = encOpt[i].szName.GetBuffer();
			li.iItem = i;
			li.iSubItem = 0;
			li.iGroupId = 101 + nGroupCounter;
			ListView_InsertItem(listView, &li);
			ListView_SetItemText(listView, 0, 1, 
				encOpt[i].listOptNames.GetAt(encOpt[i].listOptNames.FindIndex(encOpt[i].nDefaultValue)).GetBuffer());

			encOpt[i].szName.ReleaseBuffer();
			encOpt[i].listOptNames.GetAt(encOpt[i].listOptNames.FindIndex(encOpt[i].nDefaultValue)).ReleaseBuffer();
		}

		// set current settings to defaults
		defaultPreset.nSetting[i] = encOpt[i].nDefaultValue;
	}

    this->m_LstSettings.bUseTooltipsList = true;

    defaultPreset.nRawChannels = 0;
    defaultPreset.nRawSampleFormat = 0;
    defaultPreset.nRawSampleRate = 0;
    defaultPreset.nCurrentEngine = 0;
    defaultPreset.nThreads = 0;
    defaultPreset.nUsedSIMD[0] = 1;
    defaultPreset.nUsedSIMD[1] = 1;
    defaultPreset.nUsedSIMD[2] = 1;
    defaultPreset.nUsedSIMD[3] = 1;

    // add default preset to presets list
    encPresets.AddTail(defaultPreset);
    ::nCurrentPreset = 0;
    this->m_CmbPresets.InsertString(0, defaultPreset.szName);
    this->m_CmbPresets.SetCurSel(::nCurrentPreset);

    // select first item in options list
    this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

    // set raw audio input defaults
    this->m_CmbRawSampleFormat.SetCurSel(defaultPreset.nRawSampleFormat);

    if(defaultPreset.nRawSampleRate == 0)
    {
        this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), defaultPreset.nRawSampleRate);
        this->m_EdtRawSamplerate.SetWindowText(szBuff);
    }

    if(defaultPreset.nRawChannels == 0)
    {
        this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), defaultPreset.nRawChannels);
        this->m_EdtRawChannels.SetWindowText(szBuff);
    }

    // setup check state for SIMD instructions
    this->m_ChkSimdMMX.SetCheck(defaultPreset.nUsedSIMD[0] == 1 ? BST_CHECKED : BST_UNCHECKED);
    this->m_ChkSimdSSE.SetCheck(defaultPreset.nUsedSIMD[1] == 1 ? BST_CHECKED : BST_UNCHECKED);
    this->m_ChkSimdSSE2.SetCheck(defaultPreset.nUsedSIMD[2] == 1 ? BST_CHECKED : BST_UNCHECKED);
    this->m_ChkSimdSSE3.SetCheck(defaultPreset.nUsedSIMD[3] == 1 ? BST_CHECKED : BST_UNCHECKED);

    // set number of default threads
    if(defaultPreset.nThreads == 0)
    {
        this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), defaultPreset.nThreads);
        this->m_EdtThreads.SetWindowText(szBuff);
    }

    // set bitrate or quality value and CBR/VBR mode switch
    if(defaultPreset.nMode == AFTEN_ENC_MODE_CBR)
    {
        this->m_SldBitrate.SetTic(1);
        this->m_SldBitrate.SetRange(0, 19);
        this->m_SldBitrate.SetPos(FindValidBitratePos(defaultPreset.nBitrate));

        this->m_ChkVbr.SetCheck(BST_UNCHECKED);
    }
    else if(defaultPreset.nMode == AFTEN_ENC_MODE_VBR)
    {
        this->m_SldBitrate.SetTic(1);
        this->m_SldBitrate.SetRange(0, 1023);
        this->m_SldBitrate.SetPos(defaultPreset.nQuality);

        this->m_ChkVbr.SetCheck(BST_CHECKED);
    }

    // update bitrate text field
    this->UpdateBitrateText();

    // set tooltips
    CString szTmpText;

    // Bitrate/Quality Slider
    /* [-b #]         CBR bitrate in kbps */
    /* [-q #]         VBR quality */
    szTmpText = 
        _T("CBR bitrate in kbps:\n\n")
        _T("CBR mode is selected by default. This option allows for\n")
        _T("setting the fixed bitrate. The default bitrate depends\n")
        _T("on the number of channels (not including LFE).\n")
        _T("1 = 96 kbps\n")
        _T("2 = 192 kbps\n")
        _T("3 = 256 kbps\n")
        _T("4 = 384 kbps\n")
        _T("5 = 448 kbps\n")
        _T("\nVBR quality:\n\n")
        _T("A value 0 to 1023 which corresponds to SNR offset, where\n")
        _T("q=240 equates to an SNR offset of 0. 240 is the default\n")
        _T("value. This scale will most likely be replaced in the\n")
        _T("future with a better quality measurement.");

    this->m_SldBitrate.SetTooltipText(szTmpText);

    // CBR/VBR CheckBox
    szTmpText = 
        _T("Enable VBR mode. If unchecked the CBR mode is used instead.");

    this->m_ChkVbr.SetTooltipText(szTmpText);

    // Presets ComboBox
    szTmpText = 
        _T("Set currently used encoder preset. You can load/save presets from/to file\n")
        _T("from File menu. All presets are automatically loaded/saved from/to text file.");

    this->m_CmbPresets.SetTooltipText(szTmpText);

    // SIMD instructions CheckBoxes
    /* [-nosimd X]    Comma-separated list of SIMD instruction sets not to use */
    szTmpText = 
        _T("Aften will auto-detect available SIMD instruction sets\n")
        _T("for your CPU, so you shouldn't need to disable sets\n")
        _T("explicitly - unless for speed or debugging reasons.");

    this->m_ChkSimdMMX.SetTooltipText(_T("This option enables MMX optimizations (if supported by CPU).\n\n") + szTmpText);
    this->m_ChkSimdSSE.SetTooltipText(_T("This option enables SSE optimizations (if supported by CPU).\n\n") + szTmpText);
    this->m_ChkSimdSSE2.SetTooltipText(_T("This option enables SSE2 optimizations (if supported by CPU).\n\n") + szTmpText);
    this->m_ChkSimdSSE3.SetTooltipText(_T("This option enables SSE3 optimizations (if supported by CPU).\n\n") + szTmpText);

    // Sample format ComboBox
    /* [-raw_fmt X]   Raw audio input sample format (default: s16_le) */
    szTmpText = 
        _T("Raw audio input sample format specifies the sample format\n")
        _T("when using raw audio input. Using this option forces Aften to\n")
        _T("treat the input as raw audio. The choices for the\n")
        _T("pre-defined sample formats are (default: s16_le):\n")
        _T("u8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,\n")
        _T("s32_le, s32_be, float_le, float_be, double_le, double_be");

    this->m_CmbRawSampleFormat.SetTooltipText(szTmpText);

    // Sample rate EditBox
    /* [-raw_sr #]    Raw audio input sample rate (default: 48000) */
    szTmpText = 
        _T("Raw audio input sample rate option forces Aften to\n")
        _T("treat the input as raw audio (default: 48000).");

    this->m_EdtRawSamplerate.SetTooltipText(szTmpText);

    // Channels EditBox
    /* [-raw_ch #]    Raw audio input channels (default: 2) */
    szTmpText = 
        _T("Raw audio input channels forces Aften to treat the input as\n")
        _T("raw audio (default: 2).");

    this->m_EdtRawChannels.SetTooltipText(szTmpText);

    // Threads EditBox
    /* [-threads #]   Number of threads */
    szTmpText = 
        _T("Aften can use multiple threads to speed up encoding.\n")
        _T("By default, Aften uses one thread for each logical CPU\n")
        _T("your system has, but you can override this value. A\n")
        _T("value of 0 is the default and indicates that Aften\n")
        _T("should try to detect the number of CPUs.");

    this->m_EdtThreads.SetTooltipText(szTmpText);

    // Engine ComboBox
    szTmpText = 
        _T("Set currently used Aften library. By selecting optimized Aften\n")
        _T("library you can speedup the encoding process.");

    this->m_CmbEngines.SetTooltipText(szTmpText);

    // Output path EditBox
    szTmpText = 
        _T("Set default output path for encoded files. By default files\n")
        _T("are encoded to the same directory as input files. When using\n")
        _T("multiple mono input than here is set the output file path.");

    this->m_EdtOutPath.SetTooltipText(szTmpText);

    // Multiple mono input CheckBox
    szTmpText = 
        _T("Enable multiple mono input mode. By adding multiple mono input\n")
        _T("files to the files list (minimum 2, maximum 6) in correct channel\n")
        _T("order the mono input files will be encoded into single ac3 file.");

    this->m_ChkMultipleMonoInput.SetTooltipText(szTmpText);

    // engines editor
    szTmpText = _T("Edit currently available Aften engines.");
    this->m_BtnEngines.SetTooltipText(szTmpText);

    // advanced view
    szTmpText = _T("Show or hide advanced configuration options.");
    this->m_ChkAdvancedView.SetTooltipText(szTmpText);

    // enable files/dirs drag & drop for dialog
    this->DragAcceptFiles(TRUE);

    // add resize anchors for main dialog
    this->InitDialogAnchors();

    // get controls CRect for later use in UpdateView function
    CWnd *phSettings = this->GetDlgItem(IDC_STATIC_SETTINGS);
    CWnd *phPresets = this->GetDlgItem(IDC_STATIC_PRESETS);

    this->m_BtnResetCurrent.GetWindowRect(rcInit_ResetCurrent);
    phSettings->GetWindowRect(rcInit_Settings);
    phPresets->GetWindowRect(rcInit_Presets);
    this->m_LstFiles.GetWindowRect(rcInit_LstFiles);

    // set default log file path
    this->szLogFile = GetExeFilePath() + DEFAULT_LOG_FILE_NAME;

    // init log file
    if(this->cmdLineOpt.bEnableLog == true)
    {
        // open/create log file
        ::LogFile(&logCtx, this->cmdLineOpt.szLogFile);
        ::LogOpen(&logCtx);
    }

    // load all program configuration and settings
    this->LoadAllConfiguration();

    // overwrites logging setting from config file
    if(this->cmdLineOpt.bEnableLog == true)
    {
        if(this->cmdLineOpt.bHaveLogFile == true)
            this->szLogFile = this->cmdLineOpt.szLogFile;
    }

    // set log menu item check state
    this->GetMenu()->CheckMenuItem(ID_OPTIONS_ENABLELOGGING, 
        (this->bEnableLog == true) ? MF_CHECKED : MF_UNCHECKED);

    // process read-only setting
    if(this->cmdLineOpt.bHaveSaveConfig == true)
    {
        this->bSaveConfig = this->cmdLineOpt.bSaveConfig;
        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, 
            this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);
    }

    // set fixed height of combobox controls
    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING);
    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS);
    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_RAW_SAMPLE_FORMAT);
    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_ENGINES);

    // subclass edit control in presets combobox dropdown
    COMBOBOXINFO cbi;
    ZeroMemory(&cbi, sizeof(COMBOBOXINFO));
    cbi.cbSize = sizeof(COMBOBOXINFO);
    this->m_CmbPresets.GetComboBoxInfo(&cbi);
    this->m_EdtCmbPresetName.SubclassWindow(cbi.hwndItem);

    // set output file path
    if((this->cmdLineOpt.bHaveOutputFile == true) && (this->cmdLineOpt.bHaveOutputPath == false))
    {
        this->szOutputFile = this->cmdLineOpt.szOutputFile;
        this->m_EdtOutPath.SetWindowText(this->szOutputFile);
    }

    // set default output path
    if((this->cmdLineOpt.bHaveOutputFile == false) && (this->cmdLineOpt.bHaveOutputPath == true))
    {
        this->szOutputPath = this->cmdLineOpt.szOutputPath;
        this->m_EdtOutPath.SetWindowText(this->szOutputPath);
    }

    // set default view mode (if defferent then Advanced)
    if(this->nViewMode != VIEW_MODE_ADVANCED)
        this->UpdateView(this->nViewMode);
    else
        this->m_ChkAdvancedView.SetCheck(BST_CHECKED);

    // encode input files and close program
    if(this->cmdLineOpt.bEncodeAndExit == true)
    {
        // encode input files
        this->OnBnClickedButtonEncode();
        
        // close main dialog window and exit
        this->OnFileExit();
    }
    else
    {
        this->bVisible = true;
        this->ShowWindow(SW_SHOW);
    }

    return TRUE;
}

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
    {
        lpwndpos->flags &= ~SWP_SHOWWINDOW;
    }

    CResizeDialog::OnWindowPosChanging(lpwndpos);
}

bool CEncWAVtoAC3Dlg::LoadProgramConfig(CString szFileName)
{
    // init program configuration
    if(::LoadConfig(szFileName, this->m_ConfigList) == true)
    {
        int nSize = this->m_ConfigList.GetSize();
        POSITION pos = this->m_ConfigList.GetHeadPosition();
        for(INT_PTR i = 0; i < nSize; i++)
        {
            ConfigEntry ce;

            // get next entry in configuration list
            ce = this->m_ConfigList.GetNext(pos);

            // key: MainWindow
            if(ce.szKey.Compare(_T("MainWindow")) == 0)
            {
                 this->SetWindowRectStr(ce.szData); 
            }

            // key: ColumnSizeSettings
            else if(ce.szKey.Compare(_T("ColumnSizeSettings")) == 0)
            {
                int nColumn[2] = { 0, 0 };
                if(_stscanf(ce.szData, _T("%d %d"), 
                    &nColumn[0], &nColumn[1]) == 2)
                {
                    this->m_LstSettings.SetColumnWidth(0, nColumn[0]);
                    this->m_LstSettings.SetColumnWidth(1, nColumn[1]);
                }
            }

            // key: ColumnSizeFiles
            else if(ce.szKey.Compare(_T("ColumnSizeFiles")) == 0)
            {
                int nColumn[2] = { 0, 0 };
                if(_stscanf(ce.szData, _T("%d %d"), 
                    &nColumn[0], &nColumn[1]) == 2)
                {
                    this->m_LstFiles.SetColumnWidth(0, nColumn[0]);
                    this->m_LstFiles.SetColumnWidth(1, nColumn[1]);
                }
            }

            // key: OutputPath
            else if(ce.szKey.Compare(_T("OutputPath")) == 0)
            {
                if(ce.szData.Compare(_T("")) != 0 && ce.szData.Compare(DEFAULT_TEXT_OUTPUT_PATH) != 0)
                {
                    this->szOutputPath = ce.szData;
                }
            }

            // key: OutputFile
            else if(ce.szKey.Compare(_T("OutputFile")) == 0)
            {
                if(ce.szData.Compare(_T("")) != 0 && ce.szData.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0)
                {
                    this->szOutputFile = ce.szData;
                }
            }

            // key: SelectedPreset
            else if(ce.szKey.Compare(_T("SelectedPreset")) == 0)
            {
                int nPreset = 0;
                if(_stscanf(ce.szData, _T("%d"), 
                    &nPreset) == 1)
                {
                    // reset preset selection if the current preset value is invalid
                    if((nPreset >= this->m_CmbPresets.GetCount()) || (nPreset < 0))
                        nPreset = 0;

                    if(::nCurrentPreset != nPreset)
                    {
                        ::nCurrentPreset = nPreset;
                        this->m_CmbPresets.SetCurSel(nPreset);
                        this->OnCbnSelchangeComboPresets();
                    }
                }
            }

            // key: MultipleMonoInput
            else if(ce.szKey.Compare(_T("MultipleMonoInput")) == 0)
            {
                if(ce.szData.Compare(_T("true")) == 0)
                {
                    this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
                    this->bMultipleMonoInput = true;
                    this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(_T("Output file:"));
                }
                else if(ce.szData.Compare(_T("false")) == 0)
                {
                    this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
                    this->bMultipleMonoInput = false;
                    this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(_T("Output path:"));
                }
                else
                {
                    this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
                    this->bMultipleMonoInput = false;
                    this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(_T("Output path:"));
                }
            }

            // key: LogFile
            else if(ce.szKey.Compare(_T("LogFile")) == 0)
            {
                if(ce.szData.Compare(_T("")) != 0)
                {
                    this->szLogFile = ce.szData;
                }
            }

            // key: EnableLogging
            else if(ce.szKey.Compare(_T("EnableLogging")) == 0)
            {
                if(ce.szData.Compare(_T("true")) == 0)
                {
                    this->bEnableLog = true;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_ENABLELOGGING, MF_CHECKED);
                }
                else if(ce.szData.Compare(_T("false")) == 0)
                {
                    this->bEnableLog = false;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_ENABLELOGGING, MF_UNCHECKED);
                }
                else
                {
                    this->bEnableLog = false;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_ENABLELOGGING, MF_UNCHECKED);
                }
            }

            // key: DisableAllWarnings
            else if(ce.szKey.Compare(_T("DisableAllWarnings")) == 0)
            {
                if(ce.szData.Compare(_T("true")) == 0)
                {
                    this->bDisableAllWarnings = true;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_CHECKED);
                }
                else if(ce.szData.Compare(_T("false")) == 0)
                {
                    this->bDisableAllWarnings = false;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_UNCHECKED);
                }
                else
                {
                    this->bDisableAllWarnings = false;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_UNCHECKED);
                }
            }

            // key: SaveConfig
            else if(ce.szKey.Compare(_T("SaveConfig")) == 0)
            {
                if(ce.szData.Compare(_T("true")) == 0)
                {
                    this->bSaveConfig = true;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_CHECKED);
                }
                else if(ce.szData.Compare(_T("false")) == 0)
                {
                    this->bSaveConfig = false;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_UNCHECKED);
                }
                else
                {
                    this->bSaveConfig = true;
                    this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_CHECKED);
                }
            }

            // key: ViewMode
            else if(ce.szKey.Compare(_T("ViewMode")) == 0)
            {
                int nMode = VIEW_MODE_STANDARD;
                if(_stscanf(ce.szData, _T("%d"), 
                    &nMode) == 1)
                {
                    // reset mode if the value is invalid
                    if((nMode > VIEW_MODE_ADVANCED) || (nMode < VIEW_MODE_STANDARD))
                        nMode = VIEW_MODE_STANDARD;

                    // update view mode if different then currently used
                    if(nMode != this->nViewMode)
                        this->UpdateView(nMode); 

                    if(nMode == VIEW_MODE_ADVANCED)
                        this->m_ChkAdvancedView.SetCheck(BST_CHECKED);
                    else
                        this->m_ChkAdvancedView.SetCheck(BST_UNCHECKED);
                }
            }
        }

        // open/create log file
        if((this->bEnableLog == true) && (logCtx.bInit == false))
        {
            ::LogFile(&logCtx, this->szLogFile);
            ::LogOpen(&logCtx);
        }

        // set output file/path
        if(this->bMultipleMonoInput == true)
        {
            if(this->szOutputFile.Compare(_T("")) == 0)
                this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
            else
                this->m_EdtOutPath.SetWindowText(this->szOutputFile);
        }
        else
        {
            if(this->szOutputPath.Compare(_T("")) == 0)
                this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
            else
                this->m_EdtOutPath.SetWindowText(this->szOutputPath);
        }

        return true;
    }

    return false;
}

bool CEncWAVtoAC3Dlg::SaveProgramConfig(CString szFileName)
{
    // prepare program configuration
    this->m_ConfigList.RemoveAll();

    ConfigEntry ce;

    // key: MainWindow
    ce.szKey = _T("MainWindow");
    ce.szData = this->GetWindowRectStr();
    this->m_ConfigList.AddTail(ce);

    // key: ColumnSizeSettings
    ce.szKey = _T("ColumnSizeSettings");
    ce.szData.Format(_T("%d %d"), 
        this->m_LstSettings.GetColumnWidth(0),
        this->m_LstSettings.GetColumnWidth(1));
    this->m_ConfigList.AddTail(ce);

    // key: ColumnSizeFiles
    ce.szKey = _T("ColumnSizeFiles");
    ce.szData.Format(_T("%d %d"), 
        this->m_LstFiles.GetColumnWidth(0),
        this->m_LstFiles.GetColumnWidth(1));
    this->m_ConfigList.AddTail(ce);

    // key: OutputPath
    ce.szKey = _T("OutputPath");
    ce.szData = this->szOutputPath;
    if(ce.szData.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0)
        ce.szData = _T("");
    this->m_ConfigList.AddTail(ce);

    // key: OutputFile
    ce.szKey = _T("OutputFile");
    ce.szData = this->szOutputFile;
    if(ce.szData.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
        ce.szData = _T("");
    this->m_ConfigList.AddTail(ce);

    // key: SelectedPreset
    ce.szKey = _T("SelectedPreset");
    ce.szData.Format(_T("%d"), 
        this->m_CmbPresets.GetCurSel());
    this->m_ConfigList.AddTail(ce);

    // key: MultipleMonoInput
    ce.szKey = _T("MultipleMonoInput");
    ce.szData = (this->bMultipleMonoInput == true) ? _T("true") : _T("false");
    this->m_ConfigList.AddTail(ce);

    // key: LogFile
    ce.szKey = _T("LogFile");
    ce.szData = this->szLogFile;
    this->m_ConfigList.AddTail(ce);

    // key: EnableLogging
    ce.szKey = _T("EnableLogging");
    ce.szData = (this->bEnableLog == true) ? _T("true") : _T("false");
    this->m_ConfigList.AddTail(ce);

    // key: DisableAllWarnings
    ce.szKey = _T("DisableAllWarnings");
    ce.szData = (this->bDisableAllWarnings == true) ? _T("true") : _T("false");
    this->m_ConfigList.AddTail(ce);

    // key: SaveConfig
    ce.szKey = _T("SaveConfig");
    ce.szData = (this->bSaveConfig == true) ? _T("true") : _T("false");
    this->m_ConfigList.AddTail(ce);

    // key: ViewMode
    ce.szKey = _T("ViewMode");
    ce.szData.Format(_T("%d"), 
        this->nViewMode);
    this->m_ConfigList.AddTail(ce);

    // save program configuration
    return ::SaveConfig(szFileName, this->m_ConfigList);
}

bool CEncWAVtoAC3Dlg::UpdateProgramEngines()
{
    int nSize = this->m_EngineList.GetSize();

    // if there is no engine then return error
    if(nSize == 0)
    {
        // add default engine to the list
        ConfigEntry ce;

        ce.szKey = _T("Aften");
        ce.szData = _T("libaften.dll");

        this->m_EngineList.RemoveAll();
        this->m_EngineList.AddTail(ce);       

        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nCurrentEngine = 0;
        UpdateCurrentPreset(tmpPreset);

        this->m_CmbEngines.InsertString(0, ce.szKey);
        this->m_CmbEngines.SetCurSel(0);

        this->api.szLibPath = m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szData;
        OpenAftenAPI(&this->api);

        return false;
    }

    POSITION pos = this->m_EngineList.GetHeadPosition();
    for(INT_PTR i = 0; i < nSize; i++)
    {
        ConfigEntry ce;

        // get next entry in configuration list
        ce = this->m_EngineList.GetNext(pos);

        // insert all items to engines combobox
        // ce.szKey  - name of engine   
        // ce.szData - path to libaften.dll
        this->m_CmbEngines.InsertString(i, ce.szKey);
    }

    // reset current engine if it's to big
    if(GetCurrentPreset().nCurrentEngine > nSize)
    {
        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nCurrentEngine = 0;
        UpdateCurrentPreset(tmpPreset);
    }

    // load and select current engine
    if((GetCurrentPreset().nCurrentEngine >= 0) && (GetCurrentPreset().nCurrentEngine < nSize))
    {
        // load new aften library
        this->api.szLibPath = m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szData;
        if(OpenAftenAPI(&this->api) == false)
        {
            // select 'None' aften engine
            this->m_CmbEngines.SetCurSel(0);

            EncoderPreset tmpPreset = GetCurrentPreset();
            tmpPreset.nCurrentEngine = 0;
            UpdateCurrentPreset(tmpPreset);
        }
        else
        {
            // select current aften engine
            this->m_CmbEngines.SetCurSel(GetCurrentPreset().nCurrentEngine);
        }
    }

    return true;
}

bool CEncWAVtoAC3Dlg::LoadProgramEngines(CString szFileName)
{
    // init engines configuration
    this->m_EngineList.RemoveAll();
    this->m_CmbEngines.ResetContent();

    if(::LoadConfig(szFileName, this->m_EngineList) == true)
    {
        return this->UpdateProgramEngines();
    }
    else
    {
        // add default engine to the list
        ConfigEntry ce;

        ce.szKey = _T("Aften");
        ce.szData = _T("libaften.dll");

        this->m_EngineList.RemoveAll();
        this->m_EngineList.AddTail(ce);     

        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nCurrentEngine = 0;
        UpdateCurrentPreset(tmpPreset);

        this->m_CmbEngines.InsertString(0, ce.szKey);
        this->m_CmbEngines.SetCurSel(0);

        this->api.szLibPath = m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szData;
        OpenAftenAPI(&this->api);
    }

    return false;
}

bool CEncWAVtoAC3Dlg::SaveProgramEngines(CString szFileName)
{
    // save engines configuration
    return ::SaveConfig(szFileName, this->m_EngineList);
}

bool CEncWAVtoAC3Dlg::LoadFilesList(CString &szFileName)
{
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

        // clear list
        this->m_LstFiles.DeleteAllItems();

        TCHAR Buffer;
        CString szBuffer = _T("");

        while(fp.FRead(Buffer) == true)
        {
            if((Buffer != '\r') && (Buffer != '\n'))
                szBuffer += Buffer;

            if(Buffer == '\n' || nRead == nLength - sizeof(TCHAR))
            {
                szBuffer += _T("\0");

                if(szBuffer.GetLength() > 0)
                {
                    // remove leading nad trailing quotes (used for *.mux file format)
                    szBuffer.TrimLeft('"');
                    szBuffer.TrimRight('"');

                    // // add only files with proper file extensions
                    if(IsSupportedInputExt(GetFileExt(szBuffer)) == true)
                    {
                        this->AddItemToFileList(szBuffer);
                    }
                }

                szBuffer = _T("");
            }

            nRead++;
        }

        fp.FClose();
        return true;
    }
    catch(...)
    {
        return false;
    }
}

bool CEncWAVtoAC3Dlg::SaveFilesList(CString &szFileName, int nFormat)
{
    int nItems = this->m_LstFiles.GetItemCount();
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
        CString szTmpFileName;

        for(int i = 0; i < nItems; i++)
        {
            szTmpFileName = this->m_LstFiles.GetItemText(i, 0);
            szBuffer.Format(_T("%s%s%s\r\n"), nFormat == 0 ? _T("") : _T("\""), szTmpFileName, nFormat == 0 ? _T("") : _T("\""));
            fp.FWriteString(szBuffer.GetBuffer(), szBuffer.GetLength());
            szBuffer.ReleaseBuffer();
        }

        fp.FClose();
    }
    catch(...)
    {
        return false;
    }

    return true;
}

void CEncWAVtoAC3Dlg::LoadAllConfiguration()
{
    bool bRet = false;

    // load presets from file
    if(this->cmdLineOpt.bHaveLoadPresets == true)
    {
        bRet = ::LoadEncoderPresets(this->cmdLineOpt.szLoadPresets);
        if(this->bEnableLog)
        {
            if(bRet == true)
                ::LogMessage(&logCtx, _T("Loaded encoder presets: ") + this->cmdLineOpt.szLoadPresets);
            else
                ::LogMessage(&logCtx, _T("Failed to load encoder presets: ") + this->cmdLineOpt.szLoadPresets);
        }
    }
    else
    {
        bRet = ::LoadEncoderPresets(GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME);
        if(this->bEnableLog)
        {
            if(bRet == true)
                ::LogMessage(&logCtx, _T("Loaded encoder presets: ") + GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME);
            else
                ::LogMessage(&logCtx, _T("Failed to load encoder presets: ") + GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME);
        }
    }

    // process presets list
    if(bRet == true)
    {
        // get first preset from loaded list
        if(encPresets.GetSize() > 0)
        {
            // reset presets combobox
            this->m_CmbPresets.ResetContent();

            // add all preset names to preset combobox
            for(int i = 0; i < encPresets.GetCount(); i++)
            {
                EncoderPreset encTmp = encPresets.GetAt(encPresets.FindIndex(i));

                this->m_CmbPresets.InsertString(i, encTmp.szName);
            }

            // select current preset
            if((::nCurrentPreset >= encPresets.GetCount()) || (::nCurrentPreset < 0))
                ::nCurrentPreset = 0;

            this->m_CmbPresets.SetCurSel(::nCurrentPreset);

            // update all controls
            this->OnCbnSelchangeComboPresets();
        }
    }

    // load program settings from file
    if(this->cmdLineOpt.bHaveLoadConfig == true)
    {
        bRet = this->LoadProgramConfig(this->cmdLineOpt.szLoadConfig);
        if(this->bEnableLog)
        {
            if(bRet == true)
                ::LogMessage(&logCtx, _T("Loaded program config: ") + this->cmdLineOpt.szLoadConfig);
            else
                ::LogMessage(&logCtx, _T("Failed to load program config: ") + this->cmdLineOpt.szLoadConfig);
        }
    }
    else
    {
        bRet = this->LoadProgramConfig(GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME);
        if(this->bEnableLog)
        {
            if(bRet == true)
                ::LogMessage(&logCtx, _T("Loaded program config: ") + GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME);
            else
                ::LogMessage(&logCtx, _T("Failed to load program config: ") + GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME);
        }
    }

    // load engines settings from file
    if(this->cmdLineOpt.bHaveLoadEngines == true)
    {
        bRet = this->LoadProgramEngines(this->cmdLineOpt.szLoadEngines);
        if(this->bEnableLog)
        {
            if(bRet == true)
                ::LogMessage(&logCtx, _T("Loaded encoder engines: ") + this->cmdLineOpt.szLoadEngines);
            else
                ::LogMessage(&logCtx, _T("Failed to load encoder engines: ") + this->cmdLineOpt.szLoadEngines);
        }
    }
    else
    {
        bRet = this->LoadProgramEngines(GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME);
        if(this->bEnableLog)
        {
            if(bRet == true)
                ::LogMessage(&logCtx, _T("Loaded encoder engines: ") + GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME);
            else
                ::LogMessage(&logCtx, _T("Failed to load encoder engines: ") + GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME);
        }
    }

    // add input file to the files list
    if(this->cmdLineOpt.bHaveInputFile == true)
    {
        this->OnListClearList();
        this->AddItemToFileList(this->cmdLineOpt.szInputFile);
    }
    else
    {
        // load files list from file
        if(this->cmdLineOpt.bHaveLoadFiles == true)
        {
            bRet = this->LoadFilesList(this->cmdLineOpt.szLoadFiles);
            if(this->bEnableLog)
            {
                if(bRet == true)
                    ::LogMessage(&logCtx, _T("Loaded files list: ") + this->cmdLineOpt.szLoadFiles);
                else
                    ::LogMessage(&logCtx, _T("Failed to load files list: ") + this->cmdLineOpt.szLoadFiles);
            }
        }
        else
        {
            bRet = this->LoadFilesList(GetExeFilePath() + DEFAULT_FILES_FILE_NAME);
            if(this->bEnableLog)
            {
                if(bRet == true)
                    ::LogMessage(&logCtx, _T("Loaded files list: ") + GetExeFilePath() + DEFAULT_FILES_FILE_NAME);
                else
                    ::LogMessage(&logCtx, _T("Failed to load files list: ") + GetExeFilePath() + DEFAULT_FILES_FILE_NAME);
            }
        }
    }
}

void CEncWAVtoAC3Dlg::SaveAllConfiguration()
{
    bool bRet = false;

    // save encoder presets to a file
    bRet = ::SaveEncoderPresets(GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME);
    if(this->bEnableLog)
    {
        if(bRet == true)
            ::LogMessage(&logCtx, _T("Saved encoder presets: ") + GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME);
        else
            ::LogMessage(&logCtx, _T("Failed to save encoder presets: ") + GetExeFilePath() + DEFAULT_PRESETS_FILE_NAME);
    }

    // save program configuration to a file
    bRet = this->SaveProgramConfig(GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME);
    if(this->bEnableLog)
    {
        if(bRet == true)
            ::LogMessage(&logCtx, _T("Saved program config: ") + GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME);
        else
            ::LogMessage(&logCtx, _T("Failed to save program config: ") + GetExeFilePath() + DEFAULT_CONFIG_FILE_NAME);
    }

    // save engines settings to a file
    bRet = this->SaveProgramEngines(GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME);
    if(this->bEnableLog)
    {
        if(bRet == true)
            ::LogMessage(&logCtx, _T("Saved encoder engines: ") + GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME);
        else
            ::LogMessage(&logCtx, _T("Failed to save encoder engines: ") + GetExeFilePath() + DEFAULT_ENGINES_FILE_NAME);
    }

    // save files list to file
    bRet = this->SaveFilesList(GetExeFilePath() + DEFAULT_FILES_FILE_NAME, DEFAULT_FILES_FORMAT);
    if(this->bEnableLog)
    {
        if(bRet == true)
            ::LogMessage(&logCtx, _T("Saved files list: ") + GetExeFilePath() + DEFAULT_FILES_FILE_NAME);
        else
            ::LogMessage(&logCtx, _T("Failed to save files list: ") + GetExeFilePath() + DEFAULT_FILES_FILE_NAME);
    }
}

void CEncWAVtoAC3Dlg::UpdateBitrateText()
{
    int nCurPos = this->m_SldBitrate.GetPos();

    CString szBuff;

    if(this->m_ChkVbr.GetCheck() == BST_CHECKED)
    {
        // indicate that we have selected Quality based mode (VBR)
        m_StcQualityBitrate.SetWindowText(_T("Quality:"));

        szBuff.Format(_T("%d"), nCurPos);

        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nQuality = nCurPos;
        UpdateCurrentPreset(tmpPreset);
    }
    else
    {
        if((nCurPos >= 0) && (nCurPos < nNumValidCbrBitrates))
        {
            // indicate that we have selected Bitrate based mode (CBR)
            m_StcQualityBitrate.SetWindowText(_T("Bitrate:"));

            if(nCurPos == 0)
                szBuff.Format(DEFAULT_TEXT_AUTO);
            else
                szBuff.Format(_T("%d kbps"), nValidCbrBitrates[nCurPos]);

            EncoderPreset tmpPreset = GetCurrentPreset();
            tmpPreset.nBitrate = nValidCbrBitrates[nCurPos];
            UpdateCurrentPreset(tmpPreset);
        }
    }

    this->m_StcBitrate.SetWindowText(szBuff);
}

void CEncWAVtoAC3Dlg::AddItemToFileList(CString szPath)
{
    // add file to list
    CString szSize = _T("");
    ULARGE_INTEGER ulSize;
    ULONGLONG nFileSize;
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    // check if file exist
    hFind = ::FindFirstFile(szPath, &FindFileData);
    if(hFind == INVALID_HANDLE_VALUE) 
        return;

    ulSize.HighPart = FindFileData.nFileSizeHigh;
    ulSize.LowPart = FindFileData.nFileSizeLow;
    nFileSize = ulSize.QuadPart;

    ::FindClose(hFind);

    // get file index and icon
    int nItem = this->m_LstFiles.GetItemCount();
    SHFILEINFO sfi;
    LV_ITEM lvi;

    SHGetFileInfo((LPCTSTR) szPath, 
        0, 
        &sfi, 
        sizeof(sfi), 
        SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY);

    lvi.mask = LVIF_IMAGE | LVIF_PARAM;
    lvi.iItem = nItem; 
    lvi.iImage = sfi.iIcon;
    lvi.iSubItem = 0; 
    lvi.lParam = nItem;

    this->m_LstFiles.InsertItem(&lvi);

    // update item file name
    this->m_LstFiles.SetItemText(nItem, 0, szPath);

#ifndef DISABLE_AVISYNTH
    // get AVS file size
    if(GetFileExt(szPath).MakeLower() == _T("avs"))
    {
        // get input Audio stream information from Avisynth
        AvsAudioInfo infoAVS;
        memset(&infoAVS, 0, sizeof(AvsAudioInfo));
        if(GetAvisynthFileInfo(szPath, &infoAVS) == false)
            return; // failed to load avisynth script

        nFileSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
    }
#endif

    // szSize.Format(_T("%I64d"), GetFileSize64(szPath));
    szSize.Format(_T("%I64d"), nFileSize);

    // update item file size
    this->m_LstFiles.SetItemText(nItem, 1, szSize);
}

void CEncWAVtoAC3Dlg::ApplyPresetToDlg(EncoderPreset &Preset)
{
    // fill advanced encoder options list
    for(int i = 0; i < nNumEncoderOptions; i++)
    {
        // set all values
        this->m_LstSettings.SetItemText(i, 
            1, 
            encOpt[i].listOptNames.GetAt(encOpt[i].listOptNames.FindIndex(Preset.nSetting[i])));
    }

    // set bitrate or quality value
    if(Preset.nMode == AFTEN_ENC_MODE_CBR)
    {
        this->m_SldBitrate.SetTic(1);
        this->m_SldBitrate.SetRange(0, 19);
        this->m_SldBitrate.SetPos(FindValidBitratePos(Preset.nBitrate));
        this->m_ChkVbr.SetCheck(BST_UNCHECKED);
    }
    else if(Preset.nMode == AFTEN_ENC_MODE_VBR)
    {
        this->m_SldBitrate.SetTic(1);
        this->m_SldBitrate.SetRange(0, 1023);
        this->m_SldBitrate.SetPos(Preset.nQuality);
        this->m_ChkVbr.SetCheck(BST_CHECKED);
    }

    // update used SIMD instructions
    this->m_ChkSimdMMX.SetCheck(Preset.nUsedSIMD[0] == 0 ? BST_UNCHECKED : BST_CHECKED);
    this->m_ChkSimdSSE.SetCheck(Preset.nUsedSIMD[1] == 0 ? BST_UNCHECKED : BST_CHECKED);
    this->m_ChkSimdSSE2.SetCheck(Preset.nUsedSIMD[2] == 0 ? BST_UNCHECKED : BST_CHECKED);
    this->m_ChkSimdSSE3.SetCheck(Preset.nUsedSIMD[3] == 0 ? BST_UNCHECKED : BST_CHECKED);

    // update numer of threads
    if(Preset.nThreads == 0)
    {
        this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), Preset.nThreads);
        this->m_EdtThreads.SetWindowText(szBuff);
    }

    // update Aften engine
    this->m_CmbEngines.SetCurSel(Preset.nCurrentEngine);

    // update raw audio input sample format
    this->m_CmbRawSampleFormat.SetCurSel(Preset.nRawSampleFormat);

    // update raw audio input sample rate
    if(Preset.nRawSampleRate == 0)
    {
        this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), Preset.nRawSampleRate);
        this->m_EdtRawSamplerate.SetWindowText(szBuff);
    }

    // update raw audio input channels
    if(Preset.nRawChannels == 0)
    {
        this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), Preset.nRawChannels);
        this->m_EdtRawChannels.SetWindowText(szBuff);
    }

    // update bitrate text field
    this->UpdateBitrateText();

    // update settings combobox
    POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int nItem = m_LstSettings.GetNextSelectedItem(pos);

        this->m_LstSettings.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
        this->UpdateSettingsComboBox(nItem);
        this->m_CmbValue.SetCurSel(Preset.nSetting[nItem]);
    }
    else
    {
        this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
        this->UpdateSettingsComboBox(0);
        this->m_CmbValue.SetCurSel(Preset.nSetting[0]);
    }
}

void CEncWAVtoAC3Dlg::HandleDropFiles(HDROP hDropInfo)
{
    int nCount = ::DragQueryFile(hDropInfo, (UINT) 0xFFFFFFFF, NULL, 0);
    if(nCount > 0)
    {
        for(int i = 0; i < nCount; i++)
        {
            int nReqChars = ::DragQueryFile(hDropInfo, i, NULL, 0);

            CString szFile;
            ::DragQueryFile(hDropInfo, 
                i, 
                szFile.GetBuffer(nReqChars * 2 + 8), 
                nReqChars * 2 + 8);
            if(::GetFileAttributes(szFile) & FILE_ATTRIBUTE_DIRECTORY)
            {
                // insert droped files in directory and subdirs
                this->SearchFolderForFiles(szFile, true);
            }
            else
            {
                // apply filter and add only .wav files
                CString szExt = ::PathFindExtension(szFile);
                szExt.MakeLower();
                szExt.Remove('.');

                // add only files with proper file extensions
                if(IsSupportedInputExt(szExt) == true)
                {
                    // insert droped file
                    this->AddItemToFileList(szFile);
                }
            }

            szFile.ReleaseBuffer();
        }
    }

    ::DragFinish(hDropInfo);
}

void CEncWAVtoAC3Dlg::UpdateSettingsComboBox(int nItem)
{
    // remove all items from value combobox
    this->m_CmbValue.ResetContent();

    // add new items to combobox
    POSITION posNames = encOpt[nItem].listOptNames.GetHeadPosition();
    while(posNames != NULL)
    {
        this->m_CmbValue.AddString(encOpt[nItem].listOptNames.GetNext(posNames));
    }

    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING);

    // select default value or last selected
    if(::encPresets.GetSize() <= 0)
        this->m_CmbValue.SetCurSel(encOpt[nItem].nDefaultValue);
    else
        this->m_CmbValue.SetCurSel(GetCurrentPreset().nSetting[nItem]);
}

void CEncWAVtoAC3Dlg::SearchFolderForFiles(CString szFile, const bool bRecurse)
{
    try
    {
        WIN32_FIND_DATA w32FileData;  
        HANDLE hSearch = NULL; 
        BOOL fFinished = FALSE;
        TCHAR cTempBuf[(MAX_PATH * 2) + 1];

        ZeroMemory(&w32FileData, sizeof(WIN32_FIND_DATA));
        ZeroMemory(cTempBuf, MAX_PATH * 2);

        // remove '\' or '/' from end of search path
        szFile.TrimRight(_T("\\"));
        szFile.TrimRight(_T("/"));

        wsprintf(cTempBuf, _T("%s\\*.*\0"), szFile);

        hSearch = FindFirstFile(cTempBuf, &w32FileData); 
        if(hSearch == INVALID_HANDLE_VALUE) 
            return;

        while(fFinished == FALSE) 
        { 
            if(w32FileData.cFileName[0] != '.' &&
                !(w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            {  
                CString szTempBuf;
                szTempBuf.Format(_T("%s\\%s\0"), szFile, w32FileData.cFileName);

                // apply filter and add only .wav files
                CString szExt = ::PathFindExtension(szTempBuf);
                szExt.MakeLower();
                szExt.Remove('.');

                // add only files with proper file extensions
                if(IsSupportedInputExt(szExt) == true)
                {
                    this->AddItemToFileList(szTempBuf);
                }
            }

            if(w32FileData.cFileName[0] != '.' &&
                w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                wsprintf(cTempBuf, _T("%s\\%s\0"), szFile, w32FileData.cFileName);

                // recurse subdirs
                if(bRecurse == true)
                {
                    this->SearchFolderForFiles(cTempBuf, true);
                }
            }

            if(FindNextFile(hSearch, &w32FileData) == FALSE) 
            {
                if(GetLastError() == ERROR_NO_MORE_FILES) 
                    fFinished = TRUE; 
                else 
                    return;
            }
        }

        if(FindClose(hSearch) == FALSE) 
            return;
    }
    catch(...)
    {
        MessageBox(_T("Error while searching for files!"), _T("Error"), MB_OK | MB_ICONERROR);
    }
}

void CEncWAVtoAC3Dlg::ShowOptionPopup(bool bUseRect)
{
    // get starting point for context menu
    POINT point;
    GetCursorPos(&point);

    int nItem;

    // show right click context menu for selected item
    POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        nItem = m_LstSettings.GetNextSelectedItem(pos);
    }
    else
    {
        return;
    }

    if(bUseRect == true)
    {
        CRect rc;

        m_LstSettings.GetItemRect(nItem, rc, LVIR_BOUNDS);
        m_LstSettings.ClientToScreen(rc);

        // show only if user clicked in value column
        if(rc.PtInRect(point) == FALSE)
            return;
    }

    // create new popup menu
    CMenu *menu = new CMenu;
    menu->CreatePopupMenu();

    UINT nItemCount = ID_OPTIONS_MENU_START;
    POSITION posNames = encOpt[nItem].listOptNames.GetHeadPosition();
    while(posNames != NULL)
    {
        menu->AppendMenu(MF_STRING, nItemCount, encOpt[nItem].listOptNames.GetNext(posNames)); 
        nItemCount++;
    }

    // check currently selected option
    int nCurSel = this->m_CmbValue.GetCurSel();
    menu->CheckMenuItem(ID_OPTIONS_MENU_START + nCurSel, MF_CHECKED);

    // show popup menu to user
    ::SetForegroundWindow(this->GetSafeHwnd());
    int nRet = (int) menu->TrackPopupMenu(TPM_RETURNCMD, point.x, point.y, this, NULL);
    if(nRet >= ID_OPTIONS_MENU_START)
    {
        // update setting in value combobox and in options list
        this->m_CmbValue.SetCurSel(nRet - ID_OPTIONS_MENU_START);
        this->OnCbnSelchangeComboSetting();
    }

    // free used memory
    if(menu)
        delete menu;
}

void CEncWAVtoAC3Dlg::UpdateView(int nMode)
{
    // 0 - Standard, 1 - Advanced
    int nCmdShow = nMode == VIEW_MODE_STANDARD ? SW_HIDE : SW_SHOW;

    // update view mode global setting
    this->nViewMode = nMode;

    // show or hide window controls
    this->m_LstSettings.ShowWindow(nCmdShow);
    this->m_StcSelected.ShowWindow(nCmdShow);
    this->m_CmbValue.ShowWindow(nCmdShow);
    this->m_CmbRawSampleFormat.ShowWindow(nCmdShow);
    this->m_CmbEngines.ShowWindow(nCmdShow);
    this->m_BtnEngines.ShowWindow(nCmdShow);
    this->m_EdtThreads.ShowWindow(nCmdShow);
    this->m_EdtRawSamplerate.ShowWindow(nCmdShow);
    this->m_EdtRawChannels.ShowWindow(nCmdShow);
    this->m_SpnThreads.ShowWindow(nCmdShow);
    this->m_SpnRawSampleRate.ShowWindow(nCmdShow);
    this->m_SpnRawChannels.ShowWindow(nCmdShow);
    this->m_ChkSimdMMX.ShowWindow(nCmdShow);
    this->m_ChkSimdSSE.ShowWindow(nCmdShow);
    this->m_ChkSimdSSE2.ShowWindow(nCmdShow);
    this->m_ChkSimdSSE3.ShowWindow(nCmdShow);
    this->m_BtnResetCurrent.ShowWindow(nCmdShow);
    this->m_BtnRemove.ShowWindow(nCmdShow);
    this->m_BtnAddNew.ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_RAW)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_SAMPLE_FORMAT)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_SAMPLE_RATE)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_CHANNELS)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_SIMD)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_PARALLEL)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_ENGINE)->ShowWindow(nCmdShow);
    this->GetDlgItem(IDC_STATIC_THREADS)->ShowWindow(nCmdShow);
    // this->m_LstFiles.ShowWindow(nCmdShow);
    // this->m_SldBitrate.ShowWindow(nCmdShow);
    // this->m_StcBitrate.ShowWindow(nCmdShow);
    // this->m_StcQualityBitrate.ShowWindow(nCmdShow);
    // this->m_CmbPresets.ShowWindow(nCmdShow);
    // this->m_EdtOutPath.ShowWindow(nCmdShow);
    // this->m_ChkVbr.ShowWindow(nCmdShow);
    // this->m_ChkMultipleMonoInput.ShowWindow(nCmdShow);
    // this->m_BtnEncode.ShowWindow(nCmdShow);
    // this->m_BtnAddFiles.ShowWindow(nCmdShow);
    // this->m_BtnBrowse.ShowWindow(nCmdShow);
    // this->m_BtnMuxWizard.ShowWindow(nCmdShow);

    // move/resize controls
    CWnd *phSettings = this->GetDlgItem(IDC_STATIC_SETTINGS);
    CWnd *phPresets = this->GetDlgItem(IDC_STATIC_PRESETS);

    CRect rcResetCurrent;
    CRect rcSettings;
    CRect rcPresets;
    CRect rcLstFiles;

    this->m_BtnResetCurrent.GetWindowRect(rcResetCurrent);
    phSettings->GetWindowRect(rcSettings);
    phPresets->GetWindowRect(rcPresets);
    this->m_LstFiles.GetWindowRect(rcLstFiles);

    int nMargin = (this->rcInit_ResetCurrent.Height() / 3);

    if(nMode == VIEW_MODE_STANDARD)
        rcSettings.bottom = rcResetCurrent.top + nMargin;
    else if(nMode == VIEW_MODE_ADVANCED)
        rcSettings.bottom = rcSettings.top + this->rcInit_Settings.Height();

    int nLstFilesTop = rcSettings.bottom + (this->rcInit_LstFiles.top - this->rcInit_Settings.bottom);

    this->ScreenToClient(rcSettings);
    phSettings->MoveWindow(rcSettings, TRUE);

    if(nMode == VIEW_MODE_STANDARD)
        rcPresets.bottom = rcResetCurrent.top + nMargin;
    else if(nMode == VIEW_MODE_ADVANCED)
        rcPresets.bottom = rcPresets.top + this->rcInit_Presets.Height();
    this->ScreenToClient(rcPresets);
        phPresets->MoveWindow(rcPresets, TRUE);

    if((nMode == VIEW_MODE_STANDARD) || (nMode == VIEW_MODE_ADVANCED))
        rcLstFiles.top = nLstFilesTop;
    this->ScreenToClient(rcLstFiles);
    this->m_LstFiles.MoveWindow(rcLstFiles, TRUE);

    // check/uncheck proper View menu items
    this->GetMenu()->CheckMenuRadioItem(ID_VIEW_STANDARD, 
        ID_VIEW_ADVANCED,
        nMode == VIEW_MODE_STANDARD ? ID_VIEW_STANDARD : ID_VIEW_ADVANCED,
        0);

    // reset dialog anchors
    this->InitDialogAnchors();
}

void CEncWAVtoAC3Dlg::OnDropFiles(HDROP hDropInfo)
{
    if(bHandleDrop == true)
    {
        bHandleDrop = false;

        m_DDParam.pDlg = this;
        m_DDParam.hDropInfo = hDropInfo;

        hDDThread = ::CreateThread(NULL, 0, DragAndDropThread, (LPVOID) &m_DDParam, 0, &dwDDThreadID);
        if(hDDThread == NULL)
            bHandleDrop = true;
    }

    // under Win9x this does not work, we use seperate thread to handle drop
    // this->HandleDropFiles(hDropInfo);

    CResizeDialog::OnDropFiles(hDropInfo);
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
        CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
    }

    CResizeDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CEncWAVtoAC3Dlg::OnClose()
{
    // save configuration to disk (does not work in read-only mode)
    if(this->bSaveConfig == true)
    {
        this->SaveAllConfiguration();
    }

    // close log file
    if(this->bEnableLog == true)
    {
        ::LogClose(&logCtx);
    }

    CResizeDialog::OnClose();
}

void CEncWAVtoAC3Dlg::OnDestroy()
{
    CResizeDialog::OnDestroy();

    // clean-up memory used by tooltips
    this->m_LstFiles.DeleteAllItems();
    this->m_LstSettings.DeleteAllItems();
}

void CEncWAVtoAC3Dlg::OnFileAddFiles()
{
    TCHAR *pFiles = NULL;
    const DWORD dwMaxSize = (4096 * MAX_PATH);
    try
    {
        pFiles = (TCHAR *) malloc(dwMaxSize);
        if(pFiles == NULL)
        {
            MessageBox(_T("Failed to allocate memory for filenames buffer!"), 
                _T("Fatal Error"), 
                MB_OK | MB_ICONERROR);
            return;
        }

        ZeroMemory(pFiles, dwMaxSize);

        // get input file filter
        CString szFilter = GetSupportedInputFilesFilter();

        // configure open file dialog
        CFileDialog fd(TRUE, 
            szSupportedInputExt[0], 
            _T(""), 
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT, 
            szFilter);

        fd.m_ofn.lpstrFile = pFiles;
        fd.m_ofn.nMaxFile = (dwMaxSize) / 2;

        // show file dialog
        if(fd.DoModal() == IDOK)
        {
            POSITION pos;

            // get first file position
            pos = fd.GetStartPosition();
            while(pos != NULL)
            {
                // do not check file extension when using add file dialog because user can use *.* filter
                this->AddItemToFileList(fd.GetNextPathName(pos));
            }
        }
    }
    catch(...)
    {
        if(pFiles != NULL)
        {
            free(pFiles);
            pFiles = NULL;
        }
    }

    if(pFiles != NULL)
    {
        free(pFiles);
        pFiles = NULL;
    }
}

void CEncWAVtoAC3Dlg::OnFileAddDirectory()
{
    LPMALLOC pMalloc;
    BROWSEINFO bi; 
    LPITEMIDLIST pidlDesktop;
    LPITEMIDLIST pidlBrowse;
    TCHAR *lpBuffer;

    if(SHGetMalloc(&pMalloc) == E_FAIL)
        return;

    if((lpBuffer = (TCHAR *) pMalloc->Alloc(MAX_PATH * 2)) == NULL) 
    {
        pMalloc->Release();
        return; 
    }

    if(!SUCCEEDED(SHGetSpecialFolderLocation(::GetDesktopWindow(), CSIDL_DESKTOP, &pidlDesktop)))
    { 
        pMalloc->Free(lpBuffer); 
        pMalloc->Release();
        return; 
    } 

#ifndef BIF_NEWDIALOGSTYLE
        #define BIF_NEWDIALOGSTYLE 0x0040
#endif // BIF_NEWDIALOGSTYLE

    bi.hwndOwner = this->GetSafeHwnd(); 
    bi.pidlRoot = pidlDesktop; 
    bi.pszDisplayName = lpBuffer; 
    bi.lpszTitle = _T("Add directory with supported input files:");
    bi.lpfn = NULL; 
    bi.lParam = 0; 
    bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
    bi.iImage = 0;

    pidlBrowse = ::SHBrowseForFolder(&bi); 
    if(pidlBrowse != NULL)
    { 
        if(SHGetPathFromIDList(pidlBrowse, lpBuffer))
        {
            CString szBuff;
            szBuff.Format(_T("%s\0"), lpBuffer);

            // add all files in directories and all subdirs to file list
            SearchFolderForFiles(szBuff, true);
        }

        pMalloc->Free(pidlBrowse);
    } 

    pMalloc->Free(pidlDesktop); 
    pMalloc->Free(lpBuffer); 
    pMalloc->Release();
}

void CEncWAVtoAC3Dlg::OnFileMuxWizard()
{
    CEncWAVtoAC3MuxDlg dlg;

    // get number of files to process
    int nItemsCount = this->m_LstFiles.GetItemCount();

    // limit number of processed items
    if(nItemsCount > NUM_MAX_INPUT_FILES)
        nItemsCount = NUM_MAX_INPUT_FILES;

    for(int i = 0; i < nItemsCount; i++)
    {
        // get item file path
        dlg.szTmpInputFiles[i] = this->m_LstFiles.GetItemText(i, 0);
    }

    int nIndexAcmod = FindOptionIndex(_T("acmod"));
    int nIndexLfe = FindOptionIndex(_T("lfe"));
    int nIndexChconfig = FindOptionIndex(_T("chconfig"));
    bool bUpdateChconfig = false;

    // get current preset
    EncoderPreset tmpPreset = GetCurrentPreset();

    // set MUX dialog initial values
    if(encOpt[nIndexChconfig].nIgnoreValue != tmpPreset.nSetting[nIndexChconfig])
    {
       dlg.nChannelConfig = ccAften[encOpt[nIndexChconfig].listOptValues.GetAt(encOpt[nIndexChconfig].listOptValues.FindIndex(tmpPreset.nSetting[nIndexChconfig]))].acmod;
       dlg.bLFE = (ccAften[encOpt[nIndexChconfig].listOptValues.GetAt(encOpt[nIndexChconfig].listOptValues.FindIndex(tmpPreset.nSetting[nIndexChconfig]))].lfe == 1) ? true : false;
       bUpdateChconfig = true;
    }
    else
    {
        if(encOpt[nIndexAcmod].nIgnoreValue != tmpPreset.nSetting[nIndexAcmod])
        {
            dlg.nChannelConfig = tmpPreset.nSetting[nIndexAcmod];
        }
        else
        {
            int nDefault = encOpt[nIndexAcmod].listOptValues.GetCount() - 2;
            dlg.nChannelConfig = encOpt[nIndexAcmod].listOptValues.GetAt(encOpt[nIndexAcmod].listOptValues.FindIndex(nDefault));
        }

        dlg.bLFE = (tmpPreset.nSetting[nIndexLfe] == 1) ? true : false;
        bUpdateChconfig = false;
    }

    if(dlg.DoModal() == IDOK)
    {
        int nRet = IDYES;

        if(this->bDisableAllWarnings == false)
        {
            nRet = this->MessageBox(
                _T("Remove all files from list and add files selected in MUX Wizard?\n\n")
                _T("Note: The channel configuration for current preset will be adjusted."),
                _T("MUX Wizard"),
                MB_YESNO | MB_ICONQUESTION);
        }

        if(nRet == IDYES)
        {
            // clear list
            this->m_LstFiles.DeleteAllItems();

            // add items to file list
            switch(dlg.nChannelConfig)
            {
            case 0:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);
                this->AddItemToFileList(dlg.szInputFiles[3]);
                break;
            case 1:
                this->AddItemToFileList(dlg.szInputFiles[2]);
                this->AddItemToFileList(dlg.szInputFiles[3]);
                break;
            case 2:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);
                this->AddItemToFileList(dlg.szInputFiles[3]);
                break;
            case 3:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);
                this->AddItemToFileList(dlg.szInputFiles[2]);
                this->AddItemToFileList(dlg.szInputFiles[3]);
                break;
            case 4:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);

                if(dlg.bLFE == true)
                {
                    this->AddItemToFileList(dlg.szInputFiles[3]);
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                }
                else
                {
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                }
                break;
            case 5:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);
                this->AddItemToFileList(dlg.szInputFiles[2]);

                if(dlg.bLFE == true)
                {
                    this->AddItemToFileList(dlg.szInputFiles[3]);
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                }
                else
                {
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                }
                break;
            case 6:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);

                if(dlg.bLFE == true)
                {
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                    this->AddItemToFileList(dlg.szInputFiles[5]);

                    this->AddItemToFileList(dlg.szInputFiles[3]);
                }
                else
                {
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                    this->AddItemToFileList(dlg.szInputFiles[5]);
                }                
                break;
            case 7:
                this->AddItemToFileList(dlg.szInputFiles[0]);
                this->AddItemToFileList(dlg.szInputFiles[1]);
                this->AddItemToFileList(dlg.szInputFiles[2]);

                if(dlg.bLFE == true)
                {
                    this->AddItemToFileList(dlg.szInputFiles[3]);
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                    this->AddItemToFileList(dlg.szInputFiles[5]);
                }
                else
                {
                    this->AddItemToFileList(dlg.szInputFiles[4]);
                    this->AddItemToFileList(dlg.szInputFiles[5]);
                }
                break;
            };

            // update acmod value
            tmpPreset.nSetting[nIndexAcmod] = (bUpdateChconfig == true) ? encOpt[nIndexAcmod].nIgnoreValue : dlg.nChannelConfig;

            // set new acmod name value in settings list
            this->m_LstSettings.SetItemText(nIndexAcmod, 1, 
                ::encOpt[nIndexAcmod].listOptNames.GetAt(::encOpt[nIndexAcmod].listOptNames.FindIndex(tmpPreset.nSetting[nIndexAcmod])));

            // get index of lfe and update its value
            tmpPreset.nSetting[nIndexLfe] = (bUpdateChconfig == true) ? encOpt[nIndexLfe].nIgnoreValue : ((dlg.bLFE == true) ? 1 : 0);

            // set new acmod name value in settings list
            this->m_LstSettings.SetItemText(nIndexLfe, 1, 
                ::encOpt[nIndexLfe].listOptNames.GetAt(::encOpt[nIndexLfe].listOptNames.FindIndex(tmpPreset.nSetting[nIndexLfe])));

            // set new chconfig name value in settings list
            if(bUpdateChconfig == true)
            {
                int acmod = dlg.nChannelConfig;
                int lfe = (dlg.bLFE == true) ? 1 : 0;

                for(int i = 0; i < nNumChannelConfigAften; i++)
                {
                    if((ccAften[i].acmod == acmod) && (ccAften[i].lfe == lfe))
                    {
                        tmpPreset.nSetting[nIndexChconfig] = i;
                        break;
                    }
                }
            }
            else
            {
                tmpPreset.nSetting[nIndexChconfig] = encOpt[nIndexChconfig].nIgnoreValue;
            }

            // set new chconfig name value in settings list
            this->m_LstSettings.SetItemText(nIndexChconfig, 1, 
                ::encOpt[nIndexChconfig].listOptNames.GetAt(::encOpt[nIndexChconfig].listOptNames.FindIndex(tmpPreset.nSetting[nIndexChconfig])));

            // enable multi mono input if not enabled already
            if(this->bMultipleMonoInput == false)
            {
                this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
                this->OnBnClickedCheckMultipleMonoInput();
            }

            // update current preset
            UpdateCurrentPreset(tmpPreset);    
        }
    }
}

void CEncWAVtoAC3Dlg::OnFileLoadFilesList()
{
    CFileDialog fd(TRUE, 
        _T("files"), 
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

        // load files list from file
        LoadFilesList(szFileName);
    }
}

void CEncWAVtoAC3Dlg::OnFileSaveFilesList()
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

void CEncWAVtoAC3Dlg::OnFileLoadPresets()
{
    CFileDialog fd(TRUE, 
        _T("presets"), 
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

        // load presets list from file
        if(::LoadEncoderPresets(szFileName) == true)
        {
            // populate presets list
            this->m_CmbPresets.ResetContent();

            for(int i = 0; i < encPresets.GetCount(); i++)
            {
                this->m_CmbPresets.AddString(encPresets.GetAt(encPresets.FindIndex(i)).szName);
            }

            SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS);

            // select first preset
            ::nCurrentPreset = 0;
            this->m_CmbPresets.SetCurSel(0);

            // update all controls
            this->OnCbnSelchangeComboPresets();
        }
    }
}

void CEncWAVtoAC3Dlg::OnFileSavePresets()
{
    CFileDialog fd(FALSE, 
        _T("presets"), 
        _T(""), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

        // save presets list to file
        ::SaveEncoderPresets(szFileName);
    }
}

void CEncWAVtoAC3Dlg::OnFileExit()
{
    this->EndDialog(IDOK);
}

void CEncWAVtoAC3Dlg::OnOptionsEnableLogging()
{
    if(this->bEnableLog == false)
    {
        this->GetMenu()->CheckMenuItem(ID_OPTIONS_ENABLELOGGING, MF_CHECKED);
        this->bEnableLog = true;

        // open/create log file
        if(logCtx.bInit == false)
        {
            ::LogFile(&logCtx, this->szLogFile);
            ::LogOpen(&logCtx);
        }
    }
    else
    {
        this->GetMenu()->CheckMenuItem(ID_OPTIONS_ENABLELOGGING, MF_UNCHECKED);
        this->bEnableLog = false;

        // close log file
        if(logCtx.bInit == true)
        {
            ::LogClose(&logCtx);
        }
    }
}

void CEncWAVtoAC3Dlg::OnOptionsOpenLogFile()
{
    // open log file in external editor (notepad, etc.)
    LaunchAndWait(this->szLogFile, _T(""), FALSE);
}

void CEncWAVtoAC3Dlg::OnOptionsSetLogFilePath()
{
    CFileDialog fd(FALSE, 
        _T("log"), 
        this->szLogFile, // GetFileName(this->szLogFile), 
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
        _T("Supported Files (*.log;*.txt)|*.log;*.txt|LOG Files (*.log)|*.log|TXT Files (*.txt)|*.txt|All Files (*.*)|*.*||"), 
        this);

    if(fd.DoModal() == IDOK)
    {
        // get full path from filedialog
        CString szFileName = fd.GetPathName();

        // save log file path
        this->szLogFile = szFileName;

        // close old log file
        if(logCtx.bInit == true)
        {
            ::LogClose(&logCtx);
        }

        // create new log file
        ::LogFile(&logCtx, this->szLogFile);
        ::LogOpen(&logCtx);
    }
}

void CEncWAVtoAC3Dlg::OnOptionsDisableAllWarnings()
{
    this->bDisableAllWarnings = this->bDisableAllWarnings ? false : true;
    this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, 
        this->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);
}

void CEncWAVtoAC3Dlg::OnOptionsSaveConfigurationOnExit()
{
    this->bSaveConfig = this->bSaveConfig ? false : true;
    this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, 
        this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);
}

void CEncWAVtoAC3Dlg::OnOptionsLoadConfiguration()
{
    this->LoadAllConfiguration();
}

void CEncWAVtoAC3Dlg::OnOptionsSaveConfiguration()
{
    // save configuration to disk (works also in read-only mode)
    this->SaveAllConfiguration();
}

void CEncWAVtoAC3Dlg::OnViewStandard()
{
    // set view mode to Standard
    this->UpdateView(VIEW_MODE_STANDARD);
    m_ChkAdvancedView.SetCheck(BST_UNCHECKED);
}

void CEncWAVtoAC3Dlg::OnViewAdvanced()
{
    // set view mode to Advanced
    this->UpdateView(VIEW_MODE_ADVANCED);
    m_ChkAdvancedView.SetCheck(BST_CHECKED);
}

void CEncWAVtoAC3Dlg::OnHelpCommandLine()
{
    ShowCommandLineHelp(this->GetSafeHwnd());
}

void CEncWAVtoAC3Dlg::OnHelpWebsite()
{
    // go to program website using default internet browser
    LaunchAndWait(ENCWAVTOAC3_URL_HOME, _T(""), FALSE);
}

void CEncWAVtoAC3Dlg::OnHelpAbout()
{
    CAboutDlg dlg;

    // show program about dialog box
    dlg.DoModal();
}

void CEncWAVtoAC3Dlg::OnListAddFiles()
{
    this->OnFileAddFiles();
}

void CEncWAVtoAC3Dlg::OnListAddDirectory()
{
    this->OnFileAddDirectory();
}

void CEncWAVtoAC3Dlg::OnListMuxWizard()
{
    this->OnFileMuxWizard();
}

void CEncWAVtoAC3Dlg::OnListLoadList()
{
    this->OnFileLoadFilesList();
}

void CEncWAVtoAC3Dlg::OnListSavelist()
{
    this->OnFileSaveFilesList();
}

void CEncWAVtoAC3Dlg::OnListMoveUp()
{
    // move files list items one position up
    POSITION pos;
    CString szPath[2] = { _T(""), _T("") };
    CString szSize[2] = { _T(""), _T("") };

    pos = this->m_LstFiles.GetFirstSelectedItemPosition();
    while(pos != NULL)
    {
        int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
        if((nItem > 0) && (this->m_LstFiles.GetItemCount() >= 2))
        {
            szPath[0] = this->m_LstFiles.GetItemText(nItem, 0);
            szSize[0] = this->m_LstFiles.GetItemText(nItem, 1);

            szPath[1] = this->m_LstFiles.GetItemText(nItem - 1, 0);
            szSize[1] = this->m_LstFiles.GetItemText(nItem - 1, 1);

            this->m_LstFiles.SetItemText(nItem, 0, szPath[1]);
            this->m_LstFiles.SetItemText(nItem, 1, szSize[1]);

            this->m_LstFiles.SetItemText(nItem - 1, 0, szPath[0]);
            this->m_LstFiles.SetItemText(nItem - 1, 1, szSize[0]);

            this->m_LstFiles.SetItemState(nItem - 1,  LVIS_SELECTED, LVIS_SELECTED);
            this->m_LstFiles.SetItemState(nItem, 0, LVIS_SELECTED);
        }
        else
        {
            return;
        }
    }
}

void CEncWAVtoAC3Dlg::OnListMoveDown()
{
    // move files list items one position down
    POSITION pos;
    CString szPath[2] = { _T(""), _T("") };
    CString szSize[2] = { _T(""), _T("") };
    CList<ItemToMove, ItemToMove> listSel;

    pos = this->m_LstFiles.GetFirstSelectedItemPosition();
    while(pos != NULL)
    {
        int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
        if((nItem < this->m_LstFiles.GetItemCount() - 1) && (this->m_LstFiles.GetItemCount() >= 2))
        {
            ItemToMove item;

            item.nItem0 = nItem;
            item.nItem1 = nItem + 1;

            listSel.AddTail(item);
        }
        else
        {
            return;
        }
    }

    pos = listSel.GetTailPosition();
    while(pos != NULL)
    {
        ItemToMove item;
        item = listSel.GetPrev(pos);

        szPath[0] = this->m_LstFiles.GetItemText(item.nItem0, 0);
        szSize[0] = this->m_LstFiles.GetItemText(item.nItem0, 1);

        szPath[1] = this->m_LstFiles.GetItemText(item.nItem1, 0);
        szSize[1] = this->m_LstFiles.GetItemText(item.nItem1, 1);

        this->m_LstFiles.SetItemText(item.nItem0, 0, szPath[1]);
        this->m_LstFiles.SetItemText(item.nItem0, 1, szSize[1]);

        this->m_LstFiles.SetItemText(item.nItem1, 0, szPath[0]);
        this->m_LstFiles.SetItemText(item.nItem1, 1, szSize[0]);

        this->m_LstFiles.SetItemState(item.nItem1, LVIS_SELECTED, LVIS_SELECTED);
        this->m_LstFiles.SetItemState(item.nItem0, 0, LVIS_SELECTED);
    }

    listSel.RemoveAll();
}

void CEncWAVtoAC3Dlg::OnListDelFiles()
{
    CList<int,int> list;
    POSITION pos;

    // get all selected items
    pos = this->m_LstFiles.GetFirstSelectedItemPosition();
    while(pos != NULL)
    {
        list.AddTail(this->m_LstFiles.GetNextSelectedItem(pos));
    }

    // remove all selected items
    pos = list.GetTailPosition();
    while(pos != NULL)
    {
        this->m_LstFiles.DeleteItem(list.GetPrev(pos));
    }
}

void CEncWAVtoAC3Dlg::OnListClearList()
{
    this->m_LstFiles.DeleteAllItems();
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonAdd()
{
    this->OnFileAddFiles();
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonEncode()
{
    static bool bWorking = false;
    if(bWorking == true)
        return;

    // get number of files to process
    int nItemsCount = this->m_LstFiles.GetItemCount();

    // do nothing if there are no files in list
    if(nItemsCount <= 0)
    {
        MessageBox(_T("Add at least one file to the file list!"), _T("Error"), MB_ICONERROR | MB_OK);
        return;
    }

    // check if correct number of mono input files is present
    if((this->bMultipleMonoInput == true) && (nItemsCount <= 1 || nItemsCount > 6))
    {
        MessageBox(_T("Supported are minimum 2 and maximum 6 mono input files!"), _T("Error"), MB_ICONERROR | MB_OK);
        return;
    }

    bWorking = true;

    if(OpenAftenAPI(&this->api) == false)
    {
        MessageBox(_T("Failed to load libaften.dll dynamic library!"), _T("Error"), MB_ICONERROR | MB_OK);
        bWorking = false;
        return;
    }

    CEncWAVtoAC3WorkDlg dlg;

    // get all files from list
    CList<CString,CString> list;
    CList<bool,bool> listStatus;
    dlg.nTotalSize = 0;
    CString szSizeBuff;
    CString szFileBuffer;
    bool bAvisynthInput = false;

    for(int i = 0; i < nItemsCount; i++)
    {
        // check for avisynth scipts
        szFileBuffer = this->m_LstFiles.GetItemText(i, 0);

#ifndef DISABLE_AVISYNTH
        if(GetFileExt(szFileBuffer).MakeLower() == _T("avs"))
            bAvisynthInput = true;
#endif

        // get item file path
        list.AddTail(szFileBuffer);

        // set encoded status
        listStatus.AddTail(false);

        // get item file size
        szSizeBuff = this->m_LstFiles.GetItemText(i, 1);
        dlg.nTotalSize += _ttoi64(szSizeBuff);
    }

#ifndef DISABLE_AVISYNTH
    // check if we can process avisynth *.avs script (check all files in the list)
    // 1. 'Multiple mono input' mode - off
    if((this->bMultipleMonoInput == true) && (bAvisynthInput == true))
    {
        MessageBox(_T("Disable 'Multiple mono input' mode in order to use Avisynth scripts!"), 
            _T("Error"), MB_ICONERROR | MB_OK);
        bWorking = false;
        return;
    }
#endif

    // set pointer to files list
    dlg.workParam.list = &list;

    // set pointer to status list
    dlg.workParam.listStatus = &listStatus;

    // get output file/path
    this->m_EdtOutPath.GetWindowText(dlg.workParam.szOutPath);
    dlg.workParam.bUseOutPath = false;

    int nLen = dlg.workParam.szOutPath.GetLength();
    if(nLen < 3)
    {
        this->MessageBox(_T("Invalid output path!"), _T("Error"), MB_OK | MB_ICONERROR);
        bWorking = false;
        return;
    }

    CString szExt = dlg.workParam.szOutPath.Right(4);
    if(this->bMultipleMonoInput == true)
    {
        if(dlg.workParam.szOutPath.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0)
        {
            if((nLen < 4) || (szExt.CompareNoCase(_T(".ac3")) != 0))
            {
                this->MessageBox(_T("Invalid output file!"), _T("Error"), MB_OK | MB_ICONERROR);
                bWorking = false;
                return;
            }
        }
    }

    if((dlg.workParam.szOutPath.Compare(_T("")) != 0) && 
        ((dlg.workParam.szOutPath.Compare(DEFAULT_TEXT_OUTPUT_PATH) != 0 &&  this->bMultipleMonoInput == false)  ||
        (dlg.workParam.szOutPath.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0 && this->bMultipleMonoInput == true)))
    {
        if(this->bMultipleMonoInput == false)
        {
            if(MakeFullPath(dlg.workParam.szOutPath) == false)
            {
                // show error message
                this->MessageBox(_T("Failed to create output path!"), _T("Error"), MB_OK | MB_ICONERROR);
                bWorking = false;
                return;
            }
        }
        else
        {
            CString szTmpOutPath = dlg.workParam.szOutPath;
            CString szFile = GetFileName(dlg.workParam.szOutPath);

            szTmpOutPath.Truncate(szTmpOutPath.GetLength() - szFile.GetLength());

            if(MakeFullPath(szTmpOutPath) == false)
            {
                // show error message
                this->MessageBox(_T("Failed to create output path!"), _T("Error"), MB_OK | MB_ICONERROR);
                bWorking = false;
                return;
            }
        }

        dlg.workParam.bUseOutPath = true;
    }

    // multi mono input
    dlg.workParam.bMultiMonoInput = this->bMultipleMonoInput;

    // copy aften api for worker dialog
    dlg.workParam.api = this->api;

    // execution time counter variables
    CMyCounter countTime;
    CString szText;

    countTime.Init();

    // show work dialog
    countTime.Start();
    dlg.DoModal();
    countTime.Stop();

    // remove encoded files from list
    for(INT_PTR i = listStatus.GetSize() - 1; i >= 0; i--)
    {
        if(listStatus.GetAt(listStatus.FindIndex(i)) == true)
        {
            this->m_LstFiles.DeleteItem(i);
        }
    }

    // show total work time
    if(dlg.nCount <= 0)
    {
        szText.Format(_T(""));
    }
    else
    {
        if(this->bMultipleMonoInput == true)
        {
        szText.Format(_T("Encoded %d mono files in %s (%0.3lf sec)"), 
            dlg.nCount, 
            FormatTime(countTime.Time(), 3), 
            countTime.Time());
        }
        else
        {
        szText.Format(_T("Encoded %d file%s in %s (%0.3lf sec)"), 
            dlg.nCount, 
            dlg.nCount == 1 ? _T("") : _T("s"),
            FormatTime(countTime.Time(), 3), 
            countTime.Time());
        }
    }

    this->m_StatusBar.SetText(szText, 0, 0);

    bWorking = false;
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckSimdMmx()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[0] = this->m_ChkSimdMMX.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckSimdSse()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[1] = this->m_ChkSimdSSE.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckSimdSse2()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[2] = this->m_ChkSimdSSE2.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckSimdSse3()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[3] = this->m_ChkSimdSSE3.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckVbr()
{
    if(this->m_ChkVbr.GetCheck() == BST_CHECKED)
    {
        // switch to VBR mode
        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nMode = AFTEN_ENC_MODE_VBR;
        UpdateCurrentPreset(tmpPreset);
        this->m_SldBitrate.SetRange(0, 1023, TRUE);
        int nNewPos = GetCurrentPreset().nQuality;
        this->m_SldBitrate.SetPos(nNewPos);
    }
    else
    {
        // switch to CBR mode
        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nMode = AFTEN_ENC_MODE_CBR;
        UpdateCurrentPreset(tmpPreset);
        this->m_SldBitrate.SetRange(0, 19, TRUE);
        int nNewPos = FindValidBitratePos(GetCurrentPreset().nBitrate);
        this->m_SldBitrate.SetPos(nNewPos);
    }

    this->UpdateBitrateText();
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonPresetAdd()
{
    EncoderPreset newPreset = GetCurrentPreset();

    static int nCount = 0;

    newPreset.szName.Format(_T("New preset (%d)"), nCount++);
    ::encPresets.AddTail(newPreset);

    ::nCurrentPreset = ::encPresets.GetCount() - 1;
    this->m_CmbPresets.InsertString(::nCurrentPreset, newPreset.szName);
    this->m_CmbPresets.SetCurSel(::nCurrentPreset);

    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS);
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonPresetDel()
{
    // we need at least one preset present
    if(::encPresets.GetCount() >= 2)
    {
        int nCount = this->m_CmbPresets.GetCount();
        int nPreset = this->m_CmbPresets.GetCurSel();

        ::encPresets.RemoveAt(::encPresets.FindIndex(nPreset));
        this->m_CmbPresets.DeleteString(nPreset);

        this->m_CmbPresets.SetCurSel(::nCurrentPreset);

        if(nPreset == (nCount - 1)) // last preset
        {
            this->m_CmbPresets.SetCurSel(nCount - 2);
            ::nCurrentPreset = nCount - 2;
        }
        else if(nPreset == 0) // first preset
        {
            this->m_CmbPresets.SetCurSel(0);
            ::nCurrentPreset = 0;
        }
        else // in the middle
        {
            this->m_CmbPresets.SetCurSel(nPreset);
            ::nCurrentPreset = nPreset;
        }

        SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS);

        // update all controls
        this->OnCbnSelchangeComboPresets();
    }
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonBrowse()
{
    // mono input uses output dir to set output file name
    // if file name is not present then 'mux.ac3' is used
    if(this->bMultipleMonoInput == true)
    {
        // configure save file dialog
        CFileDialog fd(FALSE, 
            szSupportedOutputExt[0], 
            _T(""), 
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER, 
            _T("AC3 Files (*.ac3)|*.ac3|")
            _T("All Files (*.*)|*.*||"));

        // show file dialog
        if(fd.DoModal() == IDOK)
        {
            CString szFileName;
            szFileName = fd.GetPathName();
            this->m_EdtOutPath.SetWindowText(szFileName);
            this->szOutputFile = szFileName;
        }
    }
    else
    {
        LPMALLOC pMalloc;
        BROWSEINFO bi; 
        LPITEMIDLIST pidlDesktop;
        LPITEMIDLIST pidlBrowse;
        TCHAR *lpBuffer;

        if(SHGetMalloc(&pMalloc) == E_FAIL)
            return;

        if((lpBuffer = (TCHAR *) pMalloc->Alloc(MAX_PATH * 2)) == NULL) 
        {
            pMalloc->Release();
            return; 
        }

        if(!SUCCEEDED(SHGetSpecialFolderLocation(::GetDesktopWindow(), CSIDL_DESKTOP, &pidlDesktop)))
        { 
            pMalloc->Free(lpBuffer); 
            pMalloc->Release();
            return; 
        } 

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif // BIF_NEWDIALOGSTYLE

        bi.hwndOwner = this->GetSafeHwnd(); 
        bi.pidlRoot = pidlDesktop; 
        bi.pszDisplayName = lpBuffer; 
        bi.lpszTitle = _T("Select default output path:");
        bi.lpfn = NULL; 
        bi.lParam = 0; 
        bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
        bi.iImage = 0;

        pidlBrowse = ::SHBrowseForFolder(&bi); 
        if(pidlBrowse != NULL)
        { 
            if(SHGetPathFromIDList(pidlBrowse, lpBuffer))
            {
                CString szBuff;
                szBuff.Format(_T("%s\0"), lpBuffer);
                this->m_EdtOutPath.SetWindowText(szBuff);
                this->szOutputPath = szBuff;
            }

            pMalloc->Free(pidlBrowse);
        } 

        pMalloc->Free(pidlDesktop); 
        pMalloc->Free(lpBuffer); 
        pMalloc->Release();
    }

    return;
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonPresetsDefaults()
{
    int nPreset = this->m_CmbPresets.GetCurSel();
    if(nPreset != CB_ERR)
    {
        ::nCurrentPreset = nPreset;

        // load default preset and set all settings
        EncoderPreset tmpPreset = defaultPreset;
        tmpPreset.szName = GetCurrentPreset().szName;
        UpdateCurrentPreset(tmpPreset);

        // apply current preset to main dialog
        this->ApplyPresetToDlg(tmpPreset);
    }
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckMultipleMonoInput()
{
    this->bMultipleMonoInput = this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED ? true : false;

    if(this->bMultipleMonoInput == true)
    {
        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(_T("Output file:"));
    }
    else
    {
        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(_T("Output path:"));
    }

    CString szBuff = this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath;

    if(szBuff.Compare(_T("")) == 0 ||
        szBuff.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0 || 
        szBuff.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
    {
        this->m_EdtOutPath.SetWindowText(this->bMultipleMonoInput == true ? DEFAULT_TEXT_OUTPUT_FILE : DEFAULT_TEXT_OUTPUT_PATH);
    }
    else
    {
        this->m_EdtOutPath.SetWindowText(this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath);
    }
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonMuxWizard()
{
    this->OnFileMuxWizard();
}

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboSetting()
{
    POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int nItem = this->m_LstSettings.GetNextSelectedItem(pos);

        // get current selection index in value combobox
        int nVal = this->m_CmbValue.GetCurSel();

        EncoderPreset tmpPreset = GetCurrentPreset();
        tmpPreset.nSetting[nItem] = nVal;

        UpdateCurrentPreset(tmpPreset);

        CString szName = ::encOpt[nItem].listOptNames.GetAt(::encOpt[nItem].listOptNames.FindIndex(nVal));

        // set new value name in settings list
        this->m_LstSettings.SetItemText(nItem, 1, szName);
    }
}

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboPresets()
{
    int nPreset = this->m_CmbPresets.GetCurSel();
    if(nPreset != CB_ERR)
    {
        ::nCurrentPreset = nPreset;

        // load selected preset and set all settings
        EncoderPreset tmpPreset = GetCurrentPreset();

        // apply current preset to main dialog
        this->ApplyPresetToDlg(tmpPreset);
    }
}

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboEngines()
{
    // unload currently used aften library
    CloseAftenAPI(&this->api);

    // get newly selected engine by user
    int nSel = this->m_CmbEngines.GetCurSel();
    if(nSel == CB_ERR)
    {
        // nothing is selected
        return;
    }

    // update current engine id
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nCurrentEngine = nSel;
    UpdateCurrentPreset(tmpPreset);

    // change current directory
    ::SetCurrentDirectory(GetExeFilePath());

    // load new aften library
    this->api.szLibPath = m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szData;
    if(OpenAftenAPI(&this->api) == false)
    {
        // show error message in statusbar
        CString szLogMessage = _T("Failed to load '") +
            m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szKey +
            _T("' library!");
        this->m_StatusBar.SetText(szLogMessage, 0, 0);

        // this->nCurrentEngine = -1;
        // MessageBox(_T("Unable to load Aften API!"), _T("Error"), MB_ICONERROR | MB_OK);
        return;
    }
    else
    {
        // get Aften library version (Ansi string)
        const char *szAftenVersionAnsi = this->api.LibAften_aften_get_version();
        int nVersionLen = strlen(szAftenVersionAnsi);

#ifdef _UNICODE
        TCHAR szAftenVersion[256] = _T("");
        ZeroMemory(szAftenVersion, 256 * sizeof(TCHAR));
        int nChars = MultiByteToWideChar(CP_ACP, 
            MB_PRECOMPOSED, 
            szAftenVersionAnsi, nVersionLen,
            szAftenVersion, 256); 
        if(nChars == 0)
            _stprintf(szAftenVersion, _T("?.??"));
#else
        const char *szAftenVersion = szAftenVersionAnsi;
#endif // _UNICODE

        // show Aften build and version info in statusbar
        CString szLogMessage = _T("Loaded '") +
            m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szKey +
            _T("' library, version ") + 
            szAftenVersion;
        this->m_StatusBar.SetText(szLogMessage, 0, 0);
    }
}

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboRawSampleFormat()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nRawSampleFormat = this->m_CmbRawSampleFormat.GetCurSel();
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnLvnItemchangedListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    static int nLastItem = -1;

    POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int nItem = m_LstSettings.GetNextSelectedItem(pos);

        // update values combobox if item has changed
        if(nLastItem != nItem)
        {
            this->UpdateSettingsComboBox(nItem);

            nLastItem = nItem;
        }
    }

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnLvnKeydownListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    // handle keyboard events here
    switch(pLVKeyDow->wVKey)
    {
    case VK_DELETE: 
        {
            this->OnListDelFiles();
        }
        break;
    case VK_UP: // left Alt + Up
        {
            if(GetAsyncKeyState(VK_LMENU) & 0x8001)
                this->OnListMoveUp();
        }
        break;
    case VK_DOWN: // left Alt + Down
        {
            if(GetAsyncKeyState(VK_LMENU) & 0x8001)
                this->OnListMoveDown();
        }
        break;

    default: break;
    };

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnLvnColumnclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    nSortColumn = pNMLV->iSubItem;

    this->m_LstFiles.SortItems(CompareFunction, reinterpret_cast<DWORD_PTR>(this));

    if(nSortOrder[nSortColumn] == true)
        nSortOrder[nSortColumn] = false;
    else
        nSortOrder[nSortColumn] = true;

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnLvnKeydownListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    // handle keyboard events here
    switch(pLVKeyDow->wVKey)
    {
    case VK_LEFT: 
        {
            POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
            if(pos != NULL)
            {
                int nItem = this->m_LstSettings.GetNextSelectedItem(pos);

                // get current selection index in value combobox
                int nVal = this->m_CmbValue.GetCurSel();

                // select previous item in value combobox
                if(nVal > 0)
                {
                    nVal = nVal - 1;
                    this->m_CmbValue.SetCurSel(nVal);
                }
                else
                {
                    return;
                }

                EncoderPreset tmpPreset = GetCurrentPreset();
                tmpPreset.nSetting[nItem] = nVal;

                UpdateCurrentPreset(tmpPreset);

                CString szName = ::encOpt[nItem].listOptNames.GetAt(::encOpt[nItem].listOptNames.FindIndex(nVal));

                // set new value name in settings list
                this->m_LstSettings.SetItemText(nItem, 1, szName);
            }
        }
        break;
    case VK_RIGHT: 
        {
            POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
            if(pos != NULL)
            {
                int nItem = this->m_LstSettings.GetNextSelectedItem(pos);

                // get current selection index in value combobox
                int nVal = this->m_CmbValue.GetCurSel();

                // select next item in value combobox
                if(nVal < this->m_CmbValue.GetCount() - 1)
                {
                    nVal = nVal + 1;
                    this->m_CmbValue.SetCurSel(nVal);
                }
                else
                {
                    return;
                }

                EncoderPreset tmpPreset = GetCurrentPreset();
                tmpPreset.nSetting[nItem] = nVal;

                UpdateCurrentPreset(tmpPreset);

                CString szName = ::encOpt[nItem].listOptNames.GetAt(::encOpt[nItem].listOptNames.FindIndex(nVal));

                // set new value name in settings list
                this->m_LstSettings.SetItemText(nItem, 1, szName);
            }
        }
        break;
    default: 
        {

        }
        break;
    };

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnNMRclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    // get starting point for context menu
    POINT point;
    GetCursorPos(&point);

    // show right click context menu
    CMenu menu;
    menu.LoadMenu(IDR_MENU_LIST);
    CMenu *subMenu = menu.GetSubMenu(0);
    ::SetForegroundWindow(this->GetSafeHwnd());
    subMenu->TrackPopupMenu(0, point.x, point.y, this, NULL);

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnNMRclickListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
    // show option values in popup menu
    this->ShowOptionPopup(true);

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnNMDblclkListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
    POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int nItem = m_LstSettings.GetNextSelectedItem(pos);

        // show option help text
        this->MessageBox(encOpt[nItem].szHelpText, encOpt[nItem].szName, MB_ICONINFORMATION | MB_OK);
    }

    *pResult = 0;
}

#ifndef DISABLE_AVISYNTH

bool CEncWAVtoAC3Dlg::GetAvisynthFileInfo(CString szFileName, AvsAudioInfo *pInfoAVS)
{
    TCHAR *pszInPath = szFileName.GetBuffer();

    if(pInfoAVS == NULL)
        return false;

    memset(pInfoAVS, 0, sizeof(AvsAudioInfo));

    // show AVS file information text
    CAvs2Raw decoderAVS;
    char szInputFileAVS[MAX_PATH] = "";

    // initialize Avisynth - only one input file supported
    // NOTE: only Ansi file names supported
#ifdef _UNICODE
    ConvertUnicodeToAnsi(pszInPath, szInputFileAVS, lstrlen(pszInPath)); 
    if(decoderAVS.OpenAvisynth(szInputFileAVS) == false)
#else
    if(decoderAVS.OpenAvisynth(pszInPath) == false)
#endif
    {
        this->MessageBox(_T("Failed to initialize Avisynth"), _T("Error"), MB_ICONERROR | MB_OK);
        return false;
    }
    else
    {
        // get input Audio stream information from Avisynth
        (*pInfoAVS) = decoderAVS.GetInputInfo();

        // close Avisynth
        decoderAVS.CloseAvisynth();

        return true;
    }   
}

#endif

void CEncWAVtoAC3Dlg::OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    POSITION pos = m_LstFiles.GetFirstSelectedItemPosition();
    if(pos != NULL)
    {
        int nItem = m_LstFiles.GetNextSelectedItem(pos);
        CString szFileName = m_LstFiles.GetItemText(nItem, 0);

#ifndef DISABLE_AVISYNTH
        // show AVS file information text
        if(GetFileExt(szFileName).MakeLower() == _T("avs"))
		{
            // get input Audio stream information from Avisynth
            AvsAudioInfo infoAVS;
            memset(&infoAVS, 0, sizeof(AvsAudioInfo));
            if(GetAvisynthFileInfo(szFileName, &infoAVS))
            {
                CString szInfo;

                TCHAR *pszInPath = szFileName.GetBuffer();

                // get inforamtion about input file
                szInfo += _T("Sample format\t: ");
                switch(infoAVS.nSampleType)
                {
                case SAMPLE_INT8:
                    szInfo += _T("SAMPLE_INT8\n");
                    break;
                case SAMPLE_INT16:
                    szInfo += _T("SAMPLE_INT16\n");
                    break;
                case SAMPLE_INT24:
                    szInfo += _T("SAMPLE_INT24\n");
                    break;
                case SAMPLE_INT32:
                    szInfo += _T("SAMPLE_INT32\n");
                    break;
                case SAMPLE_FLOAT:
                    szInfo += _T("SAMPLE_FLOAT\n");
                    break;
                default:
                    szInfo += _T("unknown\n");
                    break;
                }

                CString szBuff;
                szBuff.Format(_T("Sample rate\t: %d\n"), infoAVS.nSamplesPerSecond);
                szInfo += szBuff;
                szBuff.Format(_T("Channels\t\t: %d\n"), infoAVS.nAudioChannels);
                szInfo += szBuff;
                szBuff.Format(_T("Audio samples\t: %I64d\n"), infoAVS.nAudioSamples);
                szInfo += szBuff;
                szBuff.Format(_T("Decoded size\t: %I64d"), 
                    infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels);
                szInfo += szBuff;

                szFileName.ReleaseBuffer();

                // show infor to user
                this->MessageBox(szInfo, _T("AVS File Properties"), MB_ICONINFORMATION | MB_OK);
            }
        } 
#endif
    }

    *pResult = 0;
}

void CEncWAVtoAC3Dlg::OnEnChangeEditRawSampleRate()
{
    // check if number is in range
    CString szBuff;
    int nPos;

    this->m_EdtRawSamplerate.GetWindowText(szBuff);

    // check if we have focus (needed for editing)
    if(szBuff.Compare(_T("")) == 0)
        return;

    if(szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
    {
        nPos = 0;
        this->m_SpnRawSampleRate.SetPos(nPos);
    }
    else
    {
        nPos = (int) _ttoi(szBuff);
        if((nPos <= 0) || (nPos > INT_MAX))
        {
            nPos = 0;
            this->m_SpnRawSampleRate.SetPos(0);
            this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
        }
        else
        {
            this->m_SpnRawSampleRate.SetPos(nPos);
        }
    }

    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nRawSampleRate = nPos;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnEnChangeEditRawChannels()
{
    // check if number is in range
    CString szBuff;
    int nPos;

    this->m_EdtRawChannels.GetWindowText(szBuff);

    // check if we have focus (needed for editing)
    if(szBuff.Compare(_T("")) == 0)
        return;

    if(szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
    {
        nPos = 0;
        this->m_SpnRawChannels.SetPos(nPos);
    }
    else
    {
        nPos = (int) _ttoi(szBuff);
        if((nPos <= 0) || (nPos > INT_MAX))
        {
            nPos = 0;
            this->m_SpnRawChannels.SetPos(0);
            this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
        }
        else
        {
            this->m_SpnRawChannels.SetPos(nPos);
        }
    }

    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nRawChannels = nPos;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnEnChangeEditThreads()
{
    // check if number is in range
    CString szBuff;
    int nPos;

    this->m_EdtThreads.GetWindowText(szBuff);

    // check if we have focus (needed for editing)
    if(szBuff.Compare(_T("")) == 0)
        return;

    if(szBuff.Compare(DEFAULT_TEXT_AUTO) == 0)
    {
        nPos = 0;
        this->m_SpnThreads.SetPos(nPos);
    }
    else
    {
        nPos = (int) _ttoi(szBuff);
        if((nPos <= 0) || (nPos > INT_MAX))
        {
            nPos = 0;
            this->m_SpnThreads.SetPos(0);
            this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
        }
        else
        {
            this->m_SpnThreads.SetPos(nPos);
        }
    }

    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nThreads = nPos;
    UpdateCurrentPreset(tmpPreset);
}

void CEncWAVtoAC3Dlg::OnEnChangeEditOutputPath()
{
    CString szBuff;

    this->m_EdtOutPath.GetWindowText(szBuff);

    // update output file/path value
    if(this->bMultipleMonoInput == true)
        this->szOutputFile = szBuff;
    else
        this->szOutputPath = szBuff;
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditOutputPath()
{
    CString szBuff;
    this->m_EdtOutPath.GetWindowText(szBuff);
    if(szBuff.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0 || szBuff.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
        this->m_EdtOutPath.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditRawSampleRate()
{
    CString szBuff;
    this->m_EdtRawSamplerate.GetWindowText(szBuff);
    if(szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
        this->m_EdtRawSamplerate.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditRawChannels()
{
    CString szBuff;
    this->m_EdtRawChannels.GetWindowText(szBuff);
    if(szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
        this->m_EdtRawChannels.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnSetfocusEditThreads()
{
    CString szBuff;
    this->m_EdtThreads.GetWindowText(szBuff);
    if(szBuff.Compare(DEFAULT_TEXT_AUTO) == 0)
        this->m_EdtThreads.SetWindowText(_T(""));
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditOutputPath()
{
    CString szBuff;
    this->m_EdtOutPath.GetWindowText(szBuff);
    if(szBuff.Compare(_T("")) == 0)
    {
        if(this->bMultipleMonoInput == true)
            this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
        else
            this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
    }
    else
    {
        if(this->bMultipleMonoInput == true)
            this->szOutputFile = szBuff;
        else
            this->szOutputPath = szBuff;
    }
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditRawSampleRate()
{
    CString szBuff;
    this->m_EdtRawSamplerate.GetWindowText(szBuff);
    if(szBuff.Compare(_T("")) == 0)
        this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditRawChannels()
{
    CString szBuff;
    this->m_EdtRawChannels.GetWindowText(szBuff);
    if(szBuff.Compare(_T("")) == 0)
        this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
}

void CEncWAVtoAC3Dlg::OnEnKillfocusEditThreads()
{
    CString szBuff;
    this->m_EdtThreads.GetWindowText(szBuff);
    if(szBuff.Compare(_T("")) == 0)
        this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
}

LRESULT CEncWAVtoAC3Dlg::EditChangeComboPresets(WPARAM wParam, LPARAM lParam)
{
    if((lParam == (LPARAM) 0) && (wParam != NULL))
    {
        int nPreset = this->m_CmbPresets.GetCurSel();
        if(nPreset != CB_ERR)
        {
            // there is always one preset present (Default)
            CString *szName = (CString *) wParam;

            DWORD dwEditSel = this->m_CmbPresets.GetEditSel();

            // update current preset name
            EncoderPreset tmpPreset = GetCurrentPreset();
            tmpPreset.szName = *szName;
            UpdateCurrentPreset(tmpPreset);

            // update preset name in combobox
            this->m_CmbPresets.DeleteString(nPreset);
            this->m_CmbPresets.InsertString(nPreset, *szName);
            this->m_CmbPresets.SetCurSel(nPreset);
            this->m_CmbPresets.SetEditSel(HIWORD(dwEditSel), LOWORD(dwEditSel));
            // TRACE(_T("Preset=%s\n"), *szName);
        }
    }

    return(0);
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonEngines()
{
    CEncWAVtoAC3EngDlg dlg;

    dlg.nCurrSel = this->m_CmbEngines.GetCurSel();

    // copy engines list to engines editor dialog
    int nSize = this->m_EngineList.GetSize();
    POSITION pos = this->m_EngineList.GetHeadPosition();
    for(INT_PTR i = 0; i < nSize; i++)
    {
        ConfigEntry ce;

        // get next entry in configuration list
        ce = this->m_EngineList.GetNext(pos);

        // insert all items to new engines list
        dlg.m_EngineList.AddTail(ce);
    }

    // show pengines editor dialog box
    if(dlg.DoModal() == IDOK)
    {
        // init engines configuration
        this->m_EngineList.RemoveAll();
        this->m_CmbEngines.ResetContent();

        // copy new engines from editor dialog to list
        int nSize = dlg.m_EngineList.GetSize();
        POSITION pos = dlg.m_EngineList.GetHeadPosition();
        for(INT_PTR i = 0; i < nSize; i++)
        {
            ConfigEntry ce;

            // get next entry in configuration list
            ce = dlg.m_EngineList.GetNext(pos);

            // insert all items to new engines list
            this->m_EngineList.AddTail(ce);
        }

        // update currently loaded program engines
        this->UpdateProgramEngines();

        // update engines combobox and preset
        this->OnCbnSelchangeComboEngines();
    }
}

void CEncWAVtoAC3Dlg::OnBnClickedCheckAdvancedView()
{
    if(m_ChkAdvancedView.GetCheck() == BST_UNCHECKED)
        this->UpdateView(VIEW_MODE_STANDARD);
    else
        this->UpdateView(VIEW_MODE_ADVANCED);
}
