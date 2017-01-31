//
// WAV to AC3 Encoder
// Copyright (C) 2007-2016 Wies³aw Šoltés <wieslaw.soltes@gmail.com>
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

void CEncWAVtoAC3Dlg::InitTitle()
{
    // set program name and version in main dialog title
    CString szDialogTitle = _T("");
    szDialogTitle.Format(_T("WAV to AC3 Encoder %s"), ENCWAVTOAC3_VERSION);
    this->SetWindowText(szDialogTitle);
}

void CEncWAVtoAC3Dlg::InitDialogAnchors()
{
    CleanUp();
    AddAnchor(IDC_STATIC_QUALITY, AnchorTopLeft);
    AddAnchor(IDC_STATIC_BITRATE, AnchorTopRight);
    AddAnchor(IDC_SLIDER_BITRATE, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_CHECK_VBR, AnchorTopRight);
    AddAnchor(IDC_STATIC_ENGINE, AnchorTopLeft);
    AddAnchor(IDC_COMBO_ENGINES, AnchorTopLeft);
    AddAnchor(IDC_BUTTON_ENGINES, AnchorTopLeft);
    AddAnchor(IDC_STATIC_THREADS, AnchorTopLeft);
    AddAnchor(IDC_EDIT_THREADS, AnchorTopLeft);
    AddAnchor(IDC_SPIN_THREADS, AnchorTopLeft);
    AddAnchor(IDC_LIST_SETTINGS, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_STATIC_OPTION_VALUE, AnchorTopLeft);
    AddAnchor(IDC_COMBO_SETTING, AnchorTopLeft, AnchorTopRight);
    AddAnchor(IDC_STATIC_PRESET, AnchorTopLeft);
    AddAnchor(IDC_COMBO_PRESETS, AnchorTopLeft);
    AddAnchor(IDC_BUTTON_PRESETS_DEFAULTS, AnchorTopLeft);
    AddAnchor(IDC_BUTTON_PRESET_DEL, AnchorTopLeft);
    AddAnchor(IDC_BUTTON_PRESET_ADD, AnchorTopLeft);
    AddAnchor(IDC_STATIC_SIMD, AnchorTopLeft);
    AddAnchor(IDC_CHECK_SIMD_MMX, AnchorTopLeft);
    AddAnchor(IDC_CHECK_SIMD_SSE, AnchorTopLeft);
    AddAnchor(IDC_CHECK_SIMD_SSE2, AnchorTopLeft);
    AddAnchor(IDC_CHECK_SIMD_SSE3, AnchorTopLeft);
    AddAnchor(IDC_STATIC_SAMPLE_FORMAT, AnchorTopLeft);
    AddAnchor(IDC_COMBO_RAW_SAMPLE_FORMAT, AnchorTopLeft);
    AddAnchor(IDC_STATIC_SAMPLE_RATE, AnchorTopLeft);
    AddAnchor(IDC_EDIT_RAW_SAMPLE_RATE, AnchorTopLeft);
    AddAnchor(IDC_SPIN_RAW_SAMPLE_RATE, AnchorTopLeft);
    AddAnchor(IDC_STATIC_CHANNELS, AnchorTopLeft);
    AddAnchor(IDC_EDIT_RAW_CHANNELS, AnchorTopLeft);
    AddAnchor(IDC_SPIN_RAW_CHANNELS, AnchorTopLeft);
    AddAnchor(IDC_LIST_FILES, AnchorTopLeft, AnchorBottomRight);
    AddAnchor(IDC_BUTTON_ADD, AnchorBottomLeft);
    AddAnchor(IDC_STATIC_OUTPUT, AnchorBottomLeft);
    AddAnchor(IDC_EDIT_OUTPUT_PATH, AnchorBottomLeft, AnchorBottomRight);
    AddAnchor(IDC_BUTTON_BROWSE, AnchorBottomRight);
    AddAnchor(IDC_CHECK_MULTIPLE_MONO_INPUT, AnchorBottomLeft);
    AddAnchor(IDC_BUTTON_MUX_WIZARD, AnchorBottomLeft);
    AddAnchor(IDC_BUTTON_ENCODE, AnchorBottomRight);
    AddAnchor(IDC_STATUSBAR, AnchorBottomLeft, AnchorBottomRight);
}

void CEncWAVtoAC3Dlg::InitTooltips()
{
    // set tooltips
    CString szTmpText;

    // Bitrate/Quality Slider
    /* [-b #]         CBR bitrate in kbps */
    /* [-q #]         VBR quality */
    szTmpText = HaveLangStrings() ? GetLangString(0x00206001) :
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
    szTmpText = HaveLangStrings() ? GetLangString(0x00206002) :
        _T("Enable VBR mode. If unchecked the CBR mode is used instead.");

    this->m_ChkVbr.SetTooltipText(szTmpText);

    // Presets ComboBox
    szTmpText = HaveLangStrings() ? GetLangString(0x00206003) :
        _T("Set currently used encoder preset. You can load/save presets from/to file\n")
        _T("from File menu. All presets are automatically loaded/saved from/to text file.");

    this->m_CmbPresets.SetTooltipText(szTmpText);

    // SIMD instructions CheckBoxes
    /* [-nosimd X]    Comma-separated list of SIMD instruction sets not to use */
    szTmpText = HaveLangStrings() ? GetLangString(0x00206004) :
        _T("Aften will auto-detect available SIMD instruction sets\n")
        _T("for your CPU, so you shouldn't need to disable sets\n")
        _T("explicitly - unless for speed or debugging reasons.");

    this->m_ChkSimdMMX.SetTooltipText(HaveLangStrings() ? GetLangString(0x00206005) + szTmpText :
        _T("This option enables MMX optimizations (if supported by CPU).\n\n") + szTmpText);

    this->m_ChkSimdSSE.SetTooltipText(HaveLangStrings() ? GetLangString(0x00206006) + szTmpText :
        _T("This option enables SSE optimizations (if supported by CPU).\n\n") + szTmpText);

    this->m_ChkSimdSSE2.SetTooltipText(HaveLangStrings() ? GetLangString(0x00206007) + szTmpText :
        _T("This option enables SSE2 optimizations (if supported by CPU).\n\n") + szTmpText);

    this->m_ChkSimdSSE3.SetTooltipText(HaveLangStrings() ? GetLangString(0x00206008) + szTmpText :
        _T("This option enables SSE3 optimizations (if supported by CPU).\n\n") + szTmpText);

    // Sample format ComboBox
    /* [-raw_fmt X]   Raw audio input sample format (default: s16_le) */
    szTmpText = HaveLangStrings() ? GetLangString(0x00206009) :
        _T("Raw audio input sample format specifies the sample format\n")
        _T("when using raw audio input. Using this option forces Aften to\n")
        _T("treat the input as raw audio. The choices for the\n")
        _T("pre-defined sample formats are (default: s16_le):\n")
        _T("u8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,\n")
        _T("s32_le, s32_be, float_le, float_be, double_le, double_be");

    this->m_CmbRawSampleFormat.SetTooltipText(szTmpText);

    // Sample rate EditBox
    /* [-raw_sr #]    Raw audio input sample rate (default: 48000) */
    szTmpText = HaveLangStrings() ? GetLangString(0x0020600A) :
        _T("Raw audio input sample rate option forces Aften to\n")
        _T("treat the input as raw audio (default: 48000).");

    this->m_EdtRawSamplerate.SetTooltipText(szTmpText);

    // Channels EditBox
    /* [-raw_ch #]    Raw audio input channels (default: 2) */
    szTmpText = HaveLangStrings() ? GetLangString(0x0020600B) :
        _T("Raw audio input channels forces Aften to treat the input as\n")
        _T("raw audio (default: 2).");

    this->m_EdtRawChannels.SetTooltipText(szTmpText);

    // Threads EditBox
    /* [-threads #]   Number of threads */
    szTmpText = HaveLangStrings() ? GetLangString(0x0020600C) :
        _T("Aften can use multiple threads to speed up encoding.\n")
        _T("By default, Aften uses one thread for each logical CPU\n")
        _T("your system has, but you can override this value. A\n")
        _T("value of 0 is the default and indicates that Aften\n")
        _T("should try to detect the number of CPUs.");

    this->m_EdtThreads.SetTooltipText(szTmpText);

    // Engine ComboBox
    szTmpText = HaveLangStrings() ? GetLangString(0x0020600D) :
        _T("Set currently used Aften library. By selecting optimized Aften\n")
        _T("library you can speedup the encoding process.");

    this->m_CmbEngines.SetTooltipText(szTmpText);

    // Output path EditBox
    szTmpText = HaveLangStrings() ? GetLangString(0x0020600E) :
        _T("Set default output path for encoded files. By default files\n")
        _T("are encoded to the same directory as input files. When using\n")
        _T("multiple mono input than here is set the output file path.");

    this->m_EdtOutPath.SetTooltipText(szTmpText);

    // Multiple mono input CheckBox
    szTmpText = HaveLangStrings() ? GetLangString(0x0020600F) :
        _T("Enable multiple mono input mode. By adding multiple mono input\n")
        _T("files to the files list (minimum 2, maximum 6) in correct channel\n")
        _T("order the mono input files will be encoded into single ac3 file.");

    this->m_ChkMultipleMonoInput.SetTooltipText(szTmpText);

    // engines editor
    szTmpText = HaveLangStrings() ? GetLangString(0x00206010) :
        _T("Edit currently available Aften engines.");

    this->m_BtnEngines.SetTooltipText(szTmpText);
}

void CEncWAVtoAC3Dlg::InitSettingsList()
{
    int nGroupCounter = -1;

    LVITEM li = { 0 };
    li.mask = LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS;

    HWND listSettings = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();

    ListView_DeleteAllItems(listSettings);

    // fill advanced encoder options list
    for (int i = 0; i < nNumEncoderOptions; i++)
    {
        if (encOpt[i].bBeginGroup == true)
            nGroupCounter++;

        if (nGroupCounter >= 0 && nGroupCounter < nNumEncoderOptionsGroups)
        {
            li.pszText = encOpt[i].szName.GetBuffer();
            li.iItem = i;
            li.iSubItem = 0;
            li.iGroupId = 101 + nGroupCounter;

            ListView_InsertItem(listSettings, &li);
            ListView_SetItemText(listSettings, i, 1,
                encOpt[i].listOptNames.GetAt(encOpt[i].listOptNames.FindIndex(encOpt[i].nDefaultValue)).GetBuffer());

            this->m_LstSettings.listTooltips.AddTail(encOpt[i].szHelpText);

            encOpt[i].szName.ReleaseBuffer();
            encOpt[i].listOptNames.GetAt(encOpt[i].listOptNames.FindIndex(encOpt[i].nDefaultValue)).ReleaseBuffer();
        }
    }

    // enable tooltips for settings list
    this->m_LstSettings.bUseTooltipsList = true;

    // select first item in settings list
    this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void CEncWAVtoAC3Dlg::InitDefaultPreset()
{
    // set current settings to defaults
    for (int i = 0; i < nNumEncoderOptions; i++)
        defaultPreset.nSetting[i] = encOpt[i].nDefaultValue;

    // set default preset name
    defaultPreset.szName = DEFAULT_PRESET_NAME;

    // set Aften engine defaults values for first start (when there is no config file present)
    // there is Aften api function to get this values: AftenContext aftenCtx; aften_set_defaults(&aftenCtx);
    defaultPreset.nMode = AFTEN_ENC_MODE_CBR; // aftenCtx.params.encoding_mode;
    defaultPreset.nBitrate = 0; // aftenCtx.params.bitrate;
    defaultPreset.nQuality = 240; // aftenCtx.params.quality;
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
    this->nCurrentPreset = 0;
    this->m_CmbPresets.InsertString(0, defaultPreset.szName);
    this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

    // set raw audio input defaults
    this->m_CmbRawSampleFormat.SetCurSel(defaultPreset.nRawSampleFormat);

    if (defaultPreset.nRawSampleRate == 0)
    {
        this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
    }
    else
    {
        CString szBuff;
        szBuff.Format(_T("%d"), defaultPreset.nRawSampleRate);
        this->m_EdtRawSamplerate.SetWindowText(szBuff);
    }

    if (defaultPreset.nRawChannels == 0)
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
    if (defaultPreset.nThreads == 0)
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
    if (defaultPreset.nMode == AFTEN_ENC_MODE_CBR)
    {
        this->m_SldBitrate.SetTic(1);
        this->m_SldBitrate.SetRange(0, 19);
        this->m_SldBitrate.SetPos(FindValidBitratePos(defaultPreset.nBitrate));
        this->m_ChkVbr.SetCheck(BST_UNCHECKED);
    }
    else if (defaultPreset.nMode == AFTEN_ENC_MODE_VBR)
    {
        this->m_SldBitrate.SetTic(1);
        this->m_SldBitrate.SetRange(0, 1023);
        this->m_SldBitrate.SetPos(defaultPreset.nQuality);
        this->m_ChkVbr.SetCheck(BST_CHECKED);
    }
}

void CEncWAVtoAC3Dlg::InitRawSamleFormatComboBox()
{
    CString szIgnored = DEFAULT_TEXT_IGNORED;

    // update language string
    szRawSampleFormats[0] = (LPTSTR)(LPCTSTR)(szIgnored);

    // remove all items from sample format combobox
    this->m_CmbRawSampleFormat.ResetContent();

    // setup default values for raw audio input
    for (int i = 0; i < nNumRawSampleFormats; i++)
        this->m_CmbRawSampleFormat.InsertString(i, szRawSampleFormats[i]);
}

void CEncWAVtoAC3Dlg::InitSettingsListGroups()
{
    LVGROUP lg;
    lg.cbSize = sizeof(LVGROUP);
    lg.mask = LVGF_HEADER | LVGF_GROUPID;

    HWND listView = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();

    // remove all settings groups
    ListView_RemoveAllGroups(listView);

    // add Groups to settings listctrl
    for (int i = 0; i < nNumEncoderOptionsGroups; i++)
    {
        lg.pszHeader = (LPTSTR)(LPCTSTR)(HaveLangStrings() ? GetLangString(0x00208000 + i + 1) : pszGroups[i]);
        lg.iGroupId = 101 + i;
        ListView_InsertGroup(listView, -1, &lg);
    }
}

void CEncWAVtoAC3Dlg::InitDialogControls()
{
    // set text style to normal or bold for static controls and buttons
    m_StcQualityBitrate.SetBold(false);
    m_StcBitrate.SetBold(true);
    m_StcSelected.SetBold(false);
    m_StcPreconfigured.SetBold(false);
    m_BtnEncode.SetBold(true);

    // create status bar control
    VERIFY(m_StatusBar.Create(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
        CRect(0, 0, 0, 0),
        this,
        IDC_STATUSBAR));

    // status bar has only one pane
    // int nStatusBarParts[3] = { 150, 150, -1 };
    // m_StatusBar.SetParts(3, nStatusBarParts);

    // set range for work threads spinner
    m_SpnThreads.SetRange32(0, INT_MAX);
    m_SpnThreads.SetPos(0);

    // setup file listctrl
    // use LVS_SHAREIMAGELISTS to prevent 
    // deletion of image list under win9x
    // set this style under resource editor
    this->m_LstFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    this->m_LstFiles.InsertColumn(0, _T("File path"), LVCFMT_LEFT, 624, 0);
    this->m_LstFiles.InsertColumn(1, _T("File size (bytes)"), LVCFMT_LEFT, 140, 0);

    // initialize the image list for file listctrl
    SHFILEINFO sfi;
    HIMAGELIST m_ilLargeTmp;
    HIMAGELIST m_ilSmallTmp;
    TCHAR szSystemRoot[MAX_PATH + 1];

    GetWindowsDirectory(szSystemRoot, MAX_PATH);
    PathStripToRoot(szSystemRoot);

    m_ilLargeTmp = (HIMAGELIST)SHGetFileInfo(szSystemRoot,
        0,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON | SHGFI_ICON);

    m_ilSmallTmp = (HIMAGELIST)SHGetFileInfo(szSystemRoot,
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
    lc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lc.iSubItem = 0;
    lc.cx = 265;
    lc.pszText = _T("Option");
    ListView_InsertColumn(listView, 0, &lc);

    lc.iSubItem = 1;
    lc.cx = 210;
    lc.pszText = _T("Value");
    ListView_InsertColumn(listView, 1, &lc);

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
    if (this->bMultipleMonoInput == true)
        this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
    else
        this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
}

BOOL CEncWAVtoAC3Dlg::OnInitDialog()
{
    CResizeDialog::OnInitDialog();

    // set dialog icons
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // load accelerators for main dialog
    m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_MAIN_MENU));

    // set window title
    this->InitTitle();

    // init dialog controls
    this->InitDialogControls();

    // initialize language strings
    this->InitLang(true);

    // init encoder default preset
    this->InitDefaultPreset();

    // update bitrate text field
    this->UpdateBitrateText();

    // enable files/dirs drag & drop for dialog
    this->DragAcceptFiles(TRUE);

    // add resize anchors for main dialog
    this->InitDialogAnchors();

    // load all program configuration and settings
    this->LoadAllConfiguration();

    // process read-only setting
    if (this->cmdLineOpt.bHaveSaveConfig == true)
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
    if ((this->cmdLineOpt.bHaveOutputFile == true) && (this->cmdLineOpt.bHaveOutputPath == false))
    {
        this->szOutputFile = this->cmdLineOpt.szOutputFile;
        this->m_EdtOutPath.SetWindowText(this->szOutputFile);
    }

    // set default output path
    if ((this->cmdLineOpt.bHaveOutputFile == false) && (this->cmdLineOpt.bHaveOutputPath == true))
    {
        this->szOutputPath = this->cmdLineOpt.szOutputPath;
        this->m_EdtOutPath.SetWindowText(this->szOutputPath);
    }

    // encode input files and close program
    if (this->cmdLineOpt.bEncodeAndExit == true)
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
