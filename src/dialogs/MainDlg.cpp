#include "StdAfx.h"
#include "MainApp.h"
#include "MainDlg.h"
#include "WorkDlg.h"
#include "MuxDlg.h"
#include "EnginesDlg.h"
#include "AboutDlg.h"
#include "utilities\Utilities.h"
#include "utilities\TimeCount.h"

namespace app
{
    IMPLEMENT_DYNAMIC(CMainDlg, CDialog)
    CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
        : CMyDialogEx(CMainDlg::IDD, pParent)
    {
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

        this->nSortOrder[0] = true;
        this->nSortOrder[1] = true;
        this->szOutputPath = _T("");
        this->szOutputFile = _T("");
        this->bMultipleMonoInput = false;
        this->bVisible = false;
        this->bDisableAllWarnings = false;
        this->bSaveConfig = true;
    }

    void CMainDlg::DoDataExchange(CDataExchange* pDX)
    {
        CMyDialogEx::DoDataExchange(pDX);
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

    BEGIN_MESSAGE_MAP(CMainDlg, CMyDialogEx)
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
            CMyDialogEx::OnPaint();
        }
    }

    HCURSOR CMainDlg::OnQueryDragIcon()
    {
        return static_cast<HCURSOR>(m_hIcon);
    }

    void CMainDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
    {
        if (this->bVisible == false)
            lpwndpos->flags &= ~SWP_SHOWWINDOW;

        CMyDialogEx::OnWindowPosChanging(lpwndpos);
    }

    void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
    {
        this->UpdateBitrateText();

        CMyDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
    }

    void CMainDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
    {
        if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_THREADS)
        {
            CString szBuff;

            if (nPos == 0)
                szBuff = DEFAULT_TEXT_AUTO;
            else
                szBuff.Format(_T("%d"), nPos);

            this->m_EdtThreads.SetWindowText(szBuff);
        }
        if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_SAMPLE_RATE)
        {
            CString szBuff;

            if (nPos == 0)
                szBuff = DEFAULT_TEXT_IGNORED;
            else
                szBuff.Format(_T("%d"), nPos);

            this->m_EdtRawSamplerate.SetWindowText(szBuff);
        }
        if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_CHANNELS)
        {
            CString szBuff;

            if (nPos == 0)
                szBuff = DEFAULT_TEXT_IGNORED;
            else
                szBuff.Format(_T("%d"), nPos);

            this->m_EdtRawChannels.SetWindowText(szBuff);
        }
        else
        {
            CMyDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
        }

        CMyDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
    }

    void CMainDlg::OnClose()
    {
        if (this->bSaveConfig == true)
        {
            try
            {
                this->SaveAllConfiguration();
            }
            catch (...)
            {
                OutputDebugString(_T("Failed to save configuration."));
            }
        }

        CMyDialogEx::OnClose();
    }

    void CMainDlg::OnDestroy()
    {
        CMyDialogEx::OnDestroy();

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
        return CMyDialogEx::PreTranslateMessage(pMsg);
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

        if (m_Config.m_bIsPortable == true)
        {
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        }
        else
        {
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());
        }

        int nItemsCount = this->m_LstFiles.GetItemCount();
        if (nItemsCount <= 0)
        {
            OutputDebugString(_T("Error: Add at least one file to the file list!"));
            MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207011).c_str() : _T("Add at least one file to the file list!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_ICONERROR | MB_OK);

            return;
        }

        if ((this->bMultipleMonoInput == true) && (nItemsCount < 1 || nItemsCount > 6))
        {
            OutputDebugString(_T("Error: Supported are minimum 1 and maximum 6 mono input files!"));
            MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207012).c_str() : _T("Supported are minimum 1 and maximum 6 mono input files!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_ICONERROR | MB_OK);

            return;
        }

        bWorking = true;

        if (this->api.IsAftenOpen())
        {
            this->api.CloseAftenAPI();
        }

        if (this->api.OpenAftenAPI() == false)
        {
            OutputDebugString(_T("Error: Failed to load libaften.dll dynamic library!"));
            MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207013).c_str() : _T("Failed to load libaften.dll dynamic library!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_ICONERROR | MB_OK);

            bWorking = false;
            return;
        }

        CWorkDlg dlg;
        util::CListT<CString> list;
        util::CListT<char> listStatus;
        dlg.pWorkerContext->nTotalSize = 0;
        CString szSizeBuff;
        CString szFileBuffer;
        bool bAvisynthInput = false;

        for (int i = 0; i < nItemsCount; i++)
        {
            szFileBuffer = this->m_LstFiles.GetItemText(i, 0);
            if (util::Utilities::GetFileExtension(szFileBuffer).MakeLower() == _T("avs"))
                bAvisynthInput = true;

            list.Insert(szFileBuffer);

            char status = false;
            listStatus.Insert(status);

            szSizeBuff = this->m_LstFiles.GetItemText(i, 1);
            dlg.pWorkerContext->nTotalSize += _ttoi64(szSizeBuff);
        }

        if ((this->bMultipleMonoInput == true) && (bAvisynthInput == true))
        {
            OutputDebugString(_T("Error: Disable 'Multiple mono input' mode in order to use Avisynth scripts!"));
            MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207014).c_str() : _T("Disable 'Multiple mono input' mode in order to use Avisynth scripts!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_ICONERROR | MB_OK);

            bWorking = false;
            return;
        }

        dlg.pWorkerContext->pPreset = &this->GetCurrentPreset();
        dlg.pWorkerContext->pFilesList = &list;
        dlg.pWorkerContext->pStatusList = &listStatus;
        this->m_EdtOutPath.GetWindowText(dlg.pWorkerContext->szOutPath);
        dlg.pWorkerContext->bUseOutPath = false;

        int nLen = dlg.pWorkerContext->szOutPath.GetLength();
        if (nLen < 3)
        {
            OutputDebugString(_T("Error: Invalid output path!"));
            this->MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207015).c_str() : _T("Invalid output path!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_OK | MB_ICONERROR);

            bWorking = false;
            return;
        }

        CString szExt = dlg.pWorkerContext->szOutPath.Right(4);
        if (this->bMultipleMonoInput == true)
        {
            if (dlg.pWorkerContext->szOutPath.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0)
            {
                if ((nLen < 4) || (szExt.CompareNoCase(_T(".ac3")) != 0))
                {
                    OutputDebugString(_T("Error: Invalid output file!"));
                    this->MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207016).c_str() : _T("Invalid output file!"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                        MB_OK | MB_ICONERROR);

                    bWorking = false;
                    return;
                }
            }
        }

        if ((dlg.pWorkerContext->szOutPath.Compare(_T("")) != 0) &&
            ((dlg.pWorkerContext->szOutPath.Compare(DEFAULT_TEXT_OUTPUT_PATH) != 0 && this->bMultipleMonoInput == false) ||
            (dlg.pWorkerContext->szOutPath.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0 && this->bMultipleMonoInput == true)))
        {
            if (this->bMultipleMonoInput == false)
            {
                if (util::Utilities::MakeFullPath(dlg.pWorkerContext->szOutPath) == false)
                {
                    OutputDebugString(_T("Error: Failed to create output path!"));
                    this->MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207017).c_str() : _T("Failed to create output path!"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                        MB_OK | MB_ICONERROR);

                    bWorking = false;
                    return;
                }
            }
            else
            {
                CString szTmpOutPath = dlg.pWorkerContext->szOutPath;
                CString szFile = util::Utilities::GetFileName(dlg.pWorkerContext->szOutPath);

                szTmpOutPath.Truncate(szTmpOutPath.GetLength() - szFile.GetLength());
                if (util::Utilities::MakeFullPath(szTmpOutPath) == false)
                {
                    OutputDebugString(_T("Error: Failed to create output path!"));
                    this->MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207017).c_str() : _T("Failed to create output path!"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                        MB_OK | MB_ICONERROR);

                    bWorking = false;
                    return;
                }
            }

            dlg.pWorkerContext->bUseOutPath = true;
        }

        dlg.pWorkerContext->bMultiMonoInput = this->bMultipleMonoInput;
        dlg.pWorkerContext->api = this->api;

        util::CTimeCount countTime;
        CString szText;

        countTime.Start();
        dlg.DoModal();
        countTime.Stop();

        for (int i = listStatus.Count() - 1; i >= 0; i--)
        {
            auto status = listStatus.Get(i);
            if (status == (char)true)
                this->m_LstFiles.DeleteItem(i);
        }

        if (dlg.pWorkerContext->nCount <= 0)
        {
            szText.Format(_T(""));
            OutputDebugString(_T("Error: Failed to encode all files."));
        }
        else
        {
            if (this->bMultipleMonoInput == true)
            {
                szText.Format(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207018).c_str() : _T("Encoded %d mono files in %s (%0.3lf sec)"),
                    dlg.pWorkerContext->nCount,
                    countTime.Format(countTime.ElapsedTime(), 3),
                    countTime.ElapsedTime());
            }
            else
            {
                szText.Format(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207019).c_str() : _T("Encoded %d file%s in %s (%0.3lf sec)"),
                    dlg.pWorkerContext->nCount,
                    dlg.pWorkerContext->nCount == 1 ? _T("") :
                    (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701A).c_str() : _T("s")),
                    countTime.Format(countTime.ElapsedTime(), 3),
                    countTime.ElapsedTime());
            }
        }

        this->m_StatusBar.SetText(szText, 0, 0);
        bWorking = false;
    }

    void CMainDlg::OnBnClickedCheckSimdMmx()
    {
        auto& preset = GetCurrentPreset();
        preset.nUsedSIMD[0] = this->m_ChkSimdMMX.GetCheck() == BST_CHECKED ? 1 : 0;
    }

    void CMainDlg::OnBnClickedCheckSimdSse()
    {
        auto& preset = GetCurrentPreset();
        preset.nUsedSIMD[1] = this->m_ChkSimdSSE.GetCheck() == BST_CHECKED ? 1 : 0;
    }

    void CMainDlg::OnBnClickedCheckSimdSse2()
    {
        auto& preset = GetCurrentPreset();
        preset.nUsedSIMD[2] = this->m_ChkSimdSSE2.GetCheck() == BST_CHECKED ? 1 : 0;
    }

    void CMainDlg::OnBnClickedCheckSimdSse3()
    {
        auto& preset = GetCurrentPreset();
        preset.nUsedSIMD[3] = this->m_ChkSimdSSE3.GetCheck() == BST_CHECKED ? 1 : 0;
    }

    void CMainDlg::OnBnClickedCheckVbr()
    {
        if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
        {
            auto& preset = GetCurrentPreset();
            preset.nMode = AFTEN_ENC_MODE_VBR;
            this->m_SldBitrate.SetRange(0, 1023, TRUE);
            int nNewPos = GetCurrentPreset().nQuality;
            this->m_SldBitrate.SetPos(nNewPos);
        }
        else
        {
            auto& preset = GetCurrentPreset();
            preset.nMode = AFTEN_ENC_MODE_CBR;
            this->m_SldBitrate.SetRange(0, config::CEncoderDefaults::nNumValidCbrBitrates - 1, TRUE);
            int nNewPos = config::CEncoderDefaults::FindValidBitratePos(GetCurrentPreset().nBitrate);
            this->m_SldBitrate.SetPos(nNewPos);
        }

        this->UpdateBitrateText();
    }

    void CMainDlg::OnBnClickedButtonPresetAdd()
    {
        auto preset = GetCurrentPreset();
        static int nCount = 0;

        preset.szName.Format(_T("%s (%d)"),
            m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701B) : _T("New preset"),
            nCount++);
        this->encPresets.Insert(preset);

        this->nCurrentPreset = this->encPresets.Count() - 1;
        this->m_CmbPresets.InsertString(this->nCurrentPreset, preset.szName);
        this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);
    }

    void CMainDlg::OnBnClickedButtonPresetDel()
    {
        if (this->encPresets.Count() >= 2)
        {
            int nCount = this->m_CmbPresets.GetCount();
            int nPreset = this->m_CmbPresets.GetCurSel();

            this->encPresets.Remove(nPreset);
            this->m_CmbPresets.DeleteString(nPreset);

            this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

            if (nPreset == (nCount - 1))
            {
                this->m_CmbPresets.SetCurSel(nCount - 2);
                this->nCurrentPreset = nCount - 2;
            }
            else if (nPreset == 0)
            {
                this->m_CmbPresets.SetCurSel(0);
                this->nCurrentPreset = 0;
            }
            else
            {
                this->m_CmbPresets.SetCurSel(nPreset);
                this->nCurrentPreset = nPreset;
            }

            util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);

            this->OnCbnSelchangeComboPresets();
        }
    }

    void CMainDlg::OnBnClickedButtonBrowse()
    {
        if (this->bMultipleMonoInput == true)
        {
            CFileDialog fd(FALSE,
                config::CEncoderDefaults::szSupportedOutputExt[0],
                _T(""),
                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER,
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701C).c_str() : _T("AC3 Files (*.ac3)|*.ac3|All Files (*.*)|*.*||"),
                this);

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

            if ((lpBuffer = (TCHAR *)pMalloc->Alloc(MAX_PATH * 2)) == nullptr)
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
    #endif

            auto szTitle = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701D).c_str() : _T("Select default output path:");

            bi.hwndOwner = this->GetSafeHwnd();
            bi.pidlRoot = pidlDesktop;
            bi.pszDisplayName = lpBuffer;
            bi.lpszTitle = szTitle;
            bi.lpfn = nullptr;
            bi.lParam = 0;
            bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
            bi.iImage = 0;

            pidlBrowse = ::SHBrowseForFolder(&bi);
            if (pidlBrowse != nullptr)
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

            auto preset = defaultPreset;
            this->encPresets.Set(preset, nPreset);

            this->ApplyPresetToDlg(GetCurrentPreset());
        }
    }

    void CMainDlg::OnBnClickedCheckMultipleMonoInput()
    {
        this->bMultipleMonoInput = this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED ? true : false;

        if (this->bMultipleMonoInput == true)
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020200B).c_str() : _T("Output file:"));
        else
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020200C).c_str() : _T("Output path:"));

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
        dlg.m_EngineList = this->m_EngineList;
        if (dlg.DoModal() == IDOK)
        {
            this->m_EngineList.RemoveAll();
            this->m_CmbEngines.ResetContent();

            this->m_EngineList = dlg.m_EngineList;

            this->UpdateProgramEngines();
            this->OnCbnSelchangeComboEngines();
        }
    }

    void CMainDlg::OnCbnSelchangeComboSetting()
    {
        POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
        if (pos != nullptr)
        {
            int nItem = this->m_LstSettings.GetNextSelectedItem(pos);
            int nVal = this->m_CmbValue.GetCurSel();
            auto& preset = GetCurrentPreset();
            preset.nSetting[nItem] = nVal;
            CString szName = config::CEncoderDefaults::encOpt[nItem].listOptNames.Get(nVal);
            this->m_LstSettings.SetItemText(nItem, 1, szName);
        }
    }

    void CMainDlg::OnCbnSelchangeComboPresets()
    {
        int nPreset = this->m_CmbPresets.GetCurSel();
        if (nPreset != CB_ERR)
        {
            this->nCurrentPreset = nPreset;
            auto& preset = GetCurrentPreset();
            this->ApplyPresetToDlg(preset);
        }
    }

    void CMainDlg::OnCbnSelchangeComboEngines()
    {
        if (this->api.IsAftenOpen())
        {
            this->api.CloseAftenAPI();
        }

        int nSel = this->m_CmbEngines.GetCurSel();
        if (nSel == CB_ERR)
        {
            return;
        }

        auto& preset = GetCurrentPreset();
        preset.nCurrentEngine = nSel;

        if (m_Config.m_bIsPortable == true)
        {
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        }
        else
        {
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());
        }

        if (this->api.IsAftenOpen())
        {
            this->api.CloseAftenAPI();
        }

        this->api.szLibPath = m_EngineList.Get(GetCurrentPreset().nCurrentEngine).szValue;
        if (this->api.OpenAftenAPI() == false)
        {
            CString szLogMessage =
                (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701E).c_str() : _T("Failed to load")) +
                _T(" '") +
                m_EngineList.Get(GetCurrentPreset().nCurrentEngine).szKey +
                _T("' ") +
                (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701F).c_str() : _T("library")) +
                _T("!");
            this->m_StatusBar.SetText(szLogMessage, 0, 0);

            return;
        }
        else
        {
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
    #endif
            CString szLogMessage =
                (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207020).c_str() : _T("Loaded")) +
                _T(" '") +
                m_EngineList.Get(GetCurrentPreset().nCurrentEngine).szKey +
                _T("' ") +
                (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020701F).c_str() : _T("library")) +
                _T(", ") +
                (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207021).c_str() : _T("version")) +
                _T(" ") +
                szAftenVersion;
            this->m_StatusBar.SetText(szLogMessage, 0, 0);
        }
    }

    void CMainDlg::OnCbnSelchangeComboRawSampleFormat()
    {
        auto& preset = GetCurrentPreset();
        preset.nRawSampleFormat = this->m_CmbRawSampleFormat.GetCurSel();
    }

    LRESULT CMainDlg::EditChangeComboPresets(WPARAM wParam, LPARAM lParam)
    {
        if ((lParam == (LPARAM)0) && (wParam != (WPARAM)0))
        {
            int nPreset = this->m_CmbPresets.GetCurSel();
            if (nPreset != CB_ERR)
            {
                CString *szName = (CString *)wParam;
                DWORD dwEditSel = this->m_CmbPresets.GetEditSel();

                auto& preset = GetCurrentPreset();
                preset.szName = *szName;

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
        config::CConfigList m_ConfigList;
        if (config::CConfiguration::LoadConfig(szFileName, m_ConfigList) == true)
        {
            int nSize = m_ConfigList.Count();
            for (int i = 0; i < nSize; i++)
            {
                config::CConfigEntry ce = m_ConfigList.Get(i);

                if (ce.szKey.Compare(_T("MainWindow")) == 0)
                {
                    this->SetWindowRectStr(ce.szValue);
                }
                else if (ce.szKey.Compare(_T("ColumnSizeSettings")) == 0)
                {
                    int nColumn[2] = { 0, 0 };
                    if (_stscanf(ce.szValue, _T("%d %d"),
                        &nColumn[0], &nColumn[1]) == 2)
                    {
                        this->m_LstSettings.SetColumnWidth(0, nColumn[0]);
                        this->m_LstSettings.SetColumnWidth(1, nColumn[1]);
                    }
                }
                else if (ce.szKey.Compare(_T("ColumnSizeFiles")) == 0)
                {
                    int nColumn[2] = { 0, 0 };
                    if (_stscanf(ce.szValue, _T("%d %d"),
                        &nColumn[0], &nColumn[1]) == 2)
                    {
                        this->m_LstFiles.SetColumnWidth(0, nColumn[0]);
                        this->m_LstFiles.SetColumnWidth(1, nColumn[1]);
                    }
                }
                else if (ce.szKey.Compare(_T("OutputPath")) == 0)
                {
                    if (ce.szValue.Compare(_T("")) != 0 && ce.szValue.Compare(DEFAULT_TEXT_OUTPUT_PATH) != 0)
                    {
                        this->szOutputPath = ce.szValue;
                    }
                }
                else if (ce.szKey.Compare(_T("OutputFile")) == 0)
                {
                    if (ce.szValue.Compare(_T("")) != 0 && ce.szValue.Compare(DEFAULT_TEXT_OUTPUT_FILE) != 0)
                    {
                        this->szOutputFile = ce.szValue;
                    }
                }
                else if (ce.szKey.Compare(_T("SelectedPreset")) == 0)
                {
                    int nPreset = 0;
                    if (_stscanf(ce.szValue, _T("%d"), &nPreset) == 1)
                    {
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
                else if (ce.szKey.Compare(_T("MultipleMonoInput")) == 0)
                {
                    if (ce.szValue.Compare(_T("true")) == 0)
                    {
                        this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
                        this->bMultipleMonoInput = true;
                        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020200B).c_str() : _T("Output file:"));
                    }
                    else if (ce.szValue.Compare(_T("false")) == 0)
                    {
                        this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
                        this->bMultipleMonoInput = false;
                        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020200C).c_str() : _T("Output path:"));
                    }
                    else
                    {
                        this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
                        this->bMultipleMonoInput = false;
                        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020200C).c_str() : _T("Output path:"));
                    }
                }
                else if (ce.szKey.Compare(_T("DisableAllWarnings")) == 0)
                {
                    if (ce.szValue.Compare(_T("true")) == 0)
                    {
                        this->bDisableAllWarnings = true;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_CHECKED);
                    }
                    else if (ce.szValue.Compare(_T("false")) == 0)
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
                else if (ce.szKey.Compare(_T("SaveConfig")) == 0)
                {
                    if (ce.szValue.Compare(_T("true")) == 0)
                    {
                        this->bSaveConfig = true;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_CHECKED);
                    }
                    else if (ce.szValue.Compare(_T("false")) == 0)
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
        config::CConfigList m_ConfigList;

        m_ConfigList.RemoveAll();

        config::CConfigEntry mainWindow;
        mainWindow.szKey = _T("MainWindow");
        mainWindow.szValue = this->GetWindowRectStr();
        m_ConfigList.Insert(mainWindow);

        config::CConfigEntry columnSizeSettings;
        columnSizeSettings.szKey = _T("ColumnSizeSettings");
        columnSizeSettings.szValue.Format(_T("%d %d"),
            this->m_LstSettings.GetColumnWidth(0),
            this->m_LstSettings.GetColumnWidth(1));
        m_ConfigList.Insert(columnSizeSettings);

        config::CConfigEntry columnSizeFiles;
        columnSizeFiles.szKey = _T("ColumnSizeFiles");
        columnSizeFiles.szValue.Format(_T("%d %d"),
            this->m_LstFiles.GetColumnWidth(0),
            this->m_LstFiles.GetColumnWidth(1));
        m_ConfigList.Insert(columnSizeFiles);

        config::CConfigEntry outputPath;
        outputPath.szKey = _T("OutputPath");
        outputPath.szValue = this->szOutputPath;
        if (outputPath.szValue.Compare(DEFAULT_TEXT_OUTPUT_PATH) == 0)
            outputPath.szValue = _T("");
        m_ConfigList.Insert(outputPath);

        config::CConfigEntry outputFile;
        outputFile.szKey = _T("OutputFile");
        outputFile.szValue = this->szOutputFile;
        if (outputFile.szValue.Compare(DEFAULT_TEXT_OUTPUT_FILE) == 0)
            outputFile.szValue = _T("");
        m_ConfigList.Insert(outputFile);

        config::CConfigEntry selectedPreset;
        selectedPreset.szKey = _T("SelectedPreset");
        selectedPreset.szValue.Format(_T("%d"), this->m_CmbPresets.GetCurSel());
        m_ConfigList.Insert(selectedPreset);

        config::CConfigEntry multipleMonoInput;
        multipleMonoInput.szKey = _T("MultipleMonoInput");
        multipleMonoInput.szValue = (this->bMultipleMonoInput == true) ? _T("true") : _T("false");
        m_ConfigList.Insert(multipleMonoInput);

        config::CConfigEntry disableAllWarnings;
        disableAllWarnings.szKey = _T("DisableAllWarnings");
        disableAllWarnings.szValue = (this->bDisableAllWarnings == true) ? _T("true") : _T("false");
        m_ConfigList.Insert(disableAllWarnings);

        config::CConfigEntry saveConfig;
        saveConfig.szKey = _T("SaveConfig");
        saveConfig.szValue = (this->bSaveConfig == true) ? _T("true") : _T("false");
        m_ConfigList.Insert(saveConfig);

        return config::CConfiguration::SaveConfig(szFileName, m_ConfigList);
    }

    bool CMainDlg::UpdateProgramEngines()
    {
        if (this->m_EngineList.Count() == 0)
        {
            config::CConfigEntry ce;
            ce.szKey = _T("Aften");
            ce.szValue = _T("libaften.dll");

            this->m_EngineList.RemoveAll();
            this->m_EngineList.Insert(ce);

            auto& preset = GetCurrentPreset();
            preset.nCurrentEngine = 0;

            this->m_CmbEngines.InsertString(0, ce.szKey);
            this->m_CmbEngines.SetCurSel(0);

            if (this->api.IsAftenOpen())
            {
                this->api.CloseAftenAPI();
            }

            this->api.szLibPath = m_EngineList.Get(GetCurrentPreset().nCurrentEngine).szValue;
            this->api.OpenAftenAPI();

            return false;
        }

        int nSize = this->m_EngineList.Count();
        for (int i = 0; i < nSize; i++)
        {
            auto& ce = this->m_EngineList.Get(i);
            this->m_CmbEngines.InsertString(i, ce.szKey);
        }

        if (GetCurrentPreset().nCurrentEngine > nSize)
        {
            auto& preset = GetCurrentPreset();
            preset.nCurrentEngine = 0;
        }

        if ((GetCurrentPreset().nCurrentEngine >= 0) && (GetCurrentPreset().nCurrentEngine < nSize))
        {
            if (this->api.IsAftenOpen())
            {
                this->api.CloseAftenAPI();
            }

            this->api.szLibPath = m_EngineList.Get(GetCurrentPreset().nCurrentEngine).szValue;
            if (this->api.OpenAftenAPI() == false)
            {
                this->m_CmbEngines.SetCurSel(0);

                auto& preset = GetCurrentPreset();
                preset.nCurrentEngine = 0;
            }
            else
            {
                this->m_CmbEngines.SetCurSel(GetCurrentPreset().nCurrentEngine);
            }
        }

        return true;
    }

    bool CMainDlg::LoadProgramEngines(CString szFileName)
    {
        this->m_EngineList.RemoveAll();
        this->m_CmbEngines.ResetContent();

        if (config::CConfiguration::LoadConfig(szFileName, this->m_EngineList) == true)
        {
            return this->UpdateProgramEngines();
        }
        else
        {
            config::CConfigEntry ce;
            ce.szKey = _T("Aften");
            ce.szValue = _T("libaften.dll");

            this->m_EngineList.RemoveAll();
            this->m_EngineList.Insert(ce);

            auto& preset = GetCurrentPreset();
            preset.nCurrentEngine = 0;

            this->m_CmbEngines.InsertString(0, ce.szKey);
            this->m_CmbEngines.SetCurSel(0);

            if (this->api.IsAftenOpen())
            {
                this->api.CloseAftenAPI();
            }

            this->api.szLibPath = m_EngineList.Get(GetCurrentPreset().nCurrentEngine).szValue;
            this->api.OpenAftenAPI();
        }

        return false;
    }

    bool CMainDlg::SaveProgramEngines(CString szFileName)
    {
        return config::CConfiguration::SaveConfig(szFileName, this->m_EngineList);
    }

    bool CMainDlg::LoadFilesList(CString &szFileName)
    {
        util::CListT<CString> fl;
        if (m_Config.LoadFiles(szFileName, fl))
        {
            this->m_LstFiles.DeleteAllItems();

            for (int i = 0; i < fl.Count(); i++)
            {
                CString szPath = fl.Get(i);
                this->AddItemToFileList(szPath);
            }

            return true;
        }
        return false;
    }

    bool CMainDlg::SaveFilesList(CString &szFileName, int nFormat)
    {
        util::CListT<CString> fl;
        int nItems = this->m_LstFiles.GetItemCount();
        for (int i = 0; i < nItems; i++)
        {
            CString szPath = this->m_LstFiles.GetItemText(i, 0);
            fl.Insert(szPath);
        }

        return m_Config.SaveFiles(szFileName, fl, nFormat);
    }

    void CMainDlg::LoadAllConfiguration()
    {
        bool bPresetsRet = config::CEncoderDefaults::LoadEncoderPresets(this->encPresets, m_Config.m_szPresetsFilePath, this->defaultPreset);
        OutputDebugString((bPresetsRet ? _T("Loaded encoder presets: ") : _T("Failed to load encoder presets: ")) + m_Config.m_szPresetsFilePath);

        if (bPresetsRet == true)
        {
            if (encPresets.Count() > 0)
            {
                this->m_CmbPresets.ResetContent();

                for (int i = 0; i < encPresets.Count(); i++)
                {
                    auto& preset = encPresets.Get(i);
                    this->m_CmbPresets.InsertString(i, preset.szName);
                }

                if ((this->nCurrentPreset >= encPresets.Count()) || (this->nCurrentPreset < 0))
                    this->nCurrentPreset = 0;

                this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

                this->OnCbnSelchangeComboPresets();
            }
        }

        bool bConfigRet = this->LoadProgramConfig(m_Config.m_szConfigFilePath);
        OutputDebugString((bConfigRet ? _T("Loaded program config: ") : _T("Failed to load program config: ")) + m_Config.m_szConfigFilePath);

        bool bEnginesRet = this->LoadProgramEngines(m_Config.m_szEnginesFilePath);
        OutputDebugString((bEnginesRet ? _T("Loaded encoder engines: ") : _T("Failed to load encoder engines: ")) + m_Config.m_szEnginesFilePath);

        bool bFilesRet = this->LoadFilesList(m_Config.m_szFilesListFilePath);
        OutputDebugString((bFilesRet ? _T("Loaded files list: ") : _T("Failed to load files list: ")) + m_Config.m_szFilesListFilePath);
    }

    void CMainDlg::SaveAllConfiguration()
    {
        bool bRet = false;

        bRet = config::CEncoderDefaults::SaveEncoderPresets(this->encPresets, m_Config.m_szPresetsFilePath, this->defaultPreset);
        OutputDebugString((bRet ? _T("Saved encoder presets: ") : _T("Error: Failed to save encoder presets: ")) + m_Config.m_szPresetsFilePath);

        bRet = this->SaveProgramConfig(m_Config.m_szConfigFilePath);
        OutputDebugString((bRet ? _T("Saved program config: ") : _T("Error: Failed to save program config: ")) + m_Config.m_szConfigFilePath);

        bRet = this->SaveProgramEngines(m_Config.m_szEnginesFilePath);
        OutputDebugString((bRet ? _T("Saved encoder engines: ") : _T("Error: Failed to save encoder engines: ")) + m_Config.m_szEnginesFilePath);

        bRet = this->SaveFilesList(m_Config.m_szFilesListFilePath, 0);
        OutputDebugString((bRet ? _T("Saved files list: ") : _T("Error: Failed to save files list: ")) + m_Config.m_szFilesListFilePath);
    }

    void CMainDlg::UpdateBitrateText()
    {
        int nCurPos = this->m_SldBitrate.GetPos();
        CString szBuff;

        if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
        {
            if (m_Config.HaveLangStrings())
                m_StcQualityBitrate.SetWindowText(m_Config.GetLangString(0x00202002).c_str());
            else
                m_StcQualityBitrate.SetWindowText(_T("Quality:"));

            szBuff.Format(_T("%d"), nCurPos);

            auto& preset = GetCurrentPreset();
            preset.nQuality = nCurPos;
        }
        else
        {
            if ((nCurPos >= 0) && (nCurPos < config::CEncoderDefaults::nNumValidCbrBitrates))
            {
                if (m_Config.HaveLangStrings())
                    m_StcQualityBitrate.SetWindowText(m_Config.GetLangString(0x00202003).c_str());
                else
                    m_StcQualityBitrate.SetWindowText(_T("Bitrate:"));

                if (nCurPos == 0)
                    szBuff.Format(DEFAULT_TEXT_AUTO);
                else
                    szBuff.Format(_T("%d kbps"), config::CEncoderDefaults::nValidCbrBitrates[nCurPos]);

                auto& preset = GetCurrentPreset();
                preset.nBitrate = config::CEncoderDefaults::nValidCbrBitrates[nCurPos];
            }
        }

        this->m_StcBitrate.SetWindowText(szBuff);
    }

    config::CEncoderPreset& CMainDlg::GetCurrentPreset()
    {
        if (this->encPresets.Count() > 0)
            return this->encPresets.Get(this->nCurrentPreset);
        else
            return defaultPreset;
    }

    void CMainDlg::AddItemToFileList(CString szPath)
    {
        CString szSize = _T("");
        ULARGE_INTEGER ulSize;
        ULONGLONG nFileSize;
        WIN32_FIND_DATA FindFileData;
        HANDLE hFind;

        hFind = ::FindFirstFile(szPath, &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            return;

        ulSize.HighPart = FindFileData.nFileSizeHigh;
        ulSize.LowPart = FindFileData.nFileSizeLow;
        nFileSize = ulSize.QuadPart;

        ::FindClose(hFind);

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

        this->m_LstFiles.SetItemText(nItem, 0, szPath);

        if (util::Utilities::GetFileExtension(szPath).MakeLower() == _T("avs"))
        {
            AvsAudioInfo infoAVS;
            memset(&infoAVS, 0, sizeof(AvsAudioInfo));
            if (GetAvisynthFileInfo(szPath, &infoAVS) == false)
                return;

            nFileSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
        }

        szSize.Format(_T("%I64d"), nFileSize);
        this->m_LstFiles.SetItemText(nItem, 1, szSize);
    }

    void CMainDlg::ApplyPresetToDlg(config::CEncoderPreset &preset)
    {
        for (int i = 0; i < config::CEncoderPreset::nNumEncoderOptions; i++)
        {
            auto nSetting = preset.nSetting[i];
            auto szText = config::CEncoderDefaults::encOpt[i].listOptNames.Get(nSetting);
            this->m_LstSettings.SetItemText(i, 1, szText);
        }

        if (preset.nMode == AFTEN_ENC_MODE_CBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, config::CEncoderDefaults::nNumValidCbrBitrates - 1, TRUE);
            int nPos = config::CEncoderDefaults::FindValidBitratePos(preset.nBitrate);
            this->m_SldBitrate.SetPos(nPos);
            this->m_ChkVbr.SetCheck(BST_UNCHECKED);
        }
        else if (preset.nMode == AFTEN_ENC_MODE_VBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, 1023, TRUE);
            this->m_SldBitrate.SetPos(preset.nQuality);
            this->m_SldBitrate.Invalidate();
            this->m_ChkVbr.SetCheck(BST_CHECKED);
        }

        this->m_ChkSimdMMX.SetCheck(preset.nUsedSIMD[0] == 0 ? BST_UNCHECKED : BST_CHECKED);
        this->m_ChkSimdSSE.SetCheck(preset.nUsedSIMD[1] == 0 ? BST_UNCHECKED : BST_CHECKED);
        this->m_ChkSimdSSE2.SetCheck(preset.nUsedSIMD[2] == 0 ? BST_UNCHECKED : BST_CHECKED);
        this->m_ChkSimdSSE3.SetCheck(preset.nUsedSIMD[3] == 0 ? BST_UNCHECKED : BST_CHECKED);

        if (preset.nThreads == 0)
        {
            this->m_EdtThreads.SetWindowText(DEFAULT_TEXT_AUTO);
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), preset.nThreads);
            this->m_EdtThreads.SetWindowText(szBuff);
        }

        this->m_CmbEngines.SetCurSel(preset.nCurrentEngine);
        this->m_CmbRawSampleFormat.SetCurSel(preset.nRawSampleFormat);

        if (preset.nRawSampleRate == 0)
        {
            this->m_EdtRawSamplerate.SetWindowText(DEFAULT_TEXT_IGNORED);
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), preset.nRawSampleRate);
            this->m_EdtRawSamplerate.SetWindowText(szBuff);
        }

        if (preset.nRawChannels == 0)
        {
            this->m_EdtRawChannels.SetWindowText(DEFAULT_TEXT_IGNORED);
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), preset.nRawChannels);
            this->m_EdtRawChannels.SetWindowText(szBuff);
        }
        this->UpdateBitrateText();

        POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
        if (pos != nullptr)
        {
            int nItem = m_LstSettings.GetNextSelectedItem(pos);

            this->m_LstSettings.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
            this->UpdateSettingsComboBox(nItem);
            this->m_CmbValue.SetCurSel(preset.nSetting[nItem]);
        }
        else
        {
            this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
            this->UpdateSettingsComboBox(0);
            this->m_CmbValue.SetCurSel(preset.nSetting[0]);
        }
    }

    void CMainDlg::HandleDropFiles(HDROP hDropInfo)
    {
        int nCount = ::DragQueryFile(hDropInfo, (UINT)0xFFFFFFFF, nullptr, 0);
        if (nCount > 0)
        {
            for (int i = 0; i < nCount; i++)
            {
                int nReqChars = ::DragQueryFile(hDropInfo, i, nullptr, 0);

                CString szFile;
                ::DragQueryFile(hDropInfo,
                    i,
                    szFile.GetBuffer(nReqChars * 2 + 8),
                    nReqChars * 2 + 8);

                szFile.ReleaseBuffer();

                if (::GetFileAttributes(szFile) & FILE_ATTRIBUTE_DIRECTORY)
                {
                    this->SearchFolderForFiles(szFile, true);
                }
                else
                {
                    CString szExt = ::PathFindExtension(szFile);
                    szExt.MakeLower();
                    szExt.Remove('.');

                    if (config::CEncoderDefaults::IsSupportedInputExt(szExt) == true)
                    {
                        this->AddItemToFileList(szFile);
                    }
                }
            }
        }

        ::DragFinish(hDropInfo);
    }

    void CMainDlg::UpdateSettingsComboBox(int nItem)
    {
        this->m_CmbValue.ResetContent();

        for (int i = 0; i < config::CEncoderDefaults::encOpt[nItem].listOptNames.Count(); i++)
        {
            this->m_CmbValue.AddString(config::CEncoderDefaults::encOpt[nItem].listOptNames.Get(i));
        }

        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING, 15);

        if (this->encPresets.Count() <= 0)
            this->m_CmbValue.SetCurSel(config::CEncoderDefaults::encOpt[nItem].nDefaultValue);
        else
            this->m_CmbValue.SetCurSel(GetCurrentPreset().nSetting[nItem]);
    }

    void CMainDlg::SearchFolderForFiles(CString szPath, const bool bRecurse)
    {
        try
        {
            WIN32_FIND_DATA w32FileData;
            HANDLE hSearch = nullptr;
            BOOL fFinished = FALSE;
            TCHAR cTempBuf[(MAX_PATH * 2) + 1];

            ZeroMemory(&w32FileData, sizeof(WIN32_FIND_DATA));
            ZeroMemory(cTempBuf, MAX_PATH * 2);

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

                    CString szExt = ::PathFindExtension(szTempBuf);
                    szExt.MakeLower();
                    szExt.Remove('.');

                    if (config::CEncoderDefaults::IsSupportedInputExt(szExt) == true)
                    {
                        this->AddItemToFileList(szTempBuf);
                    }
                }

                if (w32FileData.cFileName[0] != '.' &&
                    w32FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    wsprintf(cTempBuf, _T("%s\\%s\0"), szPath, w32FileData.cFileName);
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
            MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020702A).c_str() : _T("Error while searching for files!"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_OK | MB_ICONERROR);
        }
    }

    void CMainDlg::ShowOptionPopup(bool bUseRect)
    {
        POINT point;
        GetCursorPos(&point);

        int nItem;
        POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
        if (pos != nullptr)
            nItem = m_LstSettings.GetNextSelectedItem(pos);
        else
            return;

        if (bUseRect == true)
        {
            CRect rc;
            m_LstSettings.GetItemRect(nItem, rc, LVIR_BOUNDS);
            m_LstSettings.ClientToScreen(rc);
            if (rc.PtInRect(point) == FALSE)
                return;
        }

        CMenu *menu = new CMenu;
        menu->CreatePopupMenu();

        UINT nItemCount = ID_OPTIONS_MENU_START;
        for (int i = 0; i < config::CEncoderDefaults::encOpt[nItem].listOptNames.Count(); i++)
        {
            menu->AppendMenu(MF_STRING, nItemCount, config::CEncoderDefaults::encOpt[nItem].listOptNames.Get(i));
            nItemCount++;
        }

        int nCurSel = this->m_CmbValue.GetCurSel();
        menu->CheckMenuItem(ID_OPTIONS_MENU_START + nCurSel, MF_CHECKED);

        ::SetForegroundWindow(this->GetSafeHwnd());
        int nRet = (int)menu->TrackPopupMenu(TPM_RETURNCMD, point.x, point.y, this, nullptr);
        if (nRet >= ID_OPTIONS_MENU_START)
        {
            this->m_CmbValue.SetCurSel(nRet - ID_OPTIONS_MENU_START);
            this->OnCbnSelchangeComboSetting();
        }

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

            hDDThread = ::CreateThread(nullptr, 0, DragAndDropThread, (LPVOID)&m_DDParam, 0, &dwDDThreadID);
            if (hDDThread == nullptr)
                bHandleDrop = true;
        }

        CMyDialogEx::OnDropFiles(hDropInfo);
    }

    void CMainDlg::OnEnChangeEditRawSampleRate()
    {
        CString szBuff;
        int nPos;

        this->m_EdtRawSamplerate.GetWindowText(szBuff);
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

        auto& preset = GetCurrentPreset();
        preset.nRawSampleRate = nPos;
    }

    void CMainDlg::OnEnChangeEditRawChannels()
    {
        CString szBuff;
        int nPos;

        this->m_EdtRawChannels.GetWindowText(szBuff);

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

        auto& preset = GetCurrentPreset();
        preset.nRawChannels = nPos;
    }

    void CMainDlg::OnEnChangeEditThreads()
    {
        CString szBuff;
        int nPos;

        this->m_EdtThreads.GetWindowText(szBuff);

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

        auto& preset = GetCurrentPreset();
        preset.nThreads = nPos;
    }

    void CMainDlg::OnEnChangeEditOutputPath()
    {
        CString szBuff;
        this->m_EdtOutPath.GetWindowText(szBuff);

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
        CString szDialogTitle = _T("");
        szDialogTitle.Format(_T("WAV to AC3 Encoder %s"), ENCWAVTOAC3_VERSION);
        this->SetWindowText(szDialogTitle);
    }

    void CMainDlg::InitTooltips()
    {
        CString szTmpText;

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206001).c_str() :
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

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206002).c_str() :
            _T("Enable VBR mode. If unchecked the CBR mode is used instead.");

        this->m_ChkVbr.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206003).c_str() :
            _T("Set currently used encoder preset. You can load/save presets from/to file\n")
            _T("from File menu. All presets are automatically loaded/saved from/to text file.");

        this->m_CmbPresets.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206004).c_str() :
            _T("Aften will auto-detect available SIMD instruction sets\n")
            _T("for your CPU, so you shouldn't need to disable sets\n")
            _T("explicitly - unless for speed or debugging reasons.");

        this->m_ChkSimdMMX.SetTooltipText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206005).c_str() + szTmpText :
            _T("This option enables MMX optimizations (if supported by CPU).\n\n") + szTmpText);

        this->m_ChkSimdSSE.SetTooltipText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206006).c_str() + szTmpText :
            _T("This option enables SSE optimizations (if supported by CPU).\n\n") + szTmpText);

        this->m_ChkSimdSSE2.SetTooltipText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206007).c_str() + szTmpText :
            _T("This option enables SSE2 optimizations (if supported by CPU).\n\n") + szTmpText);

        this->m_ChkSimdSSE3.SetTooltipText(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206008.c_str() + szTmpText :
            _T("This option enables SSE3 optimizations (if supported by CPU).\n\n") + szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206009).c_str() :
            _T("Raw audio input sample format specifies the sample format\n")
            _T("when using raw audio input. Using this option forces Aften to\n")
            _T("treat the input as raw audio. The choices for the\n")
            _T("pre-defined sample formats are (default: s16_le):\n")
            _T("u8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,\n")
            _T("s32_le, s32_be, float_le, float_be, double_le, double_be");

        this->m_CmbRawSampleFormat.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020600A).c_str() :
            _T("Raw audio input sample rate option forces Aften to\n")
            _T("treat the input as raw audio (default: 48000).");

        this->m_EdtRawSamplerate.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020600B).c_str() :
            _T("Raw audio input channels forces Aften to treat the input as\n")
            _T("raw audio (default: 2).");

        this->m_EdtRawChannels.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020600C).c_str() :
            _T("Aften can use multiple threads to speed up encoding.\n")
            _T("By default, Aften uses one thread for each logical CPU\n")
            _T("your system has, but you can override this value. A\n")
            _T("value of 0 is the default and indicates that Aften\n")
            _T("should try to detect the number of CPUs.");

        this->m_EdtThreads.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020600D).c_str() :
            _T("Set currently used Aften library. By selecting optimized Aften\n")
            _T("library you can speedup the encoding process.");

        this->m_CmbEngines.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020600E).c_str() :
            _T("Set default output path for encoded files. By default files\n")
            _T("are encoded to the same directory as input files. When using\n")
            _T("multiple mono input than here is set the output file path.");

        this->m_EdtOutPath.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020600F).c_str() :
            _T("Enable multiple mono input mode. By adding multiple mono input\n")
            _T("files to the files list (minimum 2, maximum 6) in correct channel\n")
            _T("order the mono input files will be encoded into single ac3 file.");

        this->m_ChkMultipleMonoInput.SetTooltipText(szTmpText);

        szTmpText = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00206010).c_str() :
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

        for (int i = 0; i < config::CEncoderPreset::nNumEncoderOptions; i++)
        {
            if (config::CEncoderDefaults::encOpt[i].bBeginGroup == true)
                nGroupCounter++;

            if (nGroupCounter >= 0 && nGroupCounter < config::CEncoderDefaults::nNumEncoderOptionsGroups)
            {
                li.pszText = config::CEncoderDefaults::encOpt[i].szName.GetBuffer();
                li.iItem = i;
                li.iSubItem = 0;
                li.iGroupId = 101 + nGroupCounter;

                ListView_InsertItem(listSettings, &li);
                ListView_SetItemText(listSettings, i, 1,
                    config::CEncoderDefaults::encOpt[i].listOptNames.Get(config::CEncoderDefaults::encOpt[i].nDefaultValue).GetBuffer());

                this->m_LstSettings.listTooltips.AddTail(config::CEncoderDefaults::encOpt[i].szHelpText);

                config::CEncoderDefaults::encOpt[i].szName.ReleaseBuffer();
                config::CEncoderDefaults::encOpt[i].listOptNames.Get(config::CEncoderDefaults::encOpt[i].nDefaultValue).ReleaseBuffer();
            }
        }

        this->m_LstSettings.bUseTooltipsList = true;

        this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    }

    void CMainDlg::InitDefaultPreset()
    {
        for (int i = 0; i < config::CEncoderPreset::nNumEncoderOptions; i++)
            defaultPreset.nSetting[i] = config::CEncoderDefaults::encOpt[i].nDefaultValue;

        defaultPreset.szName = DEFAULT_PRESET_NAME;
        defaultPreset.nMode = AFTEN_ENC_MODE_CBR;
        defaultPreset.nBitrate = 0;
        defaultPreset.nQuality = 240;
        defaultPreset.nRawChannels = 0;
        defaultPreset.nRawSampleFormat = 0;
        defaultPreset.nRawSampleRate = 0;
        defaultPreset.nCurrentEngine = 0;
        defaultPreset.nThreads = 0;
        defaultPreset.nUsedSIMD[0] = 1;
        defaultPreset.nUsedSIMD[1] = 1;
        defaultPreset.nUsedSIMD[2] = 1;
        defaultPreset.nUsedSIMD[3] = 1;

        this->nCurrentPreset = 0;
        this->m_CmbPresets.InsertString(0, defaultPreset.szName);
        this->m_CmbPresets.SetCurSel(this->nCurrentPreset);

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

        this->m_ChkSimdMMX.SetCheck(defaultPreset.nUsedSIMD[0] == 1 ? BST_CHECKED : BST_UNCHECKED);
        this->m_ChkSimdSSE.SetCheck(defaultPreset.nUsedSIMD[1] == 1 ? BST_CHECKED : BST_UNCHECKED);
        this->m_ChkSimdSSE2.SetCheck(defaultPreset.nUsedSIMD[2] == 1 ? BST_CHECKED : BST_UNCHECKED);
        this->m_ChkSimdSSE3.SetCheck(defaultPreset.nUsedSIMD[3] == 1 ? BST_CHECKED : BST_UNCHECKED);

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

        if (defaultPreset.nMode == AFTEN_ENC_MODE_CBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, config::CEncoderDefaults::nNumValidCbrBitrates - 1, TRUE);
            this->m_SldBitrate.SetPos(config::CEncoderDefaults::FindValidBitratePos(defaultPreset.nBitrate));
            this->m_ChkVbr.SetCheck(BST_UNCHECKED);
        }
        else if (defaultPreset.nMode == AFTEN_ENC_MODE_VBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, 1023, TRUE);
            this->m_SldBitrate.SetPos(defaultPreset.nQuality);
            this->m_ChkVbr.SetCheck(BST_CHECKED);
        }

        auto preset = defaultPreset;
        encPresets.Insert(preset);
    }

    void CMainDlg::InitRawSamleFormatComboBox()
    {
        CString szIgnored = DEFAULT_TEXT_IGNORED;
        config::CEncoderDefaults::szRawSampleFormats[0] = (LPTSTR)(LPCTSTR)(szIgnored);

        this->m_CmbRawSampleFormat.ResetContent();

        for (int i = 0; i < config::CEncoderDefaults::nNumRawSampleFormats; i++)
            this->m_CmbRawSampleFormat.InsertString(i, config::CEncoderDefaults::szRawSampleFormats[i]);
    }

    void CMainDlg::InitSettingsListGroups()
    {
        LVGROUP lg;
        lg.cbSize = sizeof(LVGROUP);
        lg.mask = LVGF_HEADER | LVGF_GROUPID;

        HWND listView = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();

        ListView_RemoveAllGroups(listView);

        for (int i = 0; i < config::CEncoderDefaults::nNumEncoderOptionsGroups; i++)
        {
            lg.pszHeader = (LPTSTR)(LPCTSTR)(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00208000 + i + 1).c_str() : config::CEncoderDefaults::pszGroups[i]);
            lg.iGroupId = 101 + i;
            ListView_InsertGroup(listView, -1, &lg);
        }
    }

    void CMainDlg::InitDialogControls()
    {
        m_StcQualityBitrate.SetBold(false);
        m_StcBitrate.SetBold(true);
        m_StcSelected.SetBold(false);
        m_StcPreconfigured.SetBold(false);
        m_BtnEncode.SetBold(true);

        VERIFY(m_StatusBar.Create(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
            CRect(0, 0, 0, 0),
            this,
            IDC_STATUSBAR));

        CMFCDynamicLayout* layout = this->GetDynamicLayout();
        layout->AddItem(IDC_STATUSBAR, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeHorizontal(100));

        m_SpnThreads.SetRange32(0, INT_MAX);
        m_SpnThreads.SetPos(0);

        this->m_LstFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        this->m_LstFiles.InsertColumn(0, _T("File path"), LVCFMT_LEFT, 624, 0);
        this->m_LstFiles.InsertColumn(1, _T("File size (bytes)"), LVCFMT_LEFT, 140, 0);

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

        ListView_SetImageList(this->m_LstFiles.GetSafeHwnd(), m_ilLargeTmp, LVSIL_NORMAL);
        ListView_SetImageList(this->m_LstFiles.GetSafeHwnd(), m_ilSmallTmp, LVSIL_SMALL);

        HWND listView = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();
        ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT);
        ListView_EnableGroupView(listView, TRUE);

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

        this->m_SpnRawSampleRate.SetRange32(0, INT_MAX);
        this->m_SpnRawSampleRate.SetPos(0);

        this->m_SpnRawChannels.SetRange32(0, INT_MAX);
        this->m_SpnRawChannels.SetPos(0);

        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
            this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);

        if (this->bMultipleMonoInput == true)
            this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_FILE);
        else
            this->m_EdtOutPath.SetWindowText(DEFAULT_TEXT_OUTPUT_PATH);
    }

    BOOL CMainDlg::OnInitDialog()
    {
        CMyDialogEx::OnInitDialog();

        SetIcon(m_hIcon, TRUE);
        SetIcon(m_hIcon, FALSE);

        m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_MAIN_MENU));

        this->InitTitle();
        this->InitDialogControls();
        this->InitLang(true);
        this->InitDefaultPreset();
        this->UpdateBitrateText();
        this->DragAcceptFiles(TRUE);

        try
        {
            this->LoadAllConfiguration();
        }
        catch (...)
        {
            OutputDebugString(_T("Failed to load configuration."));
        }

        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING, 15);
        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);
        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_RAW_SAMPLE_FORMAT, 15);
        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_ENGINES, 15);

        COMBOBOXINFO cbi;
        ZeroMemory(&cbi, sizeof(COMBOBOXINFO));
        cbi.cbSize = sizeof(COMBOBOXINFO);
        this->m_CmbPresets.GetComboBoxInfo(&cbi);
        this->m_EdtCmbPresetName.SubclassWindow(cbi.hwndItem);

        this->bVisible = true;
        this->ShowWindow(SW_SHOW);

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

        if (m_Config.HaveLangStrings())
        {
            if (initLangMenu == true)
                this->InitLangMenu();

            this->InitLangButtons();
            this->InitLangStaticText();
            this->InitLangFilesList();
            this->InitLangSettingsList();
        }

        config::CEncoderDefaults::InitEncoderOptions();

        this->InitSettingsList();

        if (initLangMenu == false)
        {
            auto& preset = GetCurrentPreset();
            this->ApplyPresetToDlg(preset);
        }

        if (m_Config.HaveLangStrings())
        {
            this->InitLangMainMenu();
        }

        this->InitTooltips();

        CString szBuff = this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath;
        if (szBuff.Compare(_T("")) == 0 || szBuff.Left(1).Compare(_T("<")) == 0)
            this->m_EdtOutPath.SetWindowText(this->bMultipleMonoInput == true ? DEFAULT_TEXT_OUTPUT_FILE : DEFAULT_TEXT_OUTPUT_PATH);
        else
            this->m_EdtOutPath.SetWindowText(this->bMultipleMonoInput == true ? this->szOutputFile : this->szOutputPath);
    }

    void CMainDlg::InitLangButtons()
    {
        m_BtnEncode.SetWindowTextW(m_Config.GetLangString(0x00201001).c_str());
        m_BtnResetCurrent.SetWindowTextW(m_Config.GetLangString(0x00201002).c_str());
        m_BtnRemove.SetWindowTextW(m_Config.GetLangString(0x00201003).c_str());
        m_BtnAddNew.SetWindowTextW(m_Config.GetLangString(0x00201004).c_str());
        m_BtnAddFiles.SetWindowTextW(m_Config.GetLangString(0x00201005).c_str());
        m_BtnBrowse.SetWindowTextW(m_Config.GetLangString(0x00201006).c_str());
        m_BtnMuxWizard.SetWindowTextW(m_Config.GetLangString(0x00201007).c_str());
        m_BtnEngines.SetWindowTextW(m_Config.GetLangString(0x00201008).c_str());
    }

    void CMainDlg::InitLangStaticText()
    {
        this->GetDlgItem(IDC_STATIC_PRESET)->SetWindowTextW(m_Config.GetLangString(0x00202001).c_str());

        if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
            this->GetDlgItem(IDC_STATIC_QUALITY)->SetWindowTextW(m_Config.GetLangString(0x00202002).c_str());
        else
            this->GetDlgItem(IDC_STATIC_QUALITY)->SetWindowTextW(m_Config.GetLangString(0x00202003).c_str());

        this->GetDlgItem(IDC_STATIC_OPTION_VALUE)->SetWindowTextW(m_Config.GetLangString(0x00202004).c_str());
        this->GetDlgItem(IDC_STATIC_SAMPLE_FORMAT)->SetWindowTextW(m_Config.GetLangString(0x00202005).c_str());
        this->GetDlgItem(IDC_STATIC_SAMPLE_RATE)->SetWindowTextW(m_Config.GetLangString(0x00202006).c_str());
        this->GetDlgItem(IDC_STATIC_CHANNELS)->SetWindowTextW(m_Config.GetLangString(0x00202007).c_str());
        this->GetDlgItem(IDC_STATIC_ENGINE)->SetWindowTextW(m_Config.GetLangString(0x00202008).c_str());
        this->GetDlgItem(IDC_STATIC_THREADS)->SetWindowTextW(m_Config.GetLangString(0x00202009).c_str());
        this->GetDlgItem(IDC_CHECK_MULTIPLE_MONO_INPUT)->SetWindowTextW(m_Config.GetLangString(0x0020200A).c_str());

        if (this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED)
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.GetLangString(0x0020200B).c_str());
        else
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(m_Config.GetLangString(0x0020200C).c_str());
    }

    void CMainDlg::InitLangFilesList()
    {
        SetListCtrlColumnText(this->m_LstFiles, 0, m_Config.GetLangString(0x00203001).c_str());
        SetListCtrlColumnText(this->m_LstFiles, 1, m_Config.GetLangString(0x00203002).c_str());
    }

    void CMainDlg::InitLangFilesListContextMenu(CMenu &m_hMenu)
    {
        m_hMenu.ModifyMenuW(0, MF_STRING | MF_BYPOSITION, 0, m_Config.GetLangString(0x00204001).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_ADDFILES, 0, ID_LIST_ADDFILES, m_Config.GetLangString(0x00204002).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_ADDDIRECTORY, 0, ID_LIST_ADDDIRECTORY, m_Config.GetLangString(0x00204003).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_MUXWIZARD, 0, ID_LIST_MUXWIZARD, m_Config.GetLangString(0x00204004).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_LOADLIST, 0, ID_LIST_LOADLIST, m_Config.GetLangString(0x00204005).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_SAVELIST, 0, ID_LIST_SAVELIST, m_Config.GetLangString(0x00204006).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_MOVEUP, 0, ID_LIST_MOVEUP, m_Config.GetLangString(0x00204007).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_MOVEDOWN, 0, ID_LIST_MOVEDOWN, m_Config.GetLangString(0x00204008).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_DELFILES, 0, ID_LIST_DELFILES, m_Config.GetLangString(0x00204009).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_CLEARLIST, 0, ID_LIST_CLEARLIST, m_Config.GetLangString(0x0020400A).c_str());
    }

    void CMainDlg::InitLangSettingsList()
    {
        SetListCtrlColumnText(this->m_LstSettings, 0, m_Config.GetLangString(0x00205001).c_str());
        SetListCtrlColumnText(this->m_LstSettings, 1, m_Config.GetLangString(0x00205002).c_str());
    }

    void CMainDlg::InitLangMainMenu()
    {
        CMenu *m_hMenu = this->GetMenu();

        m_hMenu->ModifyMenuW(0, MF_STRING | MF_BYPOSITION, 0, m_Config.GetLangString(0x00101001).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_ADDFILES, 0, ID_FILE_ADDFILES, m_Config.GetLangString(0x00101002).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_ADDDIRECTORY, 0, ID_FILE_ADDDIRECTORY, m_Config.GetLangString(0x00101003).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_MUXWIZARD, 0, ID_FILE_MUXWIZARD, m_Config.GetLangString(0x00101004).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_LOADFILESLIST, 0, ID_FILE_LOADFILESLIST, m_Config.GetLangString(0x00101005).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_SAVEFILESLIST, 0, ID_FILE_SAVEFILESLIST, m_Config.GetLangString(0x00101006).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_LOADPRESETS, 0, ID_FILE_LOADPRESETS, m_Config.GetLangString(0x00101007).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_SAVEPRESETS, 0, ID_FILE_SAVEPRESETS, m_Config.GetLangString(0x00101008).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_EXIT, 0, ID_FILE_EXIT, m_Config.GetLangString(0x00101009).c_str());

        m_hMenu->ModifyMenuW(1, MF_STRING | MF_BYPOSITION, 1, m_Config.GetLangString(0x00102001).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_DISABLEALLWARNINGS, 0, ID_OPTIONS_DISABLEALLWARNINGS, m_Config.GetLangString(0x00102002).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_SAVECONFIGURATIONONEXIT, 0, ID_OPTIONS_SAVECONFIGURATIONONEXIT, m_Config.GetLangString(0x00102003).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_LOADCONFIGURATION, 0, ID_OPTIONS_LOADCONFIGURATION, m_Config.GetLangString(0x00102004).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_SAVECONFIGURATION, 0, ID_OPTIONS_SAVECONFIGURATION, m_Config.GetLangString(0x00102005).c_str());

        m_hMenu->ModifyMenuW(2, MF_STRING | MF_BYPOSITION, 2, m_Config.GetLangString(0x00103001).c_str());
        m_hMenu->ModifyMenuW(ID_LANGUAGE_DEFAULT, 0, ID_LANGUAGE_DEFAULT, m_Config.GetLangString(0x00103002).c_str());

        m_hMenu->ModifyMenuW(3, MF_STRING | MF_BYPOSITION, 3, m_Config.GetLangString(0x00104001).c_str());
        m_hMenu->ModifyMenuW(ID_HELP_WEBSITE, 0, ID_HELP_WEBSITE, m_Config.GetLangString(0x00104002).c_str());
        m_hMenu->ModifyMenuW(ID_HELP_ABOUT, 0, ID_HELP_ABOUT, m_Config.GetLangString(0x00104003).c_str());

        m_hMenu->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS,
            this->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);

        m_hMenu->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
            this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);

        this->DrawMenuBar();
    }

    void CMainDlg::InitLangMenu()
    {
        if (m_Config.m_LangLst.Count() > 0)
        {
            CMenu *m_hMenu = this->GetMenu();
            CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

            m_hLangMenu->DeleteMenu(ID_LANGUAGE_DEFAULT, 0);

            for (int i = 0; i < m_Config.m_LangLst.Count(); i++)
            {
                auto& lang = m_Config.m_LangLst.Get(i);
                CString szBuff;
                szBuff.Format(_T("%s (%s)"), lang.szEnglishName, lang.szTargetName);
                m_hLangMenu->AppendMenu(MF_STRING, ID_LANGUAGE_MENU_START + i, szBuff);

                if (m_Config.m_nLangId == i)
                    m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_CHECKED);
                else
                    m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);
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
        POSITION pos;
        CString szPath[2] = { _T(""), _T("") };
        CString szSize[2] = { _T(""), _T("") };

        pos = this->m_LstFiles.GetFirstSelectedItemPosition();
        while (pos != nullptr)
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
        POSITION pos;
        CString szPath[2] = { _T(""), _T("") };
        CString szSize[2] = { _T(""), _T("") };
        util::CListT<ItemToMove> listSel;

        pos = this->m_LstFiles.GetFirstSelectedItemPosition();
        while (pos != nullptr)
        {
            int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
            if ((nItem < this->m_LstFiles.GetItemCount() - 1) && (this->m_LstFiles.GetItemCount() >= 2))
            {
                ItemToMove item;

                item.nItem0 = nItem;
                item.nItem1 = nItem + 1;

                listSel.Insert(item);
            }
            else
            {
                return;
            }
        }


        for (int i = listSel.Count() - 1; i >= 0; i--)
        {
            auto& item = listSel.Get(i);

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
        util::CListT<int> list;
        POSITION pos;

        pos = this->m_LstFiles.GetFirstSelectedItemPosition();
        while (pos != nullptr)
        {
            int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
            list.Insert(nItem);
        }

        for (int i = list.Count() - 1; i >= 0; i--)
        {
            this->m_LstFiles.DeleteItem(list.Get(i));
        }
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

        if (pos != nullptr)
        {
            int nItem = m_LstSettings.GetNextSelectedItem(pos);
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

        switch (pLVKeyDow->wVKey)
        {
        case VK_DELETE:
        {
            this->OnListDelFiles();
        }
        break;
        case VK_UP:
        {
            if (GetAsyncKeyState(VK_LMENU) & 0x8001)
                this->OnListMoveUp();
        }
        break;
        case VK_DOWN:
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

        switch (pLVKeyDow->wVKey)
        {
        case VK_LEFT:
        {
            POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
            if (pos != nullptr)
            {
                int nItem = this->m_LstSettings.GetNextSelectedItem(pos);
                int nVal = this->m_CmbValue.GetCurSel();
                if (nVal > 0)
                {
                    nVal = nVal - 1;
                    this->m_CmbValue.SetCurSel(nVal);
                }
                else
                {
                    return;
                }

                auto& preset = GetCurrentPreset();
                preset.nSetting[nItem] = nVal;

                CString szName = config::CEncoderDefaults::encOpt[nItem].listOptNames.Get(nVal);

                this->m_LstSettings.SetItemText(nItem, 1, szName);
            }
        }
        break;
        case VK_RIGHT:
        {
            POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
            if (pos != nullptr)
            {
                int nItem = this->m_LstSettings.GetNextSelectedItem(pos);
                int nVal = this->m_CmbValue.GetCurSel();
                if (nVal < this->m_CmbValue.GetCount() - 1)
                {
                    nVal = nVal + 1;
                    this->m_CmbValue.SetCurSel(nVal);
                }
                else
                {
                    return;
                }

                auto& preset = GetCurrentPreset();
                preset.nSetting[nItem] = nVal;

                CString szName = config::CEncoderDefaults::encOpt[nItem].listOptNames.Get(nVal);
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
        POINT point;
        GetCursorPos(&point);

        CMenu m_hMenu;
        m_hMenu.LoadMenu(IDR_MENU_LIST);
        CMenu *m_hSubMenu = m_hMenu.GetSubMenu(0);
        ::SetForegroundWindow(this->GetSafeHwnd());

        if (m_Config.HaveLangStrings())
            InitLangFilesListContextMenu(m_hMenu);

        m_hSubMenu->TrackPopupMenu(0, point.x, point.y, this, nullptr);

        *pResult = 0;
    }

    void CMainDlg::OnNMRclickListSettings(NMHDR *pNMHDR, LRESULT *pResult)
    {
        this->ShowOptionPopup(true);

        *pResult = 0;
    }

    void CMainDlg::OnNMDblclkListSettings(NMHDR *pNMHDR, LRESULT *pResult)
    {
        POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
        if (pos != nullptr)
        {
            int nItem = m_LstSettings.GetNextSelectedItem(pos);
            this->MessageBox(config::CEncoderDefaults::encOpt[nItem].szHelpText, config::CEncoderDefaults::encOpt[nItem].szName, MB_ICONINFORMATION | MB_OK);
        }

        *pResult = 0;
    }

    bool CMainDlg::GetAvisynthFileInfo(CString szFileName, AvsAudioInfo *pInfoAVS)
    {
        TCHAR *pszInPath = szFileName.GetBuffer();

        if (pInfoAVS == nullptr)
            return false;

        memset(pInfoAVS, 0, sizeof(AvsAudioInfo));

        CAvs2Raw decoderAVS;
        char szInputFileAVS[MAX_PATH] = "";

    #ifdef _UNICODE
        util::Utilities::ConvertUnicodeToAnsi(pszInPath, szInputFileAVS, lstrlen(pszInPath));
        if (decoderAVS.OpenAvisynth(szInputFileAVS) == false)
    #else
        if (decoderAVS.OpenAvisynth(pszInPath) == false)
    #endif
        {
            OutputDebugString(_T("Error: Failed to initialize Avisynth!"));
            this->MessageBox(m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207022).c_str() : _T("Failed to initialize Avisynth"),
                m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207010).c_str() : _T("Error"),
                MB_ICONERROR | MB_OK);

            return false;
        }
        else
        {
            (*pInfoAVS) = decoderAVS.GetInputInfo();
            decoderAVS.CloseAvisynth();
            return true;
        }
    }

    void CMainDlg::OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult)
    {
        POSITION pos = m_LstFiles.GetFirstSelectedItemPosition();
        if (pos != nullptr)
        {
            int nItem = m_LstFiles.GetNextSelectedItem(pos);
            CString szFileName = m_LstFiles.GetItemText(nItem, 0);

            if (util::Utilities::GetFileExtension(szFileName).MakeLower() == _T("avs"))
            {
                AvsAudioInfo infoAVS;
                memset(&infoAVS, 0, sizeof(AvsAudioInfo));
                if (GetAvisynthFileInfo(szFileName, &infoAVS))
                {
                    CString szInfo;
                    TCHAR *pszInPath = szFileName.GetBuffer();

                    szInfo +=
                        (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207023).c_str() : _T("Sample format")) +
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
                            (m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207024).c_str() : _T("unknown")) +
                            _T("\n");
                        break;
                    }

                    CString szBuff;

                    szBuff.Format(_T("%s\t: %d\n"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207025).c_str() : _T("Sample rate"),
                        infoAVS.nSamplesPerSecond);
                    szInfo += szBuff;

                    szBuff.Format(_T("%s\t: %d\n"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207026).c_str() : _T("Channels"),
                        infoAVS.nAudioChannels);
                    szInfo += szBuff;

                    szBuff.Format(_T("%s\t: %I64d\n"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207027).c_str() : _T("Audio samples"),
                        infoAVS.nAudioSamples);
                    szInfo += szBuff;

                    szBuff.Format(_T("%s\t: %I64d"),
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207028).c_str() : _T("Decoded size"),
                        infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels);
                    szInfo += szBuff;

                    szFileName.ReleaseBuffer();

                    this->MessageBox(szInfo,
                        m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207029).c_str() : _T("AVS File Properties"),
                        MB_ICONINFORMATION | MB_OK);
                }
            }
        }

        *pResult = 0;
    }

    void CMainDlg::OnFileAddFiles()
    {
        TCHAR *pFiles = nullptr;
        const DWORD dwMaxSize = (4096 * MAX_PATH);
        try
        {
            pFiles = (TCHAR *)malloc(dwMaxSize);
            if (pFiles == nullptr)
            {
                MessageBox(
                    m_Config.HaveLangStrings() ? m_Config.GetLangString(0x00207009).c_str() : _T("Failed to allocate memory for filenames buffer!"),
                    m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700A).c_str() : _T("Fatal Error"),
                    MB_OK | MB_ICONERROR);
                return;
            }

            ZeroMemory(pFiles, dwMaxSize);

            CString szFilter = config::CEncoderDefaults::GetSupportedInputFilesFilter();
            CFileDialog fd(TRUE,
                config::CEncoderDefaults::szSupportedInputExt[0],
                _T(""),
                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT,
                szFilter,
                this);

            fd.m_ofn.lpstrFile = pFiles;
            fd.m_ofn.nMaxFile = (dwMaxSize) / 2;

            if (fd.DoModal() == IDOK)
            {
                POSITION pos = fd.GetStartPosition();
                while (pos != nullptr)
                {
                    this->AddItemToFileList(fd.GetNextPathName(pos));
                }
            }
        }
        catch (...)
        {
            if (pFiles != nullptr)
            {
                free(pFiles);
                pFiles = nullptr;
            }
        }

        if (pFiles != nullptr)
        {
            free(pFiles);
            pFiles = nullptr;
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

        if ((lpBuffer = (TCHAR *)pMalloc->Alloc(MAX_PATH * 2)) == nullptr)
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
    #endif

        auto szTitle = m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700B).c_str() : _T("Add directory with supported input files:");

        bi.hwndOwner = this->GetSafeHwnd();
        bi.pidlRoot = pidlDesktop;
        bi.pszDisplayName = lpBuffer;
        bi.lpszTitle = szTitle;
        bi.lpfn = nullptr;
        bi.lParam = 0;
        bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
        bi.iImage = 0;

        pidlBrowse = ::SHBrowseForFolder(&bi);
        if (pidlBrowse != nullptr)
        {
            if (SHGetPathFromIDList(pidlBrowse, lpBuffer))
            {
                CString szBuff;
                szBuff.Format(_T("%s\0"), lpBuffer);
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

        int nItemsCount = this->m_LstFiles.GetItemCount();
        if (nItemsCount > config::CEncoderDefaults::nNumMaxInputFiles)
            nItemsCount = config::CEncoderDefaults::nNumMaxInputFiles;

        for (int i = 0; i < nItemsCount; i++)
        {
            dlg.szTmpInputFiles[i] = this->m_LstFiles.GetItemText(i, 0);
        }

        int nIndexAcmod = config::CEncoderDefaults::FindOptionIndex(_T("acmod"));
        int nIndexLfe = config::CEncoderDefaults::FindOptionIndex(_T("lfe"));
        int nIndexChconfig = config::CEncoderDefaults::FindOptionIndex(_T("chconfig"));
        bool bUpdateChconfig = false;

        auto& preset = GetCurrentPreset();

        if (config::CEncoderDefaults::encOpt[nIndexChconfig].nIgnoreValue != preset.nSetting[nIndexChconfig])
        {
            dlg.nChannelConfig = config::CEncoderDefaults::ccAften[config::CEncoderDefaults::encOpt[nIndexChconfig].listOptValues.Get(preset.nSetting[nIndexChconfig])].acmod;
            dlg.bLFE = (config::CEncoderDefaults::ccAften[config::CEncoderDefaults::encOpt[nIndexChconfig].listOptValues.Get(preset.nSetting[nIndexChconfig])].lfe == 1) ? true : false;
            bUpdateChconfig = true;
        }
        else
        {
            if (config::CEncoderDefaults::encOpt[nIndexAcmod].nIgnoreValue != preset.nSetting[nIndexAcmod])
            {
                dlg.nChannelConfig = preset.nSetting[nIndexAcmod];
            }
            else
            {
                int nDefault = config::CEncoderDefaults::encOpt[nIndexAcmod].listOptValues.Count() - 2;
                dlg.nChannelConfig = config::CEncoderDefaults::encOpt[nIndexAcmod].listOptValues.Get(nDefault);
            }

            dlg.bLFE = (preset.nSetting[nIndexLfe] == 1) ? true : false;
            bUpdateChconfig = false;
        }

        if (dlg.DoModal() == IDOK)
        {
            int nRet = IDYES;

            if (this->bDisableAllWarnings == false)
            {
                nRet = this->MessageBox(
                    m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700C).c_str() :
                    _T("Remove all files from list and add files selected in MUX Wizard?\n\n")
                    _T("Note: The channel configuration for current preset will be adjusted."),
                    m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700D).c_str() : _T("MUX Wizard"),
                    MB_YESNO | MB_ICONQUESTION);
            }

            if (nRet == IDYES)
            {
                this->m_LstFiles.DeleteAllItems();

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

                preset.nSetting[nIndexAcmod] = (bUpdateChconfig == true) ? config::CEncoderDefaults::encOpt[nIndexAcmod].nIgnoreValue : dlg.nChannelConfig;

                this->m_LstSettings.SetItemText(nIndexAcmod, 1,
                    config::CEncoderDefaults::encOpt[nIndexAcmod].listOptNames.Get(preset.nSetting[nIndexAcmod]));

                preset.nSetting[nIndexLfe] = (bUpdateChconfig == true) ? config::CEncoderDefaults::encOpt[nIndexLfe].nIgnoreValue : ((dlg.bLFE == true) ? 1 : 0);

                this->m_LstSettings.SetItemText(nIndexLfe, 1,
                    config::CEncoderDefaults::encOpt[nIndexLfe].listOptNames.Get(preset.nSetting[nIndexLfe]));

                if (bUpdateChconfig == true)
                {
                    int acmod = dlg.nChannelConfig;
                    int lfe = (dlg.bLFE == true) ? 1 : 0;

                    for (int i = 0; i < config::CEncoderDefaults::nNumChannelConfigAften; i++)
                    {
                        if ((config::CEncoderDefaults::ccAften[i].acmod == acmod) && (config::CEncoderDefaults::ccAften[i].lfe == lfe))
                        {
                            preset.nSetting[nIndexChconfig] = i;
                            break;
                        }
                    }
                }
                else
                {
                    preset.nSetting[nIndexChconfig] = config::CEncoderDefaults::encOpt[nIndexChconfig].nIgnoreValue;
                }
                this->m_LstSettings.SetItemText(nIndexChconfig, 1,
                    config::CEncoderDefaults::encOpt[nIndexChconfig].listOptNames.Get(preset.nSetting[nIndexChconfig]));

                if (this->bMultipleMonoInput == false)
                {
                    this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
                    this->OnBnClickedCheckMultipleMonoInput();
                }
            }
        }
    }

    void CMainDlg::OnFileLoadFilesList()
    {
        CFileDialog fd(TRUE,
            _T("files"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700E).c_str() : _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"),
            this);

        if (fd.DoModal() == IDOK)
        {
            CString szFileName = fd.GetPathName();
            LoadFilesList(szFileName);
        }
    }

    void CMainDlg::OnFileSaveFilesList()
    {
        CFileDialog fd(FALSE,
            _T("files"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700E).c_str() : _T("Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||"),
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
            m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700F).c_str() : _T("Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||"),
            this);

        if (fd.DoModal() == IDOK)
        {
            CString szFileName = fd.GetPathName();
            if (config::CEncoderDefaults::LoadEncoderPresets(this->encPresets, szFileName, this->defaultPreset) == true)
            {
                this->m_CmbPresets.ResetContent();

                for (int i = 0; i < encPresets.Count(); i++)
                {
                    this->m_CmbPresets.AddString(encPresets.Get(i).szName);
                }

                util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);

                this->nCurrentPreset = 0;
                this->m_CmbPresets.SetCurSel(0);

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
            m_Config.HaveLangStrings() ? m_Config.GetLangString(0x0020700F).c_str() : _T("Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||"),
            this);

        if (fd.DoModal() == IDOK)
        {
            CString szFileName = fd.GetPathName();
            config::CEncoderDefaults::SaveEncoderPresets(this->encPresets, szFileName, this->defaultPreset);
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
        try
        {
            this->LoadAllConfiguration();
        }
        catch (...)
        {
            OutputDebugString(_T("Failed to load configuration."));
        }
    }

    void CMainDlg::OnOptionsSaveConfiguration()
    {
        try
        {
            this->SaveAllConfiguration();
        }
        catch (...)
        {
            OutputDebugString(_T("Failed to save configuration."));
        }
    }

    void CMainDlg::OnLanguageChangeDefault()
    {
        m_Config.m_nLangId = -1;
        m_Config.m_bHaveLang = FALSE;
        m_Config.m_Lang = nullptr;
        m_Config.m_szLangFileName = _T("");

        CMenu *m_hMenu = this->GetMenu();
        CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

        m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_CHECKED);

        for (int i = 0; i < m_Config.m_LangLst.Count(); i++)
        {
            m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);
        }

        InitLang(false);
    }

    void CMainDlg::OnLanguageChange(UINT nID)
    {
        int nLangId = nID - ID_LANGUAGE_MENU_START;
        if (nLangId >= 0 && nLangId < m_Config.m_LangLst.Count())
        {
            auto& lang = m_Config.m_LangLst.Get(nLangId);
            m_Config.m_nLangId = nID - ID_LANGUAGE_MENU_START;
            m_Config.m_bHaveLang = TRUE;
            m_Config.m_Lang = &lang.lm;
            m_Config.m_szLangFileName = lang.szFileName;
        }
        CMenu *m_hMenu = this->GetMenu();
        CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

        m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_UNCHECKED);

        for (int i = 0; i < m_Config.m_LangLst.Count(); i++)
        {
            auto& lang = m_Config.m_LangLst.Get(i);
            if (m_Config.m_nLangId == i)
                m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_CHECKED);
            else
                m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);
        }

        this->InitLang(false);
    }

    void CMainDlg::OnHelpWebsite()
    {
        util::Utilities::LaunchAndWait(ENCWAVTOAC3_URL_HOME, _T(""), FALSE);
    }

    void CMainDlg::OnHelpAbout()
    {
        CAboutDlg dlg;
        dlg.DoModal();
    }
}
