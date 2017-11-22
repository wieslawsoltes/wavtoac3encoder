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

#include "StdAfx.h"
#include "MainApp.h"
#include "MainDlg.h"
#include "WorkDlg.h"
#include "MuxDlg.h"
#include "EnginesDlg.h"
#include "AboutDlg.h"
#include "..\utilities\Utilities.h"
#include "..\utilities\MyFile.h"
#include "..\utilities\TimeCount.h"
#include "EncWorkThread.h"

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
    : CMyResizeDialog(CMainDlg::IDD, pParent)
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

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CMyResizeDialog::DoDataExchange(pDX);
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

BEGIN_MESSAGE_MAP(CMainDlg, CMyResizeDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_WINDOWPOSCHANGING()
    //}}AFX_MSG_MAP
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_DROPFILES()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_COMMAND(ID_FILE_ADDFILES, &CMainDlg::OnFileAddFiles)
    ON_COMMAND(ID_FILE_ADDDIRECTORY, &CMainDlg::OnFileAddDirectory)
    ON_COMMAND(ID_FILE_MUXWIZARD, &CMainDlg::OnFileMuxWizard)
    ON_COMMAND(ID_FILE_LOADPRESETS, &CMainDlg::OnFileLoadPresets)
    ON_COMMAND(ID_FILE_SAVEPRESETS, &CMainDlg::OnFileSavePresets)
    ON_COMMAND(ID_FILE_LOADFILESLIST, &CMainDlg::OnFileLoadFilesList)
    ON_COMMAND(ID_FILE_SAVEFILESLIST, &CMainDlg::OnFileSaveFilesList)
    ON_COMMAND(ID_FILE_EXIT, &CMainDlg::OnFileExit)
    ON_COMMAND(ID_OPTIONS_DISABLEALLWARNINGS, &CMainDlg::OnOptionsDisableAllWarnings)
    ON_COMMAND(ID_OPTIONS_SAVECONFIGURATIONONEXIT, &CMainDlg::OnOptionsSaveConfigurationOnExit)
    ON_COMMAND(ID_OPTIONS_SAVECONFIGURATION, &CMainDlg::OnOptionsSaveConfiguration)
    ON_COMMAND(ID_OPTIONS_LOADCONFIGURATION, &CMainDlg::OnOptionsLoadConfiguration)
    ON_COMMAND(ID_LANGUAGE_DEFAULT, &CMainDlg::OnLanguageChangeDefault)
    ON_COMMAND_RANGE(ID_LANGUAGE_MENU_START, ID_LANGUAGE_MENU_MAX, &CMainDlg::OnLanguageChange)
    ON_COMMAND(ID_HELP_COMMAND_LINE, &CMainDlg::OnHelpCommandLine)
    ON_COMMAND(ID_HELP_WEBSITE, &CMainDlg::OnHelpWebsite)
    ON_COMMAND(ID_HELP_ABOUT, &CMainDlg::OnHelpAbout)
    ON_COMMAND(ID_LIST_ADDFILES, &CMainDlg::OnListAddFiles)
    ON_COMMAND(ID_LIST_ADDDIRECTORY, &CMainDlg::OnListAddDirectory)
    ON_COMMAND(ID_LIST_MUXWIZARD, &CMainDlg::OnListMuxWizard)
    ON_COMMAND(ID_LIST_LOADLIST, &CMainDlg::OnListLoadList)
    ON_COMMAND(ID_LIST_SAVELIST, &CMainDlg::OnListSavelist)
    ON_COMMAND(ID_LIST_DELFILES, &CMainDlg::OnListDelFiles)
    ON_COMMAND(ID_LIST_CLEARLIST, &CMainDlg::OnListClearList)
    ON_COMMAND(ID_LIST_MOVEUP, &CMainDlg::OnListMoveUp)
    ON_COMMAND(ID_LIST_MOVEDOWN, &CMainDlg::OnListMoveDown)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &CMainDlg::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_ENCODE, &CMainDlg::OnBnClickedButtonEncode)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CMainDlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_PRESETS_DEFAULTS, &CMainDlg::OnBnClickedButtonPresetsDefaults)
    ON_BN_CLICKED(IDC_BUTTON_PRESET_ADD, &CMainDlg::OnBnClickedButtonPresetAdd)
    ON_BN_CLICKED(IDC_BUTTON_PRESET_DEL, &CMainDlg::OnBnClickedButtonPresetDel)
    ON_BN_CLICKED(IDC_CHECK_VBR, &CMainDlg::OnBnClickedCheckVbr)
    ON_BN_CLICKED(IDC_CHECK_SIMD_MMX, &CMainDlg::OnBnClickedCheckSimdMmx)
    ON_BN_CLICKED(IDC_CHECK_SIMD_SSE, &CMainDlg::OnBnClickedCheckSimdSse)
    ON_BN_CLICKED(IDC_CHECK_SIMD_SSE2, &CMainDlg::OnBnClickedCheckSimdSse2)
    ON_BN_CLICKED(IDC_CHECK_SIMD_SSE3, &CMainDlg::OnBnClickedCheckSimdSse3)
    ON_BN_CLICKED(IDC_CHECK_MULTIPLE_MONO_INPUT, &CMainDlg::OnBnClickedCheckMultipleMonoInput)
    ON_BN_CLICKED(IDC_BUTTON_MUX_WIZARD, &CMainDlg::OnBnClickedButtonMuxWizard)
    ON_CBN_SELCHANGE(IDC_COMBO_SETTING, &CMainDlg::OnCbnSelchangeComboSetting)
    ON_CBN_SELCHANGE(IDC_COMBO_PRESETS, &CMainDlg::OnCbnSelchangeComboPresets)
    ON_CBN_SELCHANGE(IDC_COMBO_ENGINES, &CMainDlg::OnCbnSelchangeComboEngines)
    ON_CBN_SELCHANGE(IDC_COMBO_RAW_SAMPLE_FORMAT, &CMainDlg::OnCbnSelchangeComboRawSampleFormat)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SETTINGS, &CMainDlg::OnLvnItemchangedListSettings)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FILES, &CMainDlg::OnLvnKeydownListFiles)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SETTINGS, &CMainDlg::OnLvnKeydownListSettings)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_FILES, &CMainDlg::OnLvnColumnclickListFiles)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_FILES, &CMainDlg::OnNMRclickListFiles)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_SETTINGS, &CMainDlg::OnNMRclickListSettings)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_SETTINGS, &CMainDlg::OnNMDblclkListSettings)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILES, &CMainDlg::OnNMDblclkListFiles)
    ON_EN_CHANGE(IDC_EDIT_THREADS, &CMainDlg::OnEnChangeEditThreads)
    ON_EN_CHANGE(IDC_EDIT_RAW_SAMPLE_RATE, &CMainDlg::OnEnChangeEditRawSampleRate)
    ON_EN_CHANGE(IDC_EDIT_RAW_CHANNELS, &CMainDlg::OnEnChangeEditRawChannels)
    ON_EN_CHANGE(IDC_EDIT_OUTPUT_PATH, &CMainDlg::OnEnChangeEditOutputPath)
    ON_EN_SETFOCUS(IDC_EDIT_OUTPUT_PATH, &CMainDlg::OnEnSetfocusEditOutputPath)
    ON_EN_SETFOCUS(IDC_EDIT_RAW_SAMPLE_RATE, &CMainDlg::OnEnSetfocusEditRawSampleRate)
    ON_EN_SETFOCUS(IDC_EDIT_RAW_CHANNELS, &CMainDlg::OnEnSetfocusEditRawChannels)
    ON_EN_SETFOCUS(IDC_EDIT_THREADS, &CMainDlg::OnEnSetfocusEditThreads)
    ON_EN_KILLFOCUS(IDC_EDIT_OUTPUT_PATH, &CMainDlg::OnEnKillfocusEditOutputPath)
    ON_EN_KILLFOCUS(IDC_EDIT_RAW_SAMPLE_RATE, &CMainDlg::OnEnKillfocusEditRawSampleRate)
    ON_EN_KILLFOCUS(IDC_EDIT_RAW_CHANNELS, &CMainDlg::OnEnKillfocusEditRawChannels)
    ON_EN_KILLFOCUS(IDC_EDIT_THREADS, &CMainDlg::OnEnKillfocusEditThreads)
    ON_MESSAGE(WM_MY_EN_CHANGE, EditChangeComboPresets)
    ON_BN_CLICKED(IDC_BUTTON_ENGINES, &CMainDlg::OnBnClickedButtonEngines)
END_MESSAGE_MAP()

void CMainDlg::OnPaint()
{
    if (IsIconic())
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
        CMyResizeDialog::OnPaint();
    }
}

HCURSOR CMainDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMainDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    // by default hide main window
    if (this->bVisible == false)
        lpwndpos->flags &= ~SWP_SHOWWINDOW;

    CMyResizeDialog::OnWindowPosChanging(lpwndpos);
}

void CMainDlg::UpdateView(int nMode)
{
    // reset dialog anchors
    this->InitDialogAnchors();
}

void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    this->UpdateBitrateText();

    CMyResizeDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMainDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_THREADS)
    {
        // update threads number
        CString szBuff;

        if (nPos == 0)
            szBuff = DEFAULT_TEXT_AUTO;
        else
            szBuff.Format(_T("%d"), nPos);

        this->m_EdtThreads.SetWindowText(szBuff);
    }
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_SAMPLE_RATE)
    {
        // update sample rate number
        CString szBuff;

        if (nPos == 0)
            szBuff = DEFAULT_TEXT_IGNORED;
        else
            szBuff.Format(_T("%d"), nPos);

        this->m_EdtRawSamplerate.SetWindowText(szBuff);
    }
    if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_CHANNELS)
    {
        // update channels number
        CString szBuff;

        if (nPos == 0)
            szBuff = DEFAULT_TEXT_IGNORED;
        else
            szBuff.Format(_T("%d"), nPos);

        this->m_EdtRawChannels.SetWindowText(szBuff);
    }
    else
    {
        CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
    }

    CMyResizeDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMainDlg::OnClose()
{
    // save configuration to disk (does not work in read-only mode)
    if (this->bSaveConfig == true)
        this->SaveAllConfiguration();

    CMyResizeDialog::OnClose();
}

void CMainDlg::OnDestroy()
{
    CMyResizeDialog::OnDestroy();

    // clean-up memory used by tooltips
    this->m_LstFiles.DeleteAllItems();
    this->m_LstSettings.DeleteAllItems();
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    if (m_hAccelTable)
    {
        if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
            return(TRUE);
    }

    return CMyResizeDialog::PreTranslateMessage(pMsg);
}

void CMainDlg::OnBnClickedButtonAdd()
{
    this->OnFileAddFiles();
}

void CMainDlg::OnBnClickedButtonEncode()
{
    static bool bWorking = false;
    if (bWorking == true)
        return;

    // get number of files to process
    int nItemsCount = this->m_LstFiles.GetItemCount();

    // do nothing if there are no files in list
    if (nItemsCount <= 0)
    {
        ::LogMessage(_T("Error: Add at least one file to the file list!"));

        MessageBox(HaveLangStrings() ? GetLangString(0x00207011) : _T("Add at least one file to the file list!"),
            HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
            MB_ICONERROR | MB_OK);

        return;
    }

    // check if correct number of mono input files is present
    if ((this->bMultipleMonoInput == true) && (nItemsCount < 1 || nItemsCount > 6))
    {
        ::LogMessage(_T("Error: Supported are minimum 1 and maximum 6 mono input files!"));

        MessageBox(HaveLangStrings() ? GetLangString(0x00207012) : _T("Supported are minimum 1 and maximum 6 mono input files!"),
            HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
            MB_ICONERROR | MB_OK);

        return;
    }

    bWorking = true;

    if (OpenAftenAPI(&this->api) == false)
    {
        ::LogMessage(_T("Error: Failed to load libaften.dll dynamic library!"));

        MessageBox(HaveLangStrings() ? GetLangString(0x00207013) : _T("Failed to load libaften.dll dynamic library!"),
            HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
            MB_ICONERROR | MB_OK);

        bWorking = false;
        return;
    }

    CWorkDlg dlg;

    // get all files from list
    CList<CString, CString> list;
    CList<bool, bool> listStatus;
    dlg.nTotalSize = 0;
    CString szSizeBuff;
    CString szFileBuffer;
    bool bAvisynthInput = false;

    for (int i = 0; i < nItemsCount; i++)
    {
        // check for Avisynth scripts
        szFileBuffer = this->m_LstFiles.GetItemText(i, 0);

#ifndef DISABLE_AVISYNTH
        if (GetFileExt(szFileBuffer).MakeLower() == _T("avs"))
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
    if ((this->bMultipleMonoInput == true) && (bAvisynthInput == true))
    {
        ::LogMessage(_T("Error: Disable 'Multiple mono input' mode in order to use Avisynth scripts!"));

        MessageBox(HaveLangStrings() ? GetLangString(0x00207014) : _T("Disable 'Multiple mono input' mode in order to use Avisynth scripts!"),
            HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
            MB_ICONERROR | MB_OK);

        bWorking = false;
        return;
    }
#endif

    // set preset
    dlg.workParam.preset = this->GetCurrentPreset();

    // set pointer to files list
    dlg.workParam.list = &list;

    // set pointer to status list
    dlg.workParam.listStatus = &listStatus;

    // get output file/path
    this->m_EdtOutPath.GetWindowText(dlg.workParam.szOutPath);
    dlg.workParam.bUseOutPath = false;

    int nLen = dlg.workParam.szOutPath.GetLength();
    if (nLen < 3)
    {
        ::LogMessage(_T("Error: Invalid output path!"));

        this->MessageBox(HaveLangStrings() ? GetLangString(0x00207015) : _T("Invalid output path!"),
            HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
            MB_OK | MB_ICONERROR);

        bWorking = false;
        return;
    }

    CString szExt = dlg.workParam.szOutPath.Right(4);
    if (this->bMultipleMonoInput == true)
    {
        if (dlg.workParam.szOutPath.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0)
        {
            if ((nLen < 4) || (szExt.CompareNoCase(_T(".ac3")) != 0))
            {
                ::LogMessage(_T("Error: Invalid output file!"));

                this->MessageBox(HaveLangStrings() ? GetLangString(0x00207016) : _T("Invalid output file!"),
                    HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
                    MB_OK | MB_ICONERROR);

                bWorking = false;
                return;
            }
        }
    }

    if ((dlg.workParam.szOutPath.Compare(_T("")) != 0) &&
        ((dlg.workParam.szOutPath.Compare(DEFAULT_TEXT_OUTPUT_PATH) != 0 && this->bMultipleMonoInput == false) ||
        (dlg.workParam.szOutPath.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0 && this->bMultipleMonoInput == true)))
    {
        if (this->bMultipleMonoInput == false)
        {
            if (MakeFullPath(dlg.workParam.szOutPath) == false)
            {
                // show error message
                ::LogMessage(_T("Error: Failed to create output path!"));

                this->MessageBox(HaveLangStrings() ? GetLangString(0x00207017) : _T("Failed to create output path!"),
                    HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
                    MB_OK | MB_ICONERROR);

                bWorking = false;
                return;
            }
        }
        else
        {
            CString szTmpOutPath = dlg.workParam.szOutPath;
            CString szFile = GetFileName(dlg.workParam.szOutPath);

            szTmpOutPath.Truncate(szTmpOutPath.GetLength() - szFile.GetLength());
            if (MakeFullPath(szTmpOutPath) == false)
            {
                // show error message
                ::LogMessage(_T("Error: Failed to create output path!"));

                this->MessageBox(HaveLangStrings() ? GetLangString(0x00207017) : _T("Failed to create output path!"),
                    HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
                    MB_OK | MB_ICONERROR);

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
    CTimeCount countTime;
    CString szText;

    // show work dialog
    countTime.Start();
    dlg.DoModal();
    countTime.Stop();

    // remove encoded files from list
    for (INT_PTR i = listStatus.GetSize() - 1; i >= 0; i--)
    {
        if (listStatus.GetAt(listStatus.FindIndex(i)) == true)
            this->m_LstFiles.DeleteItem(i);
    }

    // show total work time
    if (dlg.nCount <= 0)
    {
        szText.Format(_T(""));
        ::LogMessage(_T("Error: Failed to encode all files."));
    }
    else
    {
        if (this->bMultipleMonoInput == true)
        {
            szText.Format(HaveLangStrings() ? GetLangString(0x00207018) : _T("Encoded %d mono files in %s (%0.3lf sec)"),
                dlg.nCount,
				countTime.Format(countTime.ElapsedTime(), 3),
				countTime.ElapsedTime());

            ::LogMessage(szText);
        }
        else
        {
            szText.Format(HaveLangStrings() ? GetLangString(0x00207019) : _T("Encoded %d file%s in %s (%0.3lf sec)"),
                dlg.nCount,
                dlg.nCount == 1 ? _T("") :
                (HaveLangStrings() ? GetLangString(0x0020701A) : _T("s")),
				countTime.Format(countTime.ElapsedTime(), 3),
				countTime.ElapsedTime());

            ::LogMessage(szText);
        }
    }

    this->m_StatusBar.SetText(szText, 0, 0);
    bWorking = false;
}

void CMainDlg::OnBnClickedCheckSimdMmx()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[0] = this->m_ChkSimdMMX.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CMainDlg::OnBnClickedCheckSimdSse()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[1] = this->m_ChkSimdSSE.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CMainDlg::OnBnClickedCheckSimdSse2()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[2] = this->m_ChkSimdSSE2.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CMainDlg::OnBnClickedCheckSimdSse3()
{
    EncoderPreset tmpPreset = GetCurrentPreset();
    tmpPreset.nUsedSIMD[3] = this->m_ChkSimdSSE3.GetCheck() == BST_CHECKED ? 1 : 0;
    UpdateCurrentPreset(tmpPreset);
}

void CMainDlg::OnBnClickedCheckVbr()
{
    if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
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

void CMainDlg::OnBnClickedButtonPresetAdd()
{
    EncoderPreset newPreset = GetCurrentPreset();

    static int nCount = 0;

    newPreset.szName.Format(_T("%s (%d)"),
        HaveLangStrings() ? GetLangString(0x0020701B) : _T("New preset"),
        nCount++);
    this->encPresets.AddTail(newPreset);

    this->nCurrentPreset = this->encPresets.GetCount() - 1;
    this->m_CmbPresets.InsertString(this->nCurrentPreset, newPreset.szName);
    this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

    SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);
}

void CMainDlg::OnBnClickedButtonPresetDel()
{
    // we need at least one preset present
    if (this->encPresets.GetCount() >= 2)
    {
        int nCount = this->m_CmbPresets.GetCount();
        int nPreset = this->m_CmbPresets.GetCurSel();

        this->encPresets.RemoveAt(this->encPresets.FindIndex(nPreset));
        this->m_CmbPresets.DeleteString(nPreset);

        this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

        if (nPreset == (nCount - 1)) // last preset
        {
            this->m_CmbPresets.SetCurSel(nCount - 2);
            this->nCurrentPreset = nCount - 2;
        }
        else if (nPreset == 0) // first preset
        {
            this->m_CmbPresets.SetCurSel(0);
            this->nCurrentPreset = 0;
        }
        else // in the middle
        {
            this->m_CmbPresets.SetCurSel(nPreset);
            this->nCurrentPreset = nPreset;
        }

        SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);

        // update all controls
        this->OnCbnSelchangeComboPresets();
    }
}

void CMainDlg::OnBnClickedButtonBrowse()
{
    // mono input uses output dir to set output file name
    // if file name is not present then 'mux.ac3' is used
    if (this->bMultipleMonoInput == true)
    {
        // configure save file dialog
        CFileDialog fd(FALSE,
            szSupportedOutputExt[0],
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER,
            HaveLangStrings() ? GetLangString(0x0020701C) : _T("AC3 Files (*.ac3)|*.ac3|All Files (*.*)|*.*||"));

        // show file dialog
        if (fd.DoModal() == IDOK)
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

        if (SHGetMalloc(&pMalloc) == E_FAIL)
            return;

        if ((lpBuffer = (TCHAR *)pMalloc->Alloc(MAX_PATH * 2)) == NULL)
        {
            pMalloc->Release();
            return;
        }

        if (!SUCCEEDED(SHGetSpecialFolderLocation(::GetDesktopWindow(), CSIDL_DESKTOP, &pidlDesktop)))
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
        bi.lpszTitle = HaveLangStrings() ? GetLangString(0x0020701D) : _T("Select default output path:");
        bi.lpfn = NULL;
        bi.lParam = 0;
        bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
        bi.iImage = 0;

        pidlBrowse = ::SHBrowseForFolder(&bi);
        if (pidlBrowse != NULL)
        {
            if (SHGetPathFromIDList(pidlBrowse, lpBuffer))
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

void CMainDlg::OnBnClickedButtonPresetsDefaults()
{
    int nPreset = this->m_CmbPresets.GetCurSel();
    if (nPreset != CB_ERR)
    {
        this->nCurrentPreset = nPreset;

        // load default preset and set all settings
        EncoderPreset tmpPreset = defaultPreset;
        tmpPreset.szName = GetCurrentPreset().szName;
        UpdateCurrentPreset(tmpPreset);

        // apply current preset to main dialog
        this->ApplyPresetToDlg(tmpPreset);
    }
}

void CMainDlg::OnBnClickedCheckMultipleMonoInput()
{
    this->bMultipleMonoInput = this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED ? true : false;

    if (this->bMultipleMonoInput == true)
        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(HaveLangStrings() ? GetLangString(0x0020200B) : _T("Output file:"));
    else
        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(HaveLangStrings() ? GetLangString(0x0020200C) : _T("Output path:"));

    CString szBuff = this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath;

    if (szBuff.Compare(_T("")) == 0 ||
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

void CMainDlg::OnBnClickedButtonMuxWizard()
{
    this->OnFileMuxWizard();
}

void CMainDlg::OnBnClickedButtonEngines()
{
    CEnginesDlg dlg;

    dlg.nCurrSel = this->m_CmbEngines.GetCurSel();

    // copy engines list to engines editor dialog
    int nSize = this->m_EngineList.GetSize();
    POSITION pos = this->m_EngineList.GetHeadPosition();
    for (INT_PTR i = 0; i < nSize; i++)
    {
        ConfigEntry ce;

        // get next entry in configuration list
        ce = this->m_EngineList.GetNext(pos);

        // insert all items to new engines list
        dlg.m_EngineList.AddTail(ce);
    }

    // show engines editor dialog box
    if (dlg.DoModal() == IDOK)
    {
        // init engines configuration
        this->m_EngineList.RemoveAll();
        this->m_CmbEngines.ResetContent();

        // copy new engines from editor dialog to list
        int nSize = dlg.m_EngineList.GetSize();
        POSITION pos = dlg.m_EngineList.GetHeadPosition();
        for (INT_PTR i = 0; i < nSize; i++)
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

void CMainDlg::OnCbnSelchangeComboSetting()
{
	POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
	if (pos != NULL)
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

void CMainDlg::OnCbnSelchangeComboPresets()
{
	int nPreset = this->m_CmbPresets.GetCurSel();
	if (nPreset != CB_ERR)
	{
		this->nCurrentPreset = nPreset;

		// load selected preset and set all settings
		EncoderPreset tmpPreset = GetCurrentPreset();

		// apply current preset to main dialog
		this->ApplyPresetToDlg(tmpPreset);
	}
}

void CMainDlg::OnCbnSelchangeComboEngines()
{
	// unload currently used aften library
	CloseAftenAPI(&this->api);

	// get newly selected engine by user
	int nSel = this->m_CmbEngines.GetCurSel();
	if (nSel == CB_ERR)
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
	if (OpenAftenAPI(&this->api) == false)
	{
		// show error message in status bar
		CString szLogMessage =
			(HaveLangStrings() ? GetLangString(0x0020701E) : _T("Failed to load")) +
			_T(" '") +
			m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szKey +
			_T("' ") +
			(HaveLangStrings() ? GetLangString(0x0020701F) : _T("library")) +
			_T("!");
		this->m_StatusBar.SetText(szLogMessage, 0, 0);

		// this->nCurrentEngine = -1;
		// MessageBox(_T("Unable to load Aften API!"), _T("Error"), MB_ICONERROR | MB_OK);
		return;
	}
	else
	{
		// get Aften library version (ANSI string)
		const char *szAftenVersionAnsi = this->api.LibAften_aften_get_version();
		int nVersionLen = strlen(szAftenVersionAnsi);

#ifdef _UNICODE
		TCHAR szAftenVersion[256] = _T("");
		ZeroMemory(szAftenVersion, 256 * sizeof(TCHAR));
		int nChars = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED,
			szAftenVersionAnsi, nVersionLen,
			szAftenVersion, 256);
		if (nChars == 0)
			_stprintf(szAftenVersion, _T("?.??"));
#else
		const char *szAftenVersion = szAftenVersionAnsi;
#endif // _UNICODE

		// show Aften build and version info in status bar
		CString szLogMessage =
			(HaveLangStrings() ? GetLangString(0x00207020) : _T("Loaded")) +
			_T(" '") +
			m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szKey +
			_T("' ") +
			(HaveLangStrings() ? GetLangString(0x0020701F) : _T("library")) +
			_T(", ") +
			(HaveLangStrings() ? GetLangString(0x00207021) : _T("version")) +
			_T(" ") +
			szAftenVersion;
		this->m_StatusBar.SetText(szLogMessage, 0, 0);
	}
}

void CMainDlg::OnCbnSelchangeComboRawSampleFormat()
{
	EncoderPreset tmpPreset = GetCurrentPreset();
	tmpPreset.nRawSampleFormat = this->m_CmbRawSampleFormat.GetCurSel();
	UpdateCurrentPreset(tmpPreset);
}

LRESULT CMainDlg::EditChangeComboPresets(WPARAM wParam, LPARAM lParam)
{
	if ((lParam == (LPARAM)0) && (wParam != NULL))
	{
		int nPreset = this->m_CmbPresets.GetCurSel();
		if (nPreset != CB_ERR)
		{
			// there is always one preset present (Default)
			CString *szName = (CString *)wParam;

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
		}
	}

	return(0);
}

bool CMainDlg::LoadProgramConfig(CString szFileName)
{
	ConfigList_t m_ConfigList;

	// init program configuration
	if (::LoadConfig(szFileName, m_ConfigList) == true)
	{
		int nSize = m_ConfigList.GetSize();
		POSITION pos = m_ConfigList.GetHeadPosition();
		for (INT_PTR i = 0; i < nSize; i++)
		{
			ConfigEntry ce;

			// get next entry in configuration list
			ce = m_ConfigList.GetNext(pos);

			// key: MainWindow
			if (ce.szKey.Compare(_T("MainWindow")) == 0)
			{
				this->SetWindowRectStr(ce.szData);
			}

			// key: ColumnSizeSettings
			else if (ce.szKey.Compare(_T("ColumnSizeSettings")) == 0)
			{
				int nColumn[2] = { 0, 0 };
				if (_stscanf(ce.szData, _T("%d %d"),
					&nColumn[0], &nColumn[1]) == 2)
				{
					this->m_LstSettings.SetColumnWidth(0, nColumn[0]);
					this->m_LstSettings.SetColumnWidth(1, nColumn[1]);
				}
			}

			// key: ColumnSizeFiles
			else if (ce.szKey.Compare(_T("ColumnSizeFiles")) == 0)
			{
				int nColumn[2] = { 0, 0 };
				if (_stscanf(ce.szData, _T("%d %d"),
					&nColumn[0], &nColumn[1]) == 2)
				{
					this->m_LstFiles.SetColumnWidth(0, nColumn[0]);
					this->m_LstFiles.SetColumnWidth(1, nColumn[1]);
				}
			}

			// key: OutputPath
			else if (ce.szKey.Compare(_T("OutputPath")) == 0)
			{
				if (ce.szData.Compare(_T("")) != 0 && ce.szData.Compare(DEFAULT_TEXT_OUTPUT_PATH) != 0)
				{
					this->szOutputPath = ce.szData;
				}
			}

			// key: OutputFile
			else if (ce.szKey.Compare(_T("OutputFile")) == 0)
			{
				if (ce.szData.Compare(_T("")) != 0 && ce.szData.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0)
				{
					this->szOutputFile = ce.szData;
				}
			}

			// key: SelectedPreset
			else if (ce.szKey.Compare(_T("SelectedPreset")) == 0)
			{
				int nPreset = 0;
				if (_stscanf(ce.szData, _T("%d"),
					&nPreset) == 1)
				{
					// reset preset selection if the current preset value is invalid
					if ((nPreset >= this->m_CmbPresets.GetCount()) || (nPreset < 0))
						nPreset = 0;

					if (this->nCurrentPreset != nPreset)
					{
						this->nCurrentPreset = nPreset;
						this->m_CmbPresets.SetCurSel(nPreset);
						this->OnCbnSelchangeComboPresets();
					}
				}
			}

			// key: MultipleMonoInput
			else if (ce.szKey.Compare(_T("MultipleMonoInput")) == 0)
			{
				if (ce.szData.Compare(_T("true")) == 0)
				{
					this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
					this->bMultipleMonoInput = true;
					this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(HaveLangStrings() ? GetLangString(0x0020200B) : _T("Output file:"));
				}
				else if (ce.szData.Compare(_T("false")) == 0)
				{
					this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
					this->bMultipleMonoInput = false;
					this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(HaveLangStrings() ? GetLangString(0x0020200C) : _T("Output path:"));
				}
				else
				{
					this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
					this->bMultipleMonoInput = false;
					this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(HaveLangStrings() ? GetLangString(0x0020200C) : _T("Output path:"));
				}
			}

			// key: DisableAllWarnings
			else if (ce.szKey.Compare(_T("DisableAllWarnings")) == 0)
			{
				if (ce.szData.Compare(_T("true")) == 0)
				{
					this->bDisableAllWarnings = true;
					this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_CHECKED);
				}
				else if (ce.szData.Compare(_T("false")) == 0)
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
			else if (ce.szKey.Compare(_T("SaveConfig")) == 0)
			{
				if (ce.szData.Compare(_T("true")) == 0)
				{
					this->bSaveConfig = true;
					this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_CHECKED);
				}
				else if (ce.szData.Compare(_T("false")) == 0)
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
		}

		// set output file/path
		if (this->bMultipleMonoInput == true)
		{
			if (this->szOutputFile.Compare(_T("")) == 0)
				this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
			else
				this->m_EdtOutPath.SetWindowText(this->szOutputFile);
		}
		else
		{
			if (this->szOutputPath.Compare(_T("")) == 0)
				this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
			else
				this->m_EdtOutPath.SetWindowText(this->szOutputPath);
		}

		return true;
	}

	return false;
}

bool CMainDlg::SaveProgramConfig(CString szFileName)
{
	ConfigList_t m_ConfigList;

	// prepare program configuration
	m_ConfigList.RemoveAll();

	ConfigEntry ce;

	// key: MainWindow
	ce.szKey = _T("MainWindow");
	ce.szData = this->GetWindowRectStr();
	m_ConfigList.AddTail(ce);

	// key: ColumnSizeSettings
	ce.szKey = _T("ColumnSizeSettings");
	ce.szData.Format(_T("%d %d"),
		this->m_LstSettings.GetColumnWidth(0),
		this->m_LstSettings.GetColumnWidth(1));
	m_ConfigList.AddTail(ce);

	// key: ColumnSizeFiles
	ce.szKey = _T("ColumnSizeFiles");
	ce.szData.Format(_T("%d %d"),
		this->m_LstFiles.GetColumnWidth(0),
		this->m_LstFiles.GetColumnWidth(1));
	m_ConfigList.AddTail(ce);

	// key: OutputPath
	ce.szKey = _T("OutputPath");
	ce.szData = this->szOutputPath;
	if (ce.szData.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0)
		ce.szData = _T("");
	m_ConfigList.AddTail(ce);

	// key: OutputFile
	ce.szKey = _T("OutputFile");
	ce.szData = this->szOutputFile;
	if (ce.szData.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
		ce.szData = _T("");
	m_ConfigList.AddTail(ce);

	// key: SelectedPreset
	ce.szKey = _T("SelectedPreset");
	ce.szData.Format(_T("%d"),
		this->m_CmbPresets.GetCurSel());
	m_ConfigList.AddTail(ce);

	// key: MultipleMonoInput
	ce.szKey = _T("MultipleMonoInput");
	ce.szData = (this->bMultipleMonoInput == true) ? _T("true") : _T("false");
	m_ConfigList.AddTail(ce);

	// key: DisableAllWarnings
	ce.szKey = _T("DisableAllWarnings");
	ce.szData = (this->bDisableAllWarnings == true) ? _T("true") : _T("false");
	m_ConfigList.AddTail(ce);

	// key: SaveConfig
	ce.szKey = _T("SaveConfig");
	ce.szData = (this->bSaveConfig == true) ? _T("true") : _T("false");
	m_ConfigList.AddTail(ce);

	// save program configuration
	return ::SaveConfig(szFileName, m_ConfigList);
}

bool CMainDlg::UpdateProgramEngines()
{
	int nSize = this->m_EngineList.GetSize();

	// if there is no engine then return error
	if (nSize == 0)
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
	for (INT_PTR i = 0; i < nSize; i++)
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
	if (GetCurrentPreset().nCurrentEngine > nSize)
	{
		EncoderPreset tmpPreset = GetCurrentPreset();
		tmpPreset.nCurrentEngine = 0;
		UpdateCurrentPreset(tmpPreset);
	}

	// load and select current engine
	if ((GetCurrentPreset().nCurrentEngine >= 0) && (GetCurrentPreset().nCurrentEngine < nSize))
	{
		// load new aften library
		this->api.szLibPath = m_EngineList.GetAt(m_EngineList.FindIndex(GetCurrentPreset().nCurrentEngine)).szData;
		if (OpenAftenAPI(&this->api) == false)
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

bool CMainDlg::LoadProgramEngines(CString szFileName)
{
	// init engines configuration
	this->m_EngineList.RemoveAll();
	this->m_CmbEngines.ResetContent();

	if (::LoadConfig(szFileName, this->m_EngineList) == true)
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

bool CMainDlg::SaveProgramEngines(CString szFileName)
{
	// save engines configuration
	return ::SaveConfig(szFileName, this->m_EngineList);
}

bool CMainDlg::LoadFilesList(CString &szFileName)
{
	try
	{
		CMyFile fp;
		if (fp.FOpen(szFileName, false) == false)
			return false;

		ULONGLONG  nRead = 0, nLength = fp.FSize();
		if (nLength == 0)
		{
			fp.FClose();
			return true;
		}

		// clear list
		this->m_LstFiles.DeleteAllItems();

		TCHAR Buffer;
		CString szBuffer = _T("");

		while (fp.FRead(Buffer) == true)
		{
			if ((Buffer != '\r') && (Buffer != '\n'))
				szBuffer += Buffer;

			if (Buffer == '\n' || nRead == nLength - (fp.FMode() == 1 ? 1 : sizeof(TCHAR)))
			{
				szBuffer += _T("\0");

				if (szBuffer.GetLength() > 0)
				{
					// remove leading and trailing quotes (used for *.mux file format)
					szBuffer.TrimLeft('"');
					szBuffer.TrimRight('"');

					// // add only files with proper file extensions
					if (IsSupportedInputExt(GetFileExt(szBuffer)) == true)
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
	catch (...)
	{
		return false;
	}
}

bool CMainDlg::SaveFilesList(CString &szFileName, int nFormat)
{
	int nItems = this->m_LstFiles.GetItemCount();
	try
	{
		CMyFile fp;

		// write *.mux files always in Ansi format
		if (nFormat == 1)
		{
#ifdef _UNICODE
			fp.FSetMode(2);
#else
			fp.FSetMode(3);
#endif
		}

		if (fp.FOpen(szFileName, true) == false)
			return false;

		CString szBuffer;
		CString szTmpFileName;

		for (int i = 0; i < nItems; i++)
		{
			szTmpFileName = this->m_LstFiles.GetItemText(i, 0);
			szBuffer.Format(_T("%s%s%s\r\n"), nFormat == 0 ? _T("") : _T("\""), szTmpFileName, nFormat == 0 ? _T("") : _T("\""));
			fp.FWriteString(szBuffer.GetBuffer(), szBuffer.GetLength());
			szBuffer.ReleaseBuffer();
		}

		fp.FClose();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void CMainDlg::LoadAllConfiguration()
{
	bool bRet = false;

	// load presets from file
	if (this->cmdLineOpt.bHaveLoadPresets == true)
	{
		bRet = ::LoadEncoderPresets(this->encPresets, this->cmdLineOpt.szLoadPresets, this->defaultPreset);
		::LogMessage((bRet ? _T("Loaded encoder presets: ") : _T("Failed to load encoder presets: ")) + this->cmdLineOpt.szLoadPresets);
	}
	else
	{
		bRet = ::LoadEncoderPresets(this->encPresets, theApp.m_szPresetsFilePath, this->defaultPreset);
		::LogMessage((bRet ? _T("Loaded encoder presets: ") : _T("Failed to load encoder presets: ")) + theApp.m_szPresetsFilePath);
	}

	// process presets list
	if (bRet == true)
	{
		// get first preset from loaded list
		if (encPresets.GetSize() > 0)
		{
			// reset presets combobox
			this->m_CmbPresets.ResetContent();

			// add all preset names to preset combobox
			for (int i = 0; i < encPresets.GetCount(); i++)
			{
				EncoderPreset encTmp = encPresets.GetAt(encPresets.FindIndex(i));

				this->m_CmbPresets.InsertString(i, encTmp.szName);
			}

			// select current preset
			if ((this->nCurrentPreset >= encPresets.GetCount()) || (this->nCurrentPreset < 0))
				this->nCurrentPreset = 0;

			this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

			// update all controls
			this->OnCbnSelchangeComboPresets();
		}
	}

	// load program settings from file
	if (this->cmdLineOpt.bHaveLoadConfig == true)
	{
		bRet = this->LoadProgramConfig(this->cmdLineOpt.szLoadConfig);
		::LogMessage((bRet ? _T("Loaded program config: ") : _T("Failed to load program config: ")) + this->cmdLineOpt.szLoadConfig);
	}
	else
	{
		bRet = this->LoadProgramConfig(theApp.m_szConfigFilePath);
		::LogMessage((bRet ? _T("Loaded program config: ") : _T("Failed to load program config: ")) + theApp.m_szConfigFilePath);
	}

	// load engines settings from file
	if (this->cmdLineOpt.bHaveLoadEngines == true)
	{
		bRet = this->LoadProgramEngines(this->cmdLineOpt.szLoadEngines);
		::LogMessage((bRet ? _T("Loaded encoder engines: ") : _T("Failed to load encoder engines: ")) + this->cmdLineOpt.szLoadEngines);
	}
	else
	{
		bRet = this->LoadProgramEngines(theApp.m_szEnginesFilePath);
		::LogMessage((bRet ? _T("Loaded encoder engines: ") : _T("Failed to load encoder engines: ")) + theApp.m_szEnginesFilePath);
	}

	// add input file to the files list
	if (this->cmdLineOpt.bHaveInputFile == true)
	{
		this->OnListClearList();
		this->AddItemToFileList(this->cmdLineOpt.szInputFile);
	}
	else
	{
		// load files list from file
		if (this->cmdLineOpt.bHaveLoadFiles == true)
		{
			bRet = this->LoadFilesList(this->cmdLineOpt.szLoadFiles);
			::LogMessage((bRet ? _T("Loaded files list: ") : _T("Failed to load files list: ")) + this->cmdLineOpt.szLoadFiles);
		}
		else
		{
			bRet = this->LoadFilesList(theApp.m_szFilesListFilePath);
			::LogMessage((bRet ? _T("Loaded files list: ") : _T("Failed to load files list: ")) + theApp.m_szFilesListFilePath);
		}
	}
}

void CMainDlg::SaveAllConfiguration()
{
	bool bRet = false;

	// save encoder presets to a file
	bRet = ::SaveEncoderPresets(this->encPresets, theApp.m_szPresetsFilePath, this->defaultPreset);
	::LogMessage((bRet ? _T("Saved encoder presets: ") : _T("Error: Failed to save encoder presets: ")) + theApp.m_szPresetsFilePath);

	// save program configuration to a file
	bRet = this->SaveProgramConfig(theApp.m_szConfigFilePath);
	::LogMessage((bRet ? _T("Saved program config: ") : _T("Error: Failed to save program config: ")) + theApp.m_szConfigFilePath);

	// save engines settings to a file
	bRet = this->SaveProgramEngines(theApp.m_szEnginesFilePath);
	::LogMessage((bRet ? _T("Saved encoder engines: ") : _T("Error: Failed to save encoder engines: ")) + theApp.m_szEnginesFilePath);

	// save files list to file
	bRet = this->SaveFilesList(theApp.m_szFilesListFilePath, DEFAULT_FILES_FORMAT);
	::LogMessage((bRet ? _T("Saved files list: ") : _T("Error: Failed to save files list: ")) + theApp.m_szFilesListFilePath);
}

void CMainDlg::UpdateBitrateText()
{
	int nCurPos = this->m_SldBitrate.GetPos();
	CString szBuff;

	if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
	{
		// indicate that we have selected Quality based mode (VBR)
		if (HaveLangStrings())
			m_StcQualityBitrate.SetWindowText(GetLangString(0x00202002));
		else
			m_StcQualityBitrate.SetWindowText(_T("Quality:"));

		szBuff.Format(_T("%d"), nCurPos);

		EncoderPreset tmpPreset = GetCurrentPreset();
		tmpPreset.nQuality = nCurPos;
		UpdateCurrentPreset(tmpPreset);
	}
	else
	{
		if ((nCurPos >= 0) && (nCurPos < nNumValidCbrBitrates))
		{
			// indicate that we have selected Bitrate based mode (CBR)
			if (HaveLangStrings())
				m_StcQualityBitrate.SetWindowText(GetLangString(0x00202003));
			else
				m_StcQualityBitrate.SetWindowText(_T("Bitrate:"));

			if (nCurPos == 0)
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

EncoderPreset CMainDlg::GetCurrentPreset()
{
	return this->encPresets.GetAt(this->encPresets.FindIndex(this->nCurrentPreset));
}

void CMainDlg::UpdateCurrentPreset(EncoderPreset updatePreset)
{
	this->encPresets.SetAt(this->encPresets.FindIndex(this->nCurrentPreset), updatePreset);
}

void CMainDlg::AddItemToFileList(CString szPath)
{
	// add file to list
	CString szSize = _T("");
	ULARGE_INTEGER ulSize;
	ULONGLONG nFileSize;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	// check if file exist
	hFind = ::FindFirstFile(szPath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	ulSize.HighPart = FindFileData.nFileSizeHigh;
	ulSize.LowPart = FindFileData.nFileSizeLow;
	nFileSize = ulSize.QuadPart;

	::FindClose(hFind);

	// get file index and icon
	int nItem = this->m_LstFiles.GetItemCount();
	SHFILEINFO sfi;
	LV_ITEM lvi;

	SHGetFileInfo((LPCTSTR)szPath,
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
	if (GetFileExt(szPath).MakeLower() == _T("avs"))
	{
		// get input Audio stream information from Avisynth
		AvsAudioInfo infoAVS;
		memset(&infoAVS, 0, sizeof(AvsAudioInfo));
		if (GetAvisynthFileInfo(szPath, &infoAVS) == false)
			return; // failed to load Avisynth script

		nFileSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
	}
#endif

	// szSize.Format(_T("%I64d"), GetFileSize64(szPath));
	szSize.Format(_T("%I64d"), nFileSize);

	// update item file size
	this->m_LstFiles.SetItemText(nItem, 1, szSize);
}

void CMainDlg::ApplyPresetToDlg(EncoderPreset &Preset)
{
	// fill advanced encoder options list
	for (int i = 0; i < nNumEncoderOptions; i++)
	{
		// set all values
		this->m_LstSettings.SetItemText(i,
			1,
			encOpt[i].listOptNames.GetAt(encOpt[i].listOptNames.FindIndex(Preset.nSetting[i])));
	}

	// set bitrate or quality value
	if (Preset.nMode == AFTEN_ENC_MODE_CBR)
	{
		this->m_SldBitrate.SetTic(1);
		this->m_SldBitrate.SetRange(0, 19);
		this->m_SldBitrate.SetPos(FindValidBitratePos(Preset.nBitrate));
		this->m_ChkVbr.SetCheck(BST_UNCHECKED);
	}
	else if (Preset.nMode == AFTEN_ENC_MODE_VBR)
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

	// update number of threads
	if (Preset.nThreads == 0)
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
	if (Preset.nRawSampleRate == 0)
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
	if (Preset.nRawChannels == 0)
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
	if (pos != NULL)
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

void CMainDlg::HandleDropFiles(HDROP hDropInfo)
{
	int nCount = ::DragQueryFile(hDropInfo, (UINT)0xFFFFFFFF, NULL, 0);
	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			int nReqChars = ::DragQueryFile(hDropInfo, i, NULL, 0);

			CString szFile;
			::DragQueryFile(hDropInfo,
				i,
				szFile.GetBuffer(nReqChars * 2 + 8),
				nReqChars * 2 + 8);

			szFile.ReleaseBuffer();

			if (::GetFileAttributes(szFile) & FILE_ATTRIBUTE_DIRECTORY)
			{
				// insert dropped files in directory and subdirs
				this->SearchFolderForFiles(szFile, true);
			}
			else
			{
				// apply filter and add only .wav files
				CString szExt = ::PathFindExtension(szFile);
				szExt.MakeLower();
				szExt.Remove('.');

				// add only files with proper file extensions
				if (IsSupportedInputExt(szExt) == true)
				{
					// insert dropped file
					this->AddItemToFileList(szFile);
				}
			}
		}
	}

	::DragFinish(hDropInfo);
}

void CMainDlg::UpdateSettingsComboBox(int nItem)
{
	// remove all items from value combobox
	this->m_CmbValue.ResetContent();

	// add new items to combobox
	POSITION posNames = encOpt[nItem].listOptNames.GetHeadPosition();
	while (posNames != NULL)
		this->m_CmbValue.AddString(encOpt[nItem].listOptNames.GetNext(posNames));

	SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING, 15);

	// select default value or last selected
	if (this->encPresets.GetSize() <= 0)
		this->m_CmbValue.SetCurSel(encOpt[nItem].nDefaultValue);
	else
		this->m_CmbValue.SetCurSel(GetCurrentPreset().nSetting[nItem]);
}

void CMainDlg::SearchFolderForFiles(CString szPath, const bool bRecurse)
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
		szPath.TrimRight(_T("\\"));
		szPath.TrimRight(_T("/"));

		wsprintf(cTempBuf, _T("%s\\*.*\0"), szPath);

		hSearch = FindFirstFile(cTempBuf, &w32FileData);
		if (hSearch == INVALID_HANDLE_VALUE)
			return;

		while (fFinished == FALSE)
		{
			if (w32FileData.cFileName[0] != '.' &&
				!(w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				CString szTempBuf;
				szTempBuf.Format(_T("%s\\%s\0"), szPath, w32FileData.cFileName);

				// apply filter and add only .wav files
				CString szExt = ::PathFindExtension(szTempBuf);
				szExt.MakeLower();
				szExt.Remove('.');

				// add only files with proper file extensions
				if (IsSupportedInputExt(szExt) == true)
				{
					this->AddItemToFileList(szTempBuf);
				}
			}

			if (w32FileData.cFileName[0] != '.' &&
				w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				wsprintf(cTempBuf, _T("%s\\%s\0"), szPath, w32FileData.cFileName);

				// recurse subdirs
				if (bRecurse == true)
					this->SearchFolderForFiles(cTempBuf, true);
			}

			if (FindNextFile(hSearch, &w32FileData) == FALSE)
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					fFinished = TRUE;
				else
					return;
			}
		}

		if (FindClose(hSearch) == FALSE)
			return;
	}
	catch (...)
	{
		MessageBox(HaveLangStrings() ? GetLangString(0x0020702A) : _T("Error while searching for files!"),
			HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
			MB_OK | MB_ICONERROR);
	}
}

void CMainDlg::ShowOptionPopup(bool bUseRect)
{
	// get starting point for context menu
	POINT point;
	GetCursorPos(&point);

	int nItem;

	// show right click context menu for selected item
	POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
	if (pos != NULL)
		nItem = m_LstSettings.GetNextSelectedItem(pos);
	else
		return;

	if (bUseRect == true)
	{
		CRect rc;
		m_LstSettings.GetItemRect(nItem, rc, LVIR_BOUNDS);
		m_LstSettings.ClientToScreen(rc);

		// show only if user clicked in value column
		if (rc.PtInRect(point) == FALSE)
			return;
	}

	// create new popup menu
	CMenu *menu = new CMenu;
	menu->CreatePopupMenu();

	UINT nItemCount = ID_OPTIONS_MENU_START;
	POSITION posNames = encOpt[nItem].listOptNames.GetHeadPosition();
	while (posNames != NULL)
	{
		menu->AppendMenu(MF_STRING, nItemCount, encOpt[nItem].listOptNames.GetNext(posNames));
		nItemCount++;
	}

	// check currently selected option
	int nCurSel = this->m_CmbValue.GetCurSel();
	menu->CheckMenuItem(ID_OPTIONS_MENU_START + nCurSel, MF_CHECKED);

	// show popup menu to user
	::SetForegroundWindow(this->GetSafeHwnd());
	int nRet = (int)menu->TrackPopupMenu(TPM_RETURNCMD, point.x, point.y, this, NULL);
	if (nRet >= ID_OPTIONS_MENU_START)
	{
		// update setting in value combobox and in options list
		this->m_CmbValue.SetCurSel(nRet - ID_OPTIONS_MENU_START);
		this->OnCbnSelchangeComboSetting();
	}

	// free used memory
	if (menu)
		delete menu;
}

typedef struct TDRAGANDDROP
{
	CMainDlg *pDlg;
	HDROP hDropInfo;
} DRAGANDDROP, *PDRAGANDDROP;

static volatile bool bHandleDrop = true;
static volatile HANDLE hDDThread;
static DWORD dwDDThreadID;
static volatile DRAGANDDROP m_DDParam;

DWORD WINAPI DragAndDropThread(LPVOID lpParam)
{
	PDRAGANDDROP m_ThreadParam = (PDRAGANDDROP)lpParam;
	m_ThreadParam->pDlg->HandleDropFiles(m_ThreadParam->hDropInfo);
	bHandleDrop = true;
	return ::CloseHandle(hDDThread);
}

void CMainDlg::OnDropFiles(HDROP hDropInfo)
{
	if (bHandleDrop == true)
	{
		bHandleDrop = false;
		m_DDParam.pDlg = this;
		m_DDParam.hDropInfo = hDropInfo;

		hDDThread = ::CreateThread(NULL, 0, DragAndDropThread, (LPVOID)&m_DDParam, 0, &dwDDThreadID);
		if (hDDThread == NULL)
			bHandleDrop = true;
	}

	// under Win9x this does not work, we use separate thread to handle drop
	// this->HandleDropFiles(hDropInfo);
	CMyResizeDialog::OnDropFiles(hDropInfo);
}

void CMainDlg::OnEnChangeEditRawSampleRate()
{
	// check if number is in range
	CString szBuff;
	int nPos;

	this->m_EdtRawSamplerate.GetWindowText(szBuff);

	// check if we have focus (needed for editing)
	if (szBuff.Compare(_T("")) == 0)
		return;

	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
	{
		nPos = 0;
		this->m_SpnRawSampleRate.SetPos(nPos);
	}
	else
	{
		nPos = (int)_ttoi(szBuff);
		if ((nPos <= 0) || (nPos > INT_MAX))
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

void CMainDlg::OnEnChangeEditRawChannels()
{
	// check if number is in range
	CString szBuff;
	int nPos;

	this->m_EdtRawChannels.GetWindowText(szBuff);

	// check if we have focus (needed for editing)
	if (szBuff.Compare(_T("")) == 0)
		return;

	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
	{
		nPos = 0;
		this->m_SpnRawChannels.SetPos(nPos);
	}
	else
	{
		nPos = (int)_ttoi(szBuff);
		if ((nPos <= 0) || (nPos > INT_MAX))
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

void CMainDlg::OnEnChangeEditThreads()
{
	// check if number is in range
	CString szBuff;
	int nPos;

	this->m_EdtThreads.GetWindowText(szBuff);

	// check if we have focus (needed for editing)
	if (szBuff.Compare(_T("")) == 0)
		return;

	if (szBuff.Compare(DEFAULT_TEXT_AUTO) == 0)
	{
		nPos = 0;
		this->m_SpnThreads.SetPos(nPos);
	}
	else
	{
		nPos = (int)_ttoi(szBuff);
		if ((nPos <= 0) || (nPos > INT_MAX))
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

void CMainDlg::OnEnChangeEditOutputPath()
{
	CString szBuff;

	this->m_EdtOutPath.GetWindowText(szBuff);

	// update output file/path value
	if (this->bMultipleMonoInput == true)
		this->szOutputFile = szBuff;
	else
		this->szOutputPath = szBuff;
}

void CMainDlg::OnEnSetfocusEditOutputPath()
{
	CString szBuff;
	this->m_EdtOutPath.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0 || szBuff.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
		this->m_EdtOutPath.SetWindowText(_T(""));
}

void CMainDlg::OnEnSetfocusEditRawSampleRate()
{
	CString szBuff;
	this->m_EdtRawSamplerate.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
		this->m_EdtRawSamplerate.SetWindowText(_T(""));
}

void CMainDlg::OnEnSetfocusEditRawChannels()
{
	CString szBuff;
	this->m_EdtRawChannels.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_IGNORED) == 0)
		this->m_EdtRawChannels.SetWindowText(_T(""));
}

void CMainDlg::OnEnSetfocusEditThreads()
{
	CString szBuff;
	this->m_EdtThreads.GetWindowText(szBuff);
	if (szBuff.Compare(DEFAULT_TEXT_AUTO) == 0)
		this->m_EdtThreads.SetWindowText(_T(""));
}

void CMainDlg::OnEnKillfocusEditOutputPath()
{
	CString szBuff;
	this->m_EdtOutPath.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
	{
		if (this->bMultipleMonoInput == true)
			this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
		else
			this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
	}
	else
	{
		if (this->bMultipleMonoInput == true)
			this->szOutputFile = szBuff;
		else
			this->szOutputPath = szBuff;
	}
}

void CMainDlg::OnEnKillfocusEditRawSampleRate()
{
	CString szBuff;
	this->m_EdtRawSamplerate.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
		this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
}

void CMainDlg::OnEnKillfocusEditRawChannels()
{
	CString szBuff;
	this->m_EdtRawChannels.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
		this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
}

void CMainDlg::OnEnKillfocusEditThreads()
{
	CString szBuff;
	this->m_EdtThreads.GetWindowText(szBuff);
	if (szBuff.Compare(_T("")) == 0)
		this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
}

void CMainDlg::InitTitle()
{
	// set program name and version in main dialog title
	CString szDialogTitle = _T("");
	szDialogTitle.Format(_T("WAV to AC3 Encoder %s"), ENCWAVTOAC3_VERSION);
	this->SetWindowText(szDialogTitle);
}

void CMainDlg::InitDialogAnchors()
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

void CMainDlg::InitTooltips()
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

void CMainDlg::InitSettingsList()
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

void CMainDlg::InitDefaultPreset()
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

void CMainDlg::InitRawSamleFormatComboBox()
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

void CMainDlg::InitSettingsListGroups()
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

void CMainDlg::InitDialogControls()
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

BOOL CMainDlg::OnInitDialog()
{
	CMyResizeDialog::OnInitDialog();

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
	SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING, 15);
	SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);
	SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_RAW_SAMPLE_FORMAT, 15);
	SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_ENGINES, 15);

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

void SetListCtrlColumnText(CListCtrl& listCtrl, int nCol, CString& text)
{
	LVCOLUMN lvCol;
	::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
	lvCol.mask = LVCF_TEXT;
	listCtrl.GetColumn(nCol, &lvCol);
	lvCol.pszText = (LPTSTR)(LPCTSTR)text;
	listCtrl.SetColumn(nCol, &lvCol);
}

void CMainDlg::InitLang(bool initLangMenu)
{
	this->InitRawSamleFormatComboBox();

	this->InitSettingsListGroups();

	if (HaveLangStrings())
	{
		if (initLangMenu == true)
			this->InitLangMenu();

		// Main Dialog: Buttons
		this->InitLangButtons();

		// Main Dialog: Static Text
		this->InitLangStaticText();

		// Main Dialog: Files List
		this->InitLangFilesList();

		// Main Dialog: Settings List
		this->InitLangSettingsList();
	}

	::InitEncoderOptions();

	this->InitSettingsList();

	// restore settings list
	if (initLangMenu == false)
	{
		EncoderPreset tmpPreset = GetCurrentPreset();
		this->ApplyPresetToDlg(tmpPreset);
	}

	if (HaveLangStrings())
	{
		// Main Dialog: Main Menu
		this->InitLangMainMenu();
	}

	// init dialog tooltips
	this->InitTooltips();

	// set output path string
	CString szBuff = this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath;
	if (szBuff.Compare(_T("")) == 0 || szBuff.Left(1).Compare(_T("<")) == 0)
		this->m_EdtOutPath.SetWindowText(this->bMultipleMonoInput == true ? DEFAULT_TEXT_OUTPUT_FILE : DEFAULT_TEXT_OUTPUT_PATH);
	else
		this->m_EdtOutPath.SetWindowText(this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath);
}

void CMainDlg::InitLangButtons()
{
	m_BtnEncode.SetWindowTextW(GetLangString(0x00201001));
	m_BtnResetCurrent.SetWindowTextW(GetLangString(0x00201002));
	m_BtnRemove.SetWindowTextW(GetLangString(0x00201003));
	m_BtnAddNew.SetWindowTextW(GetLangString(0x00201004));
	m_BtnAddFiles.SetWindowTextW(GetLangString(0x00201005));
	m_BtnBrowse.SetWindowTextW(GetLangString(0x00201006));
	m_BtnMuxWizard.SetWindowTextW(GetLangString(0x00201007));
	m_BtnEngines.SetWindowTextW(GetLangString(0x00201008));
}

void CMainDlg::InitLangStaticText()
{
	this->GetDlgItem(IDC_STATIC_PRESET)->SetWindowTextW(GetLangString(0x00202001));

	if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
		this->GetDlgItem(IDC_STATIC_QUALITY)->SetWindowTextW(GetLangString(0x00202002));
	else
		this->GetDlgItem(IDC_STATIC_QUALITY)->SetWindowTextW(GetLangString(0x00202003));

	this->GetDlgItem(IDC_STATIC_OPTION_VALUE)->SetWindowTextW(GetLangString(0x00202004));
	this->GetDlgItem(IDC_STATIC_SAMPLE_FORMAT)->SetWindowTextW(GetLangString(0x00202005));
	this->GetDlgItem(IDC_STATIC_SAMPLE_RATE)->SetWindowTextW(GetLangString(0x00202006));
	this->GetDlgItem(IDC_STATIC_CHANNELS)->SetWindowTextW(GetLangString(0x00202007));
	this->GetDlgItem(IDC_STATIC_ENGINE)->SetWindowTextW(GetLangString(0x00202008));
	this->GetDlgItem(IDC_STATIC_THREADS)->SetWindowTextW(GetLangString(0x00202009));
	this->GetDlgItem(IDC_CHECK_MULTIPLE_MONO_INPUT)->SetWindowTextW(GetLangString(0x0020200A));

	if (this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED)
		this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(GetLangString(0x0020200B));
	else
		this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(GetLangString(0x0020200C));
}

void CMainDlg::InitLangFilesList()
{
	SetListCtrlColumnText(this->m_LstFiles, 0, GetLangString(0x00203001));
	SetListCtrlColumnText(this->m_LstFiles, 1, GetLangString(0x00203002));
}

void CMainDlg::InitLangFilesListContextMenu(CMenu &m_hMenu)
{
	m_hMenu.ModifyMenuW(0, MF_STRING | MF_BYPOSITION, 0, GetLangString(0x00204001));
	m_hMenu.ModifyMenuW(ID_LIST_ADDFILES, 0, ID_LIST_ADDFILES, GetLangString(0x00204002));
	m_hMenu.ModifyMenuW(ID_LIST_ADDDIRECTORY, 0, ID_LIST_ADDDIRECTORY, GetLangString(0x00204003));
	m_hMenu.ModifyMenuW(ID_LIST_MUXWIZARD, 0, ID_LIST_MUXWIZARD, GetLangString(0x00204004));
	m_hMenu.ModifyMenuW(ID_LIST_LOADLIST, 0, ID_LIST_LOADLIST, GetLangString(0x00204005));
	m_hMenu.ModifyMenuW(ID_LIST_SAVELIST, 0, ID_LIST_SAVELIST, GetLangString(0x00204006));
	m_hMenu.ModifyMenuW(ID_LIST_MOVEUP, 0, ID_LIST_MOVEUP, GetLangString(0x00204007));
	m_hMenu.ModifyMenuW(ID_LIST_MOVEDOWN, 0, ID_LIST_MOVEDOWN, GetLangString(0x00204008));
	m_hMenu.ModifyMenuW(ID_LIST_DELFILES, 0, ID_LIST_DELFILES, GetLangString(0x00204009));
	m_hMenu.ModifyMenuW(ID_LIST_CLEARLIST, 0, ID_LIST_CLEARLIST, GetLangString(0x0020400A));
}

void CMainDlg::InitLangSettingsList()
{
	SetListCtrlColumnText(this->m_LstSettings, 0, GetLangString(0x00205001));
	SetListCtrlColumnText(this->m_LstSettings, 1, GetLangString(0x00205002));
}

void CMainDlg::InitLangMainMenu()
{
	// get main menu handle
	CMenu *m_hMenu = this->GetMenu();

	// File menu
	m_hMenu->ModifyMenuW(0, MF_STRING | MF_BYPOSITION, 0, GetLangString(0x00101001));
	m_hMenu->ModifyMenuW(ID_FILE_ADDFILES, 0, ID_FILE_ADDFILES, GetLangString(0x00101002));
	m_hMenu->ModifyMenuW(ID_FILE_ADDDIRECTORY, 0, ID_FILE_ADDDIRECTORY, GetLangString(0x00101003));
	m_hMenu->ModifyMenuW(ID_FILE_MUXWIZARD, 0, ID_FILE_MUXWIZARD, GetLangString(0x00101004));
	m_hMenu->ModifyMenuW(ID_FILE_LOADFILESLIST, 0, ID_FILE_LOADFILESLIST, GetLangString(0x00101005));
	m_hMenu->ModifyMenuW(ID_FILE_SAVEFILESLIST, 0, ID_FILE_SAVEFILESLIST, GetLangString(0x00101006));
	m_hMenu->ModifyMenuW(ID_FILE_LOADPRESETS, 0, ID_FILE_LOADPRESETS, GetLangString(0x00101007));
	m_hMenu->ModifyMenuW(ID_FILE_SAVEPRESETS, 0, ID_FILE_SAVEPRESETS, GetLangString(0x00101008));
	m_hMenu->ModifyMenuW(ID_FILE_EXIT, 0, ID_FILE_EXIT, GetLangString(0x00101009));

	// Options menu
	m_hMenu->ModifyMenuW(1, MF_STRING | MF_BYPOSITION, 1, GetLangString(0x00102001));
	m_hMenu->ModifyMenuW(ID_OPTIONS_DISABLEALLWARNINGS, 0, ID_OPTIONS_DISABLEALLWARNINGS, GetLangString(0x00102002));
	m_hMenu->ModifyMenuW(ID_OPTIONS_SAVECONFIGURATIONONEXIT, 0, ID_OPTIONS_SAVECONFIGURATIONONEXIT, GetLangString(0x00102003));
	m_hMenu->ModifyMenuW(ID_OPTIONS_LOADCONFIGURATION, 0, ID_OPTIONS_LOADCONFIGURATION, GetLangString(0x00102004));
	m_hMenu->ModifyMenuW(ID_OPTIONS_SAVECONFIGURATION, 0, ID_OPTIONS_SAVECONFIGURATION, GetLangString(0x00102005));

	// Language menu
	m_hMenu->ModifyMenuW(2, MF_STRING | MF_BYPOSITION, 2, GetLangString(0x00103001));
	m_hMenu->ModifyMenuW(ID_LANGUAGE_DEFAULT, 0, ID_LANGUAGE_DEFAULT, GetLangString(0x00103002));

	// Help menu
	m_hMenu->ModifyMenuW(3, MF_STRING | MF_BYPOSITION, 3, GetLangString(0x00104001));
	m_hMenu->ModifyMenuW(ID_HELP_COMMAND_LINE, 0, ID_HELP_COMMAND_LINE, GetLangString(0x00104002));
	m_hMenu->ModifyMenuW(ID_HELP_WEBSITE, 0, ID_HELP_WEBSITE, GetLangString(0x00104003));
	m_hMenu->ModifyMenuW(ID_HELP_ABOUT, 0, ID_HELP_ABOUT, GetLangString(0x00104004));

	// restore options checked state
	m_hMenu->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS,
		this->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);

	m_hMenu->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
		this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);

	// redraw main menu
	this->DrawMenuBar();
}

void CMainDlg::InitLangMenu()
{
	// insert languages to Language sub-menu
	if (theApp.m_LangLst.GetCount() > 0)
	{
		CMenu *m_hMenu = this->GetMenu();
		CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

		m_hLangMenu->DeleteMenu(ID_LANGUAGE_DEFAULT, 0);
		//m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_UNCHECKED);
		//m_hLangMenu->AppendMenu(MF_SEPARATOR);

		POSITION pos = theApp.m_LangLst.GetHeadPosition();
		int i = 0;
		while (pos)
		{
			Lang lang = theApp.m_LangLst.GetNext(pos);
			CString szBuff;
			szBuff.Format(_T("%s (%s)"), lang.szEnglishName, lang.szTargetName);
			m_hLangMenu->AppendMenu(MF_STRING, ID_LANGUAGE_MENU_START + i, szBuff);

			if (theApp.m_nLangId == i)
				m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_CHECKED);
			else
				m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);

			i++;
		}
	}
	else
	{
		CMenu *m_hMenu = this->GetMenu();
		CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);
		m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_CHECKED);
	}
}

int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData)
{
	CMainDlg *pDlg = (CMainDlg *)lParamData;

	LVFINDINFO pInfo1, pInfo2;
	pInfo1.flags = LVFI_PARAM;
	pInfo2.flags = LVFI_PARAM;
	pInfo1.lParam = lParam1;
	pInfo2.lParam = lParam2;

	int nIndex1 = pDlg->m_LstFiles.FindItem(&pInfo1);
	int nIndex2 = pDlg->m_LstFiles.FindItem(&pInfo2);
	CString szItem1 = pDlg->m_LstFiles.GetItemText(nIndex1, pDlg->nSortColumn);
	CString szItem2 = pDlg->m_LstFiles.GetItemText(nIndex2, pDlg->nSortColumn);

	if (pDlg->nSortOrder[pDlg->nSortColumn] == true)
		return szItem1.Compare(szItem2);

	if (pDlg->nSortOrder[pDlg->nSortColumn] == false)
		return szItem2.Compare(szItem1);

	return 0;
}

void CMainDlg::OnListAddFiles()
{
	this->OnFileAddFiles();
}

void CMainDlg::OnListAddDirectory()
{
	this->OnFileAddDirectory();
}

void CMainDlg::OnListMuxWizard()
{
	this->OnFileMuxWizard();
}

void CMainDlg::OnListLoadList()
{
	this->OnFileLoadFilesList();
}

void CMainDlg::OnListSavelist()
{
	this->OnFileSaveFilesList();
}

void CMainDlg::OnListMoveUp()
{
	// move files list items one position up
	POSITION pos;
	CString szPath[2] = { _T(""), _T("") };
	CString szSize[2] = { _T(""), _T("") };

	pos = this->m_LstFiles.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
		if ((nItem > 0) && (this->m_LstFiles.GetItemCount() >= 2))
		{
			szPath[0] = this->m_LstFiles.GetItemText(nItem, 0);
			szSize[0] = this->m_LstFiles.GetItemText(nItem, 1);

			szPath[1] = this->m_LstFiles.GetItemText(nItem - 1, 0);
			szSize[1] = this->m_LstFiles.GetItemText(nItem - 1, 1);

			this->m_LstFiles.SetItemText(nItem, 0, szPath[1]);
			this->m_LstFiles.SetItemText(nItem, 1, szSize[1]);

			this->m_LstFiles.SetItemText(nItem - 1, 0, szPath[0]);
			this->m_LstFiles.SetItemText(nItem - 1, 1, szSize[0]);

			this->m_LstFiles.SetItemState(nItem - 1, LVIS_SELECTED, LVIS_SELECTED);
			this->m_LstFiles.SetItemState(nItem, 0, LVIS_SELECTED);
		}
		else
		{
			return;
		}
	}
}

void CMainDlg::OnListMoveDown()
{
	// move files list items one position down
	POSITION pos;
	CString szPath[2] = { _T(""), _T("") };
	CString szSize[2] = { _T(""), _T("") };
	CList<ItemToMove, ItemToMove> listSel;

	pos = this->m_LstFiles.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
		if ((nItem < this->m_LstFiles.GetItemCount() - 1) && (this->m_LstFiles.GetItemCount() >= 2))
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
	while (pos != NULL)
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

void CMainDlg::OnListDelFiles()
{
	CList<int, int> list;
	POSITION pos;

	// get all selected items
	pos = this->m_LstFiles.GetFirstSelectedItemPosition();
	while (pos != NULL)
		list.AddTail(this->m_LstFiles.GetNextSelectedItem(pos));

	// remove all selected items
	pos = list.GetTailPosition();
	while (pos != NULL)
		this->m_LstFiles.DeleteItem(list.GetPrev(pos));
}

void CMainDlg::OnListClearList()
{
	this->m_LstFiles.DeleteAllItems();
}

void CMainDlg::OnLvnItemchangedListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	static int nLastItem = -1;
	POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();

	if (pos != NULL)
	{
		int nItem = m_LstSettings.GetNextSelectedItem(pos);

		// update values combobox if item has changed
		if (nLastItem != nItem)
		{
			this->UpdateSettingsComboBox(nItem);
			nLastItem = nItem;
		}
	}

	*pResult = 0;
}

void CMainDlg::OnLvnKeydownListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	// handle keyboard events here
	switch (pLVKeyDow->wVKey)
	{
	case VK_DELETE:
	{
		this->OnListDelFiles();
	}
	break;
	case VK_UP: // left Alt + Up
	{
		if (GetAsyncKeyState(VK_LMENU) & 0x8001)
			this->OnListMoveUp();
	}
	break;
	case VK_DOWN: // left Alt + Down
	{
		if (GetAsyncKeyState(VK_LMENU) & 0x8001)
			this->OnListMoveDown();
	}
	break;

	default: break;
	};

	*pResult = 0;
}

void CMainDlg::OnLvnColumnclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	nSortColumn = pNMLV->iSubItem;

	this->m_LstFiles.SortItems(CompareFunction, reinterpret_cast<DWORD_PTR>(this));

	if (nSortOrder[nSortColumn] == true)
		nSortOrder[nSortColumn] = false;
	else
		nSortOrder[nSortColumn] = true;

	*pResult = 0;
}

void CMainDlg::OnLvnKeydownListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	// handle keyboard events here
	switch (pLVKeyDow->wVKey)
	{
	case VK_LEFT:
	{
		POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = this->m_LstSettings.GetNextSelectedItem(pos);

			// get current selection index in value combobox
			int nVal = this->m_CmbValue.GetCurSel();

			// select previous item in value combobox
			if (nVal > 0)
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
		if (pos != NULL)
		{
			int nItem = this->m_LstSettings.GetNextSelectedItem(pos);

			// get current selection index in value combobox
			int nVal = this->m_CmbValue.GetCurSel();

			// select next item in value combobox
			if (nVal < this->m_CmbValue.GetCount() - 1)
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

void CMainDlg::OnNMRclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	// get starting point for context menu
	POINT point;
	GetCursorPos(&point);

	// show right click context menu
	CMenu m_hMenu;
	m_hMenu.LoadMenu(IDR_MENU_LIST);
	CMenu *m_hSubMenu = m_hMenu.GetSubMenu(0);
	::SetForegroundWindow(this->GetSafeHwnd());

	// init context menu language
	if (HaveLangStrings())
		InitLangFilesListContextMenu(m_hMenu);

	// display menu
	m_hSubMenu->TrackPopupMenu(0, point.x, point.y, this, NULL);

	*pResult = 0;
}

void CMainDlg::OnNMRclickListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
	// show option values in popup menu
	this->ShowOptionPopup(true);

	*pResult = 0;
}

void CMainDlg::OnNMDblclkListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_LstSettings.GetNextSelectedItem(pos);

		// show option help text
		this->MessageBox(encOpt[nItem].szHelpText, encOpt[nItem].szName, MB_ICONINFORMATION | MB_OK);
	}

	*pResult = 0;
}

#ifndef DISABLE_AVISYNTH
bool CMainDlg::GetAvisynthFileInfo(CString szFileName, AvsAudioInfo *pInfoAVS)
{
	TCHAR *pszInPath = szFileName.GetBuffer();

	if (pInfoAVS == NULL)
		return false;

	memset(pInfoAVS, 0, sizeof(AvsAudioInfo));

	// show AVS file information text
	CAvs2Raw decoderAVS;
	char szInputFileAVS[MAX_PATH] = "";

	// initialize Avisynth - only one input file supported
	// NOTE: only ANSI file names supported
#ifdef _UNICODE
	ConvertUnicodeToAnsi(pszInPath, szInputFileAVS, lstrlen(pszInPath));
	if (decoderAVS.OpenAvisynth(szInputFileAVS) == false)
#else
	if (decoderAVS.OpenAvisynth(pszInPath) == false)
#endif
	{
		::LogMessage(_T("Error: Failed to initialize Avisynth!"));

		this->MessageBox(HaveLangStrings() ? GetLangString(0x00207022) : _T("Failed to initialize Avisynth"),
			HaveLangStrings() ? GetLangString(0x00207010) : _T("Error"),
			MB_ICONERROR | MB_OK);

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

void CMainDlg::OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_LstFiles.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_LstFiles.GetNextSelectedItem(pos);
		CString szFileName = m_LstFiles.GetItemText(nItem, 0);

#ifndef DISABLE_AVISYNTH
		// show AVS file information text
		if (GetFileExt(szFileName).MakeLower() == _T("avs"))
		{
			// get input Audio stream information from Avisynth
			AvsAudioInfo infoAVS;
			memset(&infoAVS, 0, sizeof(AvsAudioInfo));
			if (GetAvisynthFileInfo(szFileName, &infoAVS))
			{
				CString szInfo;
				TCHAR *pszInPath = szFileName.GetBuffer();

				// get information about input file
				szInfo +=
					(HaveLangStrings() ? GetLangString(0x00207023) : _T("Sample format")) +
					_T("\t: ");

				switch (infoAVS.nSampleType)
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
					szInfo +=
						(HaveLangStrings() ? GetLangString(0x00207024) : _T("unknown")) +
						_T("\n");
					break;
				}

				CString szBuff;

				szBuff.Format(_T("%s\t: %d\n"),
					HaveLangStrings() ? GetLangString(0x00207025) : _T("Sample rate"),
					infoAVS.nSamplesPerSecond);
				szInfo += szBuff;

				szBuff.Format(_T("%s\t\t: %d\n"),
					HaveLangStrings() ? GetLangString(0x00207026) : _T("Channels"),
					infoAVS.nAudioChannels);
				szInfo += szBuff;

				szBuff.Format(_T("%s\t: %I64d\n"),
					HaveLangStrings() ? GetLangString(0x00207027) : _T("Audio samples"),
					infoAVS.nAudioSamples);
				szInfo += szBuff;

				szBuff.Format(_T("%s\t: %I64d"),
					HaveLangStrings() ? GetLangString(0x00207028) : _T("Decoded size"),
					infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels);
				szInfo += szBuff;

				szFileName.ReleaseBuffer();

				// show info to user
				this->MessageBox(szInfo,
					HaveLangStrings() ? GetLangString(0x00207029) : _T("AVS File Properties"),
					MB_ICONINFORMATION | MB_OK);
			}
		}
#endif
	}

	*pResult = 0;
}

void CMainDlg::OnFileAddFiles()
{
	TCHAR *pFiles = NULL;
	const DWORD dwMaxSize = (4096 * MAX_PATH);
	try
	{
		pFiles = (TCHAR *)malloc(dwMaxSize);
		if (pFiles == NULL)
		{
			MessageBox(
				HaveLangStrings() ? GetLangString(0x00207009) : _T("Failed to allocate memory for filenames buffer!"),
				HaveLangStrings() ? GetLangString(0x0020700A) : _T("Fatal Error"),
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
		if (fd.DoModal() == IDOK)
		{
			// get first file position
			POSITION pos = fd.GetStartPosition();
			while (pos != NULL)
			{
				// do not check file extension when using add file dialog because user can use *.* filter
				this->AddItemToFileList(fd.GetNextPathName(pos));
			}
		}
	}
	catch (...)
	{
		if (pFiles != NULL)
		{
			free(pFiles);
			pFiles = NULL;
		}
	}

	if (pFiles != NULL)
	{
		free(pFiles);
		pFiles = NULL;
	}
}

void CMainDlg::OnFileAddDirectory()
{
	LPMALLOC pMalloc;
	BROWSEINFO bi;
	LPITEMIDLIST pidlDesktop;
	LPITEMIDLIST pidlBrowse;
	TCHAR *lpBuffer;

	if (SHGetMalloc(&pMalloc) == E_FAIL)
		return;

	if ((lpBuffer = (TCHAR *)pMalloc->Alloc(MAX_PATH * 2)) == NULL)
	{
		pMalloc->Release();
		return;
	}

	if (!SUCCEEDED(SHGetSpecialFolderLocation(::GetDesktopWindow(), CSIDL_DESKTOP, &pidlDesktop)))
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
	bi.lpszTitle = HaveLangStrings() ? GetLangString(0x0020700B) : _T("Add directory with supported input files:");
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
	bi.iImage = 0;

	pidlBrowse = ::SHBrowseForFolder(&bi);
	if (pidlBrowse != NULL)
	{
		if (SHGetPathFromIDList(pidlBrowse, lpBuffer))
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

void CMainDlg::OnFileMuxWizard()
{
	CMuxDlg dlg;

	// get number of files to process
	int nItemsCount = this->m_LstFiles.GetItemCount();

	// limit number of processed items
	if (nItemsCount > NUM_MAX_INPUT_FILES)
		nItemsCount = NUM_MAX_INPUT_FILES;

	for (int i = 0; i < nItemsCount; i++)
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
	if (encOpt[nIndexChconfig].nIgnoreValue != tmpPreset.nSetting[nIndexChconfig])
	{
		dlg.nChannelConfig = ccAften[encOpt[nIndexChconfig].listOptValues.GetAt(encOpt[nIndexChconfig].listOptValues.FindIndex(tmpPreset.nSetting[nIndexChconfig]))].acmod;
		dlg.bLFE = (ccAften[encOpt[nIndexChconfig].listOptValues.GetAt(encOpt[nIndexChconfig].listOptValues.FindIndex(tmpPreset.nSetting[nIndexChconfig]))].lfe == 1) ? true : false;
		bUpdateChconfig = true;
	}
	else
	{
		if (encOpt[nIndexAcmod].nIgnoreValue != tmpPreset.nSetting[nIndexAcmod])
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

	if (dlg.DoModal() == IDOK)
	{
		int nRet = IDYES;

		if (this->bDisableAllWarnings == false)
		{
			nRet = this->MessageBox(
				HaveLangStrings() ? GetLangString(0x0020700C) :
				_T("Remove all files from list and add files selected in MUX Wizard?\n\n")
				_T("Note: The channel configuration for current preset will be adjusted."),
				HaveLangStrings() ? GetLangString(0x0020700D) : _T("MUX Wizard"),
				MB_YESNO | MB_ICONQUESTION);
		}

		if (nRet == IDYES)
		{
			// clear list
			this->m_LstFiles.DeleteAllItems();

			// add items to file list
			switch (dlg.nChannelConfig)
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

				if (dlg.bLFE == true)
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

				if (dlg.bLFE == true)
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

				if (dlg.bLFE == true)
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

				if (dlg.bLFE == true)
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
			if (bUpdateChconfig == true)
			{
				int acmod = dlg.nChannelConfig;
				int lfe = (dlg.bLFE == true) ? 1 : 0;

				for (int i = 0; i < nNumChannelConfigAften; i++)
				{
					if ((ccAften[i].acmod == acmod) && (ccAften[i].lfe == lfe))
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
			if (this->bMultipleMonoInput == false)
			{
				this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
				this->OnBnClickedCheckMultipleMonoInput();
			}

			// update current preset
			UpdateCurrentPreset(tmpPreset);
		}
	}
}

void CMainDlg::OnFileLoadFilesList()
{
	CFileDialog fd(TRUE,
		_T("files"),
		_T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
		HaveLangStrings() ? GetLangString(0x0020700E) : _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"),
		this);

	if (fd.DoModal() == IDOK)
	{
		// get full path from file dialog
		CString szFileName = fd.GetPathName();

		// load files list from file
		LoadFilesList(szFileName);
	}
}

void CMainDlg::OnFileSaveFilesList()
{
	CFileDialog fd(FALSE,
		_T("files"),
		_T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
		HaveLangStrings() ? GetLangString(0x0020700E) : _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"),
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

void CMainDlg::OnFileLoadPresets()
{
	CFileDialog fd(TRUE,
		_T("presets"),
		_T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
		HaveLangStrings() ? GetLangString(0x0020700F) : _T("Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||"),
		this);

	if (fd.DoModal() == IDOK)
	{
		// get full path from file dialog
		CString szFileName = fd.GetPathName();

		// load presets list from file
		if (::LoadEncoderPresets(this->encPresets, szFileName, this->defaultPreset) == true)
		{
			// populate presets list
			this->m_CmbPresets.ResetContent();

			for (int i = 0; i < encPresets.GetCount(); i++)
			{
				this->m_CmbPresets.AddString(encPresets.GetAt(encPresets.FindIndex(i)).szName);
			}

			SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);

			// select first preset
			this->nCurrentPreset = 0;
			this->m_CmbPresets.SetCurSel(0);

			// update all controls
			this->OnCbnSelchangeComboPresets();
		}
	}
}

void CMainDlg::OnFileSavePresets()
{
	CFileDialog fd(FALSE,
		_T("presets"),
		_T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
		HaveLangStrings() ? GetLangString(0x0020700F) : _T("Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||"),
		this);

	if (fd.DoModal() == IDOK)
	{
		// get full path from file dialog
		CString szFileName = fd.GetPathName();

		// save presets list to file
		::SaveEncoderPresets(this->encPresets, szFileName, this->defaultPreset);
	}
}

void CMainDlg::OnFileExit()
{
	this->EndDialog(IDOK);
}

void CMainDlg::OnOptionsDisableAllWarnings()
{
	this->bDisableAllWarnings = this->bDisableAllWarnings ? false : true;
	this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS,
		this->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);
}

void CMainDlg::OnOptionsSaveConfigurationOnExit()
{
	this->bSaveConfig = this->bSaveConfig ? false : true;
	this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
		this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);
}

void CMainDlg::OnOptionsLoadConfiguration()
{
	this->LoadAllConfiguration();
}

void CMainDlg::OnOptionsSaveConfiguration()
{
	// save configuration to disk (works also in read-only mode)
	this->SaveAllConfiguration();
}

void CMainDlg::OnLanguageChangeDefault()
{
	// set language to default
	theApp.m_nLangId = -1;
	theApp.m_bHaveLang = FALSE;
	theApp.m_Lang = NULL;
	theApp.m_szLangFileName = _T("");

	// update Language menu checked status
	CMenu *m_hMenu = this->GetMenu();
	CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

	m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_CHECKED);

	POSITION pos = theApp.m_LangLst.GetHeadPosition();
	int i = 0;
	while (pos)
	{
		m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);
		i++;
	}

	// init language
	InitLang(false);
}

void CMainDlg::OnLanguageChange(UINT nID)
{
	// set language by ID
	POSITION pos = theApp.m_LangLst.FindIndex(nID - ID_LANGUAGE_MENU_START);
	if (pos != NULL)
	{
		Lang lang = theApp.m_LangLst.GetAt(pos);
		theApp.m_nLangId = nID - ID_LANGUAGE_MENU_START;
		theApp.m_bHaveLang = TRUE;
		theApp.m_Lang = lang.lm;
		theApp.m_szLangFileName = lang.szFileName;
	}

	// update Language menu checked status
	CMenu *m_hMenu = this->GetMenu();
	CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

	m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_UNCHECKED);

	pos = theApp.m_LangLst.GetHeadPosition();
	int i = 0;
	while (pos)
	{
		Lang lang = theApp.m_LangLst.GetNext(pos);
		if (theApp.m_nLangId == i)
			m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_CHECKED);
		else
			m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);

		i++;
	}

	// init language
	this->InitLang(false);
}

void CMainDlg::OnHelpCommandLine()
{
	ShowCommandLineHelp(this->GetSafeHwnd());
}

void CMainDlg::OnHelpWebsite()
{
	// go to program website using default Internet browser
	LaunchAndWait(ENCWAVTOAC3_URL_HOME, _T(""), FALSE);
}

void CMainDlg::OnHelpAbout()
{
	CAboutDlg dlg;

	// show program about dialog box
	dlg.DoModal();
}
