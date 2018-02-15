#include "StdAfx.h"
#include "MainApp.h"
#include "MainDlg.h"
#include "WorkDlg.h"
#include "MuxDlg.h"
#include "EnginesDlg.h"
#include "AboutDlg.h"
#include "utilities\Utilities.h"
#include "worker\TimeCount.h"

namespace app
{
    IMPLEMENT_DYNAMIC(CMainDlg, CDialog)
    CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
        : CMyDialogEx(CMainDlg::IDD, pParent)
    {
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

        this->nSortOrder[0] = true;
        this->nSortOrder[1] = true;
        this->bVisible = false;
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
                szBuff = this->pConfig->GetString(0x00207002).c_str();
            else
                szBuff.Format(_T("%d"), nPos);

            this->m_EdtThreads.SetWindowText(szBuff);
        }
        if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_SAMPLE_RATE)
        {
            CString szBuff;
            if (nPos == 0)
                szBuff = this->pConfig->GetString(0x00207003).c_str();
            else
                szBuff.Format(_T("%d"), nPos);

            this->m_EdtRawSamplerate.SetWindowText(szBuff);
        }
        if (pScrollBar->GetDlgCtrlID() == IDC_SPIN_RAW_CHANNELS)
        {
            CString szBuff;
            if (nPos == 0)
                szBuff = this->pConfig->GetString(0x00207003).c_str();
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
        if (this->pConfig->bSaveConfig == true)
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

        if (this->pConfig->m_bIsPortable == true)
            ::SetCurrentDirectory(util::Utilities::GetExeFilePath().c_str());
        else
            ::SetCurrentDirectory(util::Utilities::GetSettingsFilePath(_T(""), DIRECTORY_CONFIG).c_str());

        int nItemsCount = this->m_LstFiles.GetItemCount();
        if (nItemsCount <= 0)
        {
            OutputDebugString(_T("Error: Add at least one file to the file list!"));
            MessageBox(this->pConfig->GetString(0x00207011).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_ICONERROR | MB_OK);
            return;
        }

        if ((this->pConfig->bMultipleMonoInput == true) && (nItemsCount < 1 || nItemsCount > 6))
        {
            OutputDebugString(_T("Error: Supported are minimum 1 and maximum 6 mono input files!"));
            MessageBox(this->pConfig->GetString(0x00207012).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_ICONERROR | MB_OK);
            return;
        }

        bWorking = true;

        if (this->api.IsAftenOpen())
            this->api.CloseAftenAPI();

        if (this->api.OpenAftenAPI() == false)
        {
            OutputDebugString(_T("Error: Failed to load libaften.dll dynamic library!"));
            MessageBox(this->pConfig->GetString(0x00207013).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_ICONERROR | MB_OK);
            bWorking = false;
            return;
        }

        CWorkDlg dlg;
        dlg.pConfig = this->pConfig;

        dlg.pWorkerContext = std::make_unique<CWorkDlgWorkerContext>(this->pConfig, &dlg);
        dlg.pWorkerContext->bTerminate = false;
        dlg.pWorkerContext->bCanUpdateWindow = true;
        dlg.pWorkerContext->nCount = 0;
        dlg.pWorkerContext->m_ElapsedTimeFile = 0;
        dlg.pWorkerContext->m_ElapsedTimeTotal = 0;

        CString szSizeBuff;
        bool bAvisynthInput = false;

        dlg.pWorkerContext->nTotalSize = 0;
        for (int i = 0; i < nItemsCount; i++)
        {
            std::wstring szFileBuffer = this->m_LstFiles.GetItemText(i, 0);
            std::wstring szExt = util::Utilities::GetFileExtension(szFileBuffer);
            if (util::StringHelper::TowLower(szExt) == L"avs")
                bAvisynthInput = true;

            dlg.pWorkerContext->m_Files.emplace_back(szFileBuffer);
            dlg.pWorkerContext->m_Status.emplace_back(false);

            szSizeBuff = this->m_LstFiles.GetItemText(i, 1);
            dlg.pWorkerContext->nTotalSize += _ttoi64(szSizeBuff);
        }

        if ((this->pConfig->bMultipleMonoInput == true) && (bAvisynthInput == true))
        {
            OutputDebugString(_T("Error: Disable 'Multiple mono input' mode in order to use Avisynth scripts!"));
            MessageBox(this->pConfig->GetString(0x00207014).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_ICONERROR | MB_OK);
            bWorking = false;
            return;
        }

        dlg.pWorkerContext->pPreset = &this->GetCurrentPreset();

        CString szOutPath;
        this->m_EdtOutPath.GetWindowText(szOutPath);
        dlg.pWorkerContext->szOutPath = szOutPath;
        dlg.pWorkerContext->bUseOutPath = false;

        int nLen = dlg.pWorkerContext->szOutPath.length();
        if (nLen < 3)
        {
            OutputDebugString(_T("Error: Invalid output path!"));
            this->MessageBox(this->pConfig->GetString(0x00207015).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_OK | MB_ICONERROR);
            bWorking = false;
            return;
        }

        std::wstring szExt = dlg.pWorkerContext->szOutPath.substr(dlg.pWorkerContext->szOutPath.length() - 4, 4);
        if (this->pConfig->bMultipleMonoInput == true)
        {
            if (dlg.pWorkerContext->szOutPath != this->pConfig->GetString(0x00207005).c_str())
            {
                if ((nLen < 4) || (!util::StringHelper::CompareNoCase(szExt, L".ac3")))
                {
                    OutputDebugString(_T("Error: Invalid output file!"));
                    this->MessageBox(this->pConfig->GetString(0x00207016).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_OK | MB_ICONERROR);
                    bWorking = false;
                    return;
                }
            }
        }

        if ((!dlg.pWorkerContext->szOutPath.empty()) &&
            ((dlg.pWorkerContext->szOutPath != this->pConfig->GetString(0x00207004).c_str() && this->pConfig->bMultipleMonoInput == false) ||
            (dlg.pWorkerContext->szOutPath != this->pConfig->GetString(0x00207005).c_str() && this->pConfig->bMultipleMonoInput == true)))
        {
            if (this->pConfig->bMultipleMonoInput == false)
            {
                if (util::Utilities::MakeFullPath(dlg.pWorkerContext->szOutPath) == false)
                {
                    OutputDebugString(_T("Error: Failed to create output path!"));
                    this->MessageBox(this->pConfig->GetString(0x00207017).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_OK | MB_ICONERROR);
                    bWorking = false;
                    return;
                }
            }
            else
            {
                std::wstring szFile = util::Utilities::GetFileName(dlg.pWorkerContext->szOutPath);
                std::wstring szOutPath = dlg.pWorkerContext->szOutPath.substr(0, dlg.pWorkerContext->szOutPath.length() - szFile.length());
                if (util::Utilities::MakeFullPath(szOutPath) == false)
                {
                    OutputDebugString(_T("Error: Failed to create output path!"));
                    this->MessageBox(this->pConfig->GetString(0x00207017).c_str(), this->pConfig->GetString(0x00207010).c_str(), MB_OK | MB_ICONERROR);
                    bWorking = false;
                    return;
                }
            }

            dlg.pWorkerContext->bUseOutPath = true;
        }

        dlg.pWorkerContext->bMultiMonoInput = this->pConfig->bMultipleMonoInput;
        dlg.pWorkerContext->api = this->api;

        worker::CTimeCount countTime;
        CString szText;

        countTime.Start();
        dlg.DoModal();
        countTime.Stop();

        dlg.pWorkerContext->Wait(1000);

        std::wstring szElapsedFormatted = countTime.Formatted();
        double szElapsedSeconds = countTime.ElapsedMilliseconds() / 1000.0f;

        for (int i = (int)dlg.pWorkerContext->m_Status.size() - 1; i >= 0; i--)
        {
            if (dlg.pWorkerContext->m_Status[i]  == true)
                this->m_LstFiles.DeleteItem(i);
        }

        if (dlg.pWorkerContext->nCount <= 0)
        {
            szText = _T("");
            OutputDebugString(_T("Error: Failed to encode all files."));
        }
        else
        {
            if (this->pConfig->bMultipleMonoInput == true)
            {
                szText.Format(this->pConfig->GetString(0x00207018).c_str(),
                    dlg.pWorkerContext->nCount,
                    szElapsedFormatted.c_str(),
                    szElapsedSeconds);
            }
            else
            {
                szText.Format(this->pConfig->GetString(0x00207019).c_str(),
                    dlg.pWorkerContext->nCount,
                    dlg.pWorkerContext->nCount == 1 ? _T("") :
                    this->pConfig->GetString(0x0020701A).c_str(),
                    szElapsedFormatted.c_str(),
                    szElapsedSeconds);
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
            this->m_SldBitrate.SetRange(0, (int)this->pConfig->m_EncoderOptions.nValidCbrBitrates.size() - 1, TRUE);
            int nNewPos = this->pConfig->FindValidBitratePos(GetCurrentPreset().nBitrate);
            this->m_SldBitrate.SetPos(nNewPos);
        }

        this->UpdateBitrateText();
    }

    void CMainDlg::OnBnClickedButtonPresetAdd()
    {
        static int nCount = 0;
        auto preset = GetCurrentPreset();
        nCount++;
        preset.szName = this->pConfig->GetString(0x0020701B) + L" (" + std::to_wstring(nCount) + L")";
        this->pConfig->m_Presets.emplace_back(preset);

        this->pConfig->nCurrentPreset = (int)(this->pConfig->m_Presets.size() - 1);
        this->m_CmbPresets.InsertString(this->pConfig->nCurrentPreset, preset.szName.c_str());
        this->m_CmbPresets.SetCurSel(this->pConfig->nCurrentPreset);

        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);
    }

    void CMainDlg::OnBnClickedButtonPresetDel()
    {
        if (this->pConfig->m_Presets.size() >= 2)
        {
            int nCount = this->m_CmbPresets.GetCount();
            int nPreset = this->m_CmbPresets.GetCurSel();

            this->pConfig->m_Presets.erase(this->pConfig->m_Presets.begin() + nPreset);
            this->m_CmbPresets.DeleteString(nPreset);
            this->m_CmbPresets.SetCurSel(this->pConfig->nCurrentPreset);

            if (nPreset == (nCount - 1))
            {
                this->m_CmbPresets.SetCurSel(nCount - 2);
                this->pConfig->nCurrentPreset = nCount - 2;
            }
            else if (nPreset == 0)
            {
                this->m_CmbPresets.SetCurSel(0);
                this->pConfig->nCurrentPreset = 0;
            }
            else
            {
                this->m_CmbPresets.SetCurSel(nPreset);
                this->pConfig->nCurrentPreset = nPreset;
            }

            util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);

            this->OnCbnSelchangeComboPresets();
        }
    }

    void CMainDlg::OnBnClickedButtonBrowse()
    {
        if (this->pConfig->bMultipleMonoInput == true)
        {
            CFileDialog fd(FALSE,
                this->pConfig->m_EncoderOptions.szSupportedOutputExt[0].c_str(),
                _T(""),
                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER,
                this->pConfig->GetString(0x0020701C).c_str(),
                this);

            if (fd.DoModal() == IDOK)
            {
                std::wstring szFileName = fd.GetPathName();
                this->m_EdtOutPath.SetWindowText(szFileName.c_str());
                this->pConfig->szOutputFile = szFileName;
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

            auto szTitle = this->pConfig->GetString(0x0020701D).c_str();

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
                    this->pConfig->szOutputPath = szBuff;
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
            this->pConfig->nCurrentPreset = nPreset;

            auto preset = pConfig->m_DefaultPreset;
            this->pConfig->m_Presets[nPreset] = preset;

            this->ApplyPresetToDlg(GetCurrentPreset());
        }
    }

    void CMainDlg::OnBnClickedCheckMultipleMonoInput()
    {
        this->pConfig->bMultipleMonoInput = this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED ? true : false;

        if (this->pConfig->bMultipleMonoInput == true)
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200B).c_str());
        else
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200C).c_str());

        std::wstring szBuff = this->pConfig->bMultipleMonoInput == true ? this->pConfig->szOutputFile : this->pConfig->szOutputPath;

        if (szBuff.empty() || szBuff == this->pConfig->GetString(0x00207004).c_str() || szBuff == this->pConfig->GetString(0x00207005).c_str())
        {
            this->m_EdtOutPath.SetWindowText(this->pConfig->bMultipleMonoInput == true ? 
                this->pConfig->GetString(0x00207005).c_str() : this->pConfig->GetString(0x00207004).c_str());
        }
        else
        {
            this->m_EdtOutPath.SetWindowText(this->pConfig->bMultipleMonoInput == true ? this->pConfig->szOutputFile.c_str() : this->pConfig->szOutputPath.c_str());
        }
    }

    void CMainDlg::OnBnClickedButtonMuxWizard()
    {
        this->OnFileMuxWizard();
    }

    void CMainDlg::OnBnClickedButtonEngines()
    {
        CEnginesDlg dlg;
        dlg.pConfig = this->pConfig;
        dlg.nCurrSel = this->m_CmbEngines.GetCurSel();
        dlg.m_Engines = this->pConfig->m_Engines;
        if (dlg.DoModal() == IDOK)
        {
            this->pConfig->m_Engines.clear();
            this->m_CmbEngines.ResetContent();

            this->pConfig->m_Engines = dlg.m_Engines;

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
            preset.nOptions[nItem] = nVal;
            auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];
            std::wstring szName = option.m_Values[nVal].first;
            this->m_LstSettings.SetItemText(nItem, 1, szName.c_str());
        }
    }

    void CMainDlg::OnCbnSelchangeComboPresets()
    {
        int nPreset = this->m_CmbPresets.GetCurSel();
        if (nPreset != CB_ERR)
        {
            this->pConfig->nCurrentPreset = nPreset;
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

        if (this->pConfig->m_bIsPortable == true)
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

        this->api.szLibPath = pConfig->m_Engines[GetCurrentPreset().nCurrentEngine].second;
        if (this->api.OpenAftenAPI() == false)
        {
            std::wstring szLogMessage =
                this->pConfig->GetString(0x0020701E) +
                L" '" + pConfig->m_Engines[GetCurrentPreset().nCurrentEngine].first + L"' " +
                this->pConfig->GetString(0x0020701F) + L"!";
            this->m_StatusBar.SetText(szLogMessage.c_str() , 0, 0);
        }
        else
        {
            const char *szAftenVersionAnsi = this->api.LibAften_aften_get_version();
            int nVersionLen = strlen(szAftenVersionAnsi);
            TCHAR szAftenVersion[256] = _T("");
            ZeroMemory(szAftenVersion, 256 * sizeof(TCHAR));
            int nChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szAftenVersionAnsi, nVersionLen, szAftenVersion, 256);
            if (nChars == 0)
                _stprintf(szAftenVersion, _T("?.??"));

            std::wstring szLogMessage =
                this->pConfig->GetString(0x00207020) +
                L" '" + pConfig->m_Engines[GetCurrentPreset().nCurrentEngine].first + L"' " +
                this->pConfig->GetString(0x0020701F) + L", " +
                this->pConfig->GetString(0x00207021) +
                L" " + szAftenVersion;
            this->m_StatusBar.SetText(szLogMessage.c_str(), 0, 0);
        }
    }

    void CMainDlg::OnCbnSelchangeComboRawSampleFormat()
    {
        auto& preset = GetCurrentPreset();
        preset.m_RawInput.nRawSampleFormat = this->m_CmbRawSampleFormat.GetCurSel();
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

    bool CMainDlg::LoadProgramConfig(std::wstring szFileName)
    {
        std::vector<config::Entry> cl;
        if (this->pConfig->LoadConfig(szFileName, cl) == true)
        {
            int nSize = (int)cl.size();
            for (int i = 0; i < nSize; i++)
            {
                auto& ce = cl[i];

                if (ce.first == L"MainWindow")
                {
                    this->SetWindowRectStr(ce.second.c_str());
                }
                else if (ce.first == L"ColumnSizeSettings")
                {
                    if (!ce.second.empty())
                    {
                        auto widths = util::StringHelper::Split(ce.second.c_str(), ' ');
                        if (widths.size() == 2)
                        {
                            for (int i = 0; i < 2; i++)
                            {
                                int nWidth = util::StringHelper::ToInt(widths[i]);
                                this->m_LstSettings.SetColumnWidth(i, nWidth);
                            }
                        }
                    }
                }
                else if (ce.first == L"ColumnSizeFiles")
                {
                    auto widths = util::StringHelper::Split(ce.second.c_str(), ' ');
                    if (widths.size() == 2)
                    {
                        for (int i = 0; i < 2; i++)
                        {
                            int nWidth = util::StringHelper::ToInt(widths[i]);
                            this->m_LstFiles.SetColumnWidth(i, nWidth);
                        }
                    }
                }
                else if (ce.first == L"OutputPath")
                {
                    if (!ce.second.empty() && ce.second != this->pConfig->GetString(0x00207004).c_str())
                    {
                        this->pConfig->szOutputPath = ce.second;
                    }
                }
                else if (ce.first == L"OutputFile")
                {
                    if (!ce.second.empty() && ce.second != this->pConfig->GetString(0x00207005).c_str())
                    {
                        this->pConfig->szOutputFile = ce.second;
                    }
                }
                else if (ce.first == L"SelectedPreset")
                {
                    int nPreset = util::StringHelper::ToInt(ce.second);
                    {
                        if ((nPreset >= this->m_CmbPresets.GetCount()) || (nPreset < 0))
                            nPreset = 0;

                        if (this->pConfig->nCurrentPreset != nPreset)
                        {
                            this->pConfig->nCurrentPreset = nPreset;
                            this->m_CmbPresets.SetCurSel(nPreset);
                            this->OnCbnSelchangeComboPresets();
                        }
                    }
                }
                else if (ce.first == L"MultipleMonoInput")
                {
                    if (ce.second == L"true")
                    {
                        this->m_ChkMultipleMonoInput.SetCheck(BST_CHECKED);
                        this->pConfig->bMultipleMonoInput = true;
                        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200B).c_str());
                    }
                    else if (ce.second == L"false")
                    {
                        this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
                        this->pConfig->bMultipleMonoInput = false;
                        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200C).c_str());
                    }
                    else
                    {
                        this->m_ChkMultipleMonoInput.SetCheck(BST_UNCHECKED);
                        this->pConfig->bMultipleMonoInput = false;
                        this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200C).c_str());
                    }
                }
                else if (ce.first == L"DisableAllWarnings")
                {
                    if (ce.second == L"true")
                    {
                        this->pConfig->bDisableAllWarnings = true;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_CHECKED);
                    }
                    else if (ce.second == L"false")
                    {
                        this->pConfig->bDisableAllWarnings = false;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_UNCHECKED);
                    }
                    else
                    {
                        this->pConfig->bDisableAllWarnings = false;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS, MF_UNCHECKED);
                    }
                }
                else if (ce.first == L"SaveConfig")
                {
                    if (ce.second == L"true")
                    {
                        this->pConfig->bSaveConfig = true;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_CHECKED);
                    }
                    else if (ce.second == L"false")
                    {
                        this->pConfig->bSaveConfig = false;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_UNCHECKED);
                    }
                    else
                    {
                        this->pConfig->bSaveConfig = true;
                        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT, MF_CHECKED);
                    }
                }
            }
            if (this->pConfig->bMultipleMonoInput == true)
            {
                if (this->pConfig->szOutputFile.empty())
                    this->m_EdtOutPath.SetWindowText(this->pConfig->GetString(0x00207005).c_str());
                else
                    this->m_EdtOutPath.SetWindowText(this->pConfig->szOutputFile.c_str());
            }
            else
            {
                if (this->pConfig->szOutputPath.empty())
                    this->m_EdtOutPath.SetWindowText(this->pConfig->GetString(0x00207004).c_str());
                else
                    this->m_EdtOutPath.SetWindowText(this->pConfig->szOutputPath.c_str());
            }

            return true;
        }
        return false;
    }

    bool CMainDlg::SaveProgramConfig(std::wstring szFileName)
    {
        std::vector<config::Entry> cl;

        std::wstring mainWindow = this->GetWindowRectStr();
        cl.emplace_back(std::make_pair(std::wstring(L"MainWindow"), mainWindow));

        int nSettingsColWidth[2];
        for (int i = 0; i < 2; i++)
            nSettingsColWidth[i] = this->m_LstSettings.GetColumnWidth(i);
        std::wstring columnSizeSettings = std::to_wstring(nSettingsColWidth[0]) + L" " +  std::to_wstring(nSettingsColWidth[1]);
        cl.emplace_back(std::make_pair(std::wstring(L"ColumnSizeSettings"), columnSizeSettings));

        int nFilesColWidth[2];
        for (int i = 0; i < 2; i++)
            nFilesColWidth[i] = this->m_LstFiles.GetColumnWidth(i);
        std::wstring columnSizeFiles = std::to_wstring(nFilesColWidth[0]) + L" " + std::to_wstring(nFilesColWidth[1]);
        cl.emplace_back(std::make_pair(std::wstring(L"ColumnSizeFiles"), columnSizeFiles));

        std::wstring outputPath = (this->pConfig->szOutputPath == this->pConfig->GetString(0x00207004).c_str()) ? L"" : this->pConfig->szOutputPath;
        cl.emplace_back(std::make_pair(std::wstring( L"OutputPath"), outputPath));

        std::wstring outputFile = (this->pConfig->szOutputFile == this->pConfig->GetString(0x00207005).c_str()) ? L"" : this->pConfig->szOutputFile;
        cl.emplace_back(std::make_pair(std::wstring(L"OutputFile"), outputFile));

        std::wstring selectedPreset = std::to_wstring(this->m_CmbPresets.GetCurSel());
        cl.emplace_back(std::make_pair(std::wstring(L"SelectedPreset"), selectedPreset));

        std::wstring multipleMonoInput = (this->pConfig->bMultipleMonoInput == true) ? L"true" : L"false";
        cl.emplace_back(std::make_pair(std::wstring( L"MultipleMonoInput"), multipleMonoInput));

        std::wstring disableAllWarnings = (this->pConfig->bDisableAllWarnings == true) ? L"true" : L"false";
        cl.emplace_back(std::make_pair(std::wstring(L"DisableAllWarnings"), disableAllWarnings));

        std::wstring saveConfig = (this->pConfig->bSaveConfig == true) ? L"true" : L"false";
        cl.emplace_back(std::make_pair(std::wstring(L"SaveConfig"), saveConfig));

        return this->pConfig->SaveConfig(szFileName, cl);
    }

    bool CMainDlg::UpdateProgramEngines()
    {
        if (this->pConfig->m_Engines.size() == 0)
        {
            auto ce = std::make_pair(std::wstring(L"Aften"), std::wstring(L"libaften.dll"));
            this->pConfig->m_Engines.emplace_back(ce);

            auto& preset = GetCurrentPreset();
            preset.nCurrentEngine = 0;

            this->m_CmbEngines.InsertString(0, ce.first.c_str());
            this->m_CmbEngines.SetCurSel(0);

            if (this->api.IsAftenOpen())
            {
                this->api.CloseAftenAPI();
            }

            this->api.szLibPath = pConfig->m_Engines[GetCurrentPreset().nCurrentEngine].second;
            this->api.OpenAftenAPI();

            return false;
        }

        int nSize = (int)this->pConfig->m_Engines.size();
        for (int i = 0; i < nSize; i++)
        {
            auto& ce = this->pConfig->m_Engines[i];
            this->m_CmbEngines.InsertString(i, ce.first.c_str());
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

            this->api.szLibPath = pConfig->m_Engines[GetCurrentPreset().nCurrentEngine].second;
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

    bool CMainDlg::LoadProgramEngines(std::wstring szFileName)
    {
        this->pConfig->m_Engines.clear();
        this->m_CmbEngines.ResetContent();

        if (this->pConfig->LoadConfig(szFileName, this->pConfig->m_Engines) == true)
        {
            return this->UpdateProgramEngines();
        }
        else
        {
            auto ce = std::make_pair(std::wstring(L"Aften"), std::wstring(L"libaften.dll"));

            this->pConfig->m_Engines.clear();
            this->pConfig->m_Engines.emplace_back(ce);

            auto& preset = GetCurrentPreset();
            preset.nCurrentEngine = 0;

            this->m_CmbEngines.InsertString(0, ce.first.c_str());
            this->m_CmbEngines.SetCurSel(0);

            if (this->api.IsAftenOpen())
            {
                this->api.CloseAftenAPI();
            }

            this->api.szLibPath = pConfig->m_Engines[GetCurrentPreset().nCurrentEngine].second;
            this->api.OpenAftenAPI();
        }

        return false;
    }

    bool CMainDlg::SaveProgramEngines(std::wstring szFileName)
    {
        return this->pConfig->SaveConfig(szFileName, this->pConfig->m_Engines);
    }

    bool CMainDlg::LoadFilesList(std::wstring &szFileName)
    {
        std::vector<std::wstring> fl;
        if (this->pConfig->LoadFiles(szFileName, fl))
        {
            this->m_LstFiles.DeleteAllItems();
            for (int i = 0; i < (int)fl.size(); i++)
            {
                std::wstring szPath = fl[i];
                this->AddItemToFileList(szPath);
            }
            return true;
        }
        return false;
    }

    bool CMainDlg::SaveFilesList(std::wstring &szFileName, int nFormat)
    {
        std::vector<std::wstring> fl;
        int nItems = this->m_LstFiles.GetItemCount();
        for (int i = 0; i < nItems; i++)
        {
            std::wstring szPath = this->m_LstFiles.GetItemText(i, 0);
            fl.emplace_back(szPath);
        }
        return this->pConfig->SaveFiles(szFileName, fl, nFormat);
    }

    void CMainDlg::LoadAllConfiguration()
    {
        bool bPresetsRet = this->pConfig->LoadPresets(this->pConfig->m_Presets, this->pConfig->m_szPresetsFilePath, this->pConfig->m_DefaultPreset);
        OutputDebugString(((bPresetsRet ? L"Loaded encoder presets: " : L"Failed to load encoder presets: ") + this->pConfig->m_szPresetsFilePath).c_str());

        if (bPresetsRet == true)
        {
            int nPresetsSize = (int)pConfig->m_Presets.size();
            if (nPresetsSize > 0)
            {
                this->m_CmbPresets.ResetContent();
                for (int i = 0; i < nPresetsSize; i++)
                {
                    auto& preset = pConfig->m_Presets[i];
                    this->m_CmbPresets.InsertString(i, preset.szName.c_str());
                }

                if ((this->pConfig->nCurrentPreset >= nPresetsSize) || (this->pConfig->nCurrentPreset < 0))
                    this->pConfig->nCurrentPreset = 0;

                this->m_CmbPresets.SetCurSel(this->pConfig->nCurrentPreset);
                this->OnCbnSelchangeComboPresets();
            }
        }

        bool bConfigRet = this->LoadProgramConfig(this->pConfig->m_szConfigFilePath);
        OutputDebugString(((bConfigRet ? L"Loaded program config: " : L"Failed to load program config: ") + this->pConfig->m_szConfigFilePath).c_str());

        bool bEnginesRet = this->LoadProgramEngines(this->pConfig->m_szEnginesFilePath);
        OutputDebugString(((bEnginesRet ? L"Loaded encoder engines: " : L"Failed to load encoder engines: ") + this->pConfig->m_szEnginesFilePath).c_str());

        bool bFilesRet = this->LoadFilesList(this->pConfig->m_szFilesListFilePath);
        OutputDebugString(((bFilesRet ? L"Loaded files list: " : L"Failed to load files list: ") + this->pConfig->m_szFilesListFilePath).c_str());
    }

    void CMainDlg::SaveAllConfiguration()
    {
        bool bRet = false;

        bRet = this->pConfig->SavePresets(this->pConfig->m_Presets, this->pConfig->m_szPresetsFilePath, this->pConfig->m_DefaultPreset);
        OutputDebugString(((bRet ? L"Saved encoder presets: " : L"Error: Failed to save encoder presets: ") + this->pConfig->m_szPresetsFilePath).c_str());

        bRet = this->SaveProgramConfig(this->pConfig->m_szConfigFilePath);
        OutputDebugString(((bRet ? L"Saved program config: " : L"Error: Failed to save program config: ") + this->pConfig->m_szConfigFilePath).c_str());

        bRet = this->SaveProgramEngines(this->pConfig->m_szEnginesFilePath);
        OutputDebugString(((bRet ? L"Saved encoder engines: " : L"Error: Failed to save encoder engines: ") + this->pConfig->m_szEnginesFilePath).c_str());

        bRet = this->SaveFilesList(this->pConfig->m_szFilesListFilePath, 0);
        OutputDebugString(((bRet ? L"Saved files list: " : L"Error: Failed to save files list: ") + this->pConfig->m_szFilesListFilePath).c_str());
    }

    void CMainDlg::UpdateBitrateText()
    {
        int nCurPos = this->m_SldBitrate.GetPos();
        CString szBuff;

        if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
        {
            m_StcQualityBitrate.SetWindowText(this->pConfig->GetString(0x00202002).c_str());
            szBuff.Format(_T("%d"), nCurPos);

            auto& preset = GetCurrentPreset();
            preset.nQuality = nCurPos;
        }
        else
        {
            if ((nCurPos >= 0) && (nCurPos < (int)this->pConfig->m_EncoderOptions.nValidCbrBitrates.size()))
            {
                m_StcQualityBitrate.SetWindowText(this->pConfig->GetString(0x00202003).c_str());
                if (nCurPos == 0)
                    szBuff = this->pConfig->GetString(0x00207002).c_str();
                else
                    szBuff.Format(_T("%d kbps"), this->pConfig->m_EncoderOptions.nValidCbrBitrates[nCurPos]);

                auto& preset = GetCurrentPreset();
                preset.nBitrate = this->pConfig->m_EncoderOptions.nValidCbrBitrates[nCurPos];
            }
        }

        this->m_StcBitrate.SetWindowText(szBuff);
    }

    config::CPreset& CMainDlg::GetCurrentPreset()
    {
        if (this->pConfig->m_Presets.size() > 0)
            return this->pConfig->m_Presets[this->pConfig->nCurrentPreset];
        else
            return pConfig->m_DefaultPreset;
    }

    void CMainDlg::AddItemToFileList(std::wstring szPath)
    {
        ULONGLONG nFileSize = util::Utilities::GetFileSize64(szPath);
        std::wstring szExt = util::Utilities::GetFileExtension(szPath);
        if (util::StringHelper::TowLower(szExt) == L"avs")
        {
            AvsAudioInfo infoAVS;
            memset(&infoAVS, 0, sizeof(AvsAudioInfo));
            if (GetAvisynthFileInfo(szPath, &infoAVS) == false)
                return;

            nFileSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
        }

        int nItem = this->m_LstFiles.GetItemCount();
        SHFILEINFO sfi;
        LV_ITEM lvi;

        SHGetFileInfo((LPCTSTR)szPath.c_str(),
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
        this->m_LstFiles.SetItemText(nItem, 0, szPath.c_str());

        CString szFileSize;
        szFileSize.Format(_T("%I64d"), nFileSize);

        this->m_LstFiles.SetItemText(nItem, 1, szFileSize);
    }

    void CMainDlg::ApplyPresetToDlg(config::CPreset &preset)
    {
        
        for (int i = 0; i < (int)this->pConfig->m_EncoderOptions.m_Options.size(); i++)
        {
            int nOption = preset.nOptions[i];
            auto& option = this->pConfig->m_EncoderOptions.m_Options[i];
            std::wstring& szText = option.m_Values[nOption].first;
            this->m_LstSettings.SetItemText(i, 1, szText.c_str());
        }

        if (preset.nMode == AFTEN_ENC_MODE_CBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, (int)this->pConfig->m_EncoderOptions.nValidCbrBitrates.size() - 1, TRUE);
            int nPos = this->pConfig->FindValidBitratePos(preset.nBitrate);
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
            this->m_EdtThreads.SetWindowText(this->pConfig->GetString(0x00207002).c_str());
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), preset.nThreads);
            this->m_EdtThreads.SetWindowText(szBuff);
        }

        this->m_CmbEngines.SetCurSel(preset.nCurrentEngine);
        this->m_CmbRawSampleFormat.SetCurSel(preset.m_RawInput.nRawSampleFormat);

        if (preset.m_RawInput.nRawSampleRate == 0)
        {
            this->m_EdtRawSamplerate.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), preset.m_RawInput.nRawSampleRate);
            this->m_EdtRawSamplerate.SetWindowText(szBuff);
        }

        if (preset.m_RawInput.nRawChannels == 0)
        {
            this->m_EdtRawChannels.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), preset.m_RawInput.nRawChannels);
            this->m_EdtRawChannels.SetWindowText(szBuff);
        }
        this->UpdateBitrateText();

        POSITION pos = m_LstSettings.GetFirstSelectedItemPosition();
        if (pos != nullptr)
        {
            int nItem = m_LstSettings.GetNextSelectedItem(pos);

            this->m_LstSettings.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
            this->UpdateSettingsComboBox(nItem);

            int nOptionValue = preset.nOptions[nItem];
            this->m_CmbValue.SetCurSel(nOptionValue);
        }
        else
        {
            this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
            this->UpdateSettingsComboBox(0);

            int nOptionValue = preset.nOptions[0];
            this->m_CmbValue.SetCurSel(nOptionValue);
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
                    std::wstring szPath = szFile;
                    this->SearchFolderForFiles(szPath, true);
                }
                else
                {
                    std::wstring file = szFile;
                    std::wstring szExt = util::StringHelper::TowLower(util::Utilities::GetFileExtension(file));
                    if (this->pConfig->IsSupportedInputExt(szExt) == true)
                    {
                        std::wstring szPath = szFile;
                        this->AddItemToFileList(szPath);
                    }
                }
            }
        }

        ::DragFinish(hDropInfo);
    }

    void CMainDlg::UpdateSettingsComboBox(int nItem)
    {
        this->m_CmbValue.ResetContent();

        for (int i = 0; i < (int)this->pConfig->m_EncoderOptions.m_Options[nItem].m_Values.size(); i++)
        {
            auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];
            this->m_CmbValue.AddString(option.m_Values[i].first.c_str());
        }

        util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_SETTING, 15);

        if (this->pConfig->m_Presets.size() <= 0)
        {
            auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];
            this->m_CmbValue.SetCurSel(option.nDefaultValue);
        }
        else
        {
            this->m_CmbValue.SetCurSel(GetCurrentPreset().nOptions[nItem]);
        }
    }

    void CMainDlg::SearchFolderForFiles(std::wstring szPath, const bool bRecurse)
    {
        try
        {
            std::vector<std::wstring> files;
            bool bResult = util::Utilities::FindFiles(szPath, files, bRecurse);
            if (bResult == true)
            {
                for (auto& file : files)
                {
                    std::wstring szExt = util::StringHelper::TowLower(util::Utilities::GetFileExtension(file));
                    if (this->pConfig->IsSupportedInputExt(szExt) == true)
                    {
                        this->AddItemToFileList(file);
                    }
                }
            }
        }
        catch (...)
        {
            MessageBox(this->pConfig->GetString(0x0020702A).c_str(),
                this->pConfig->GetString(0x00207010).c_str(),
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

        auto pMenu = std::make_unique<CMenu>();
        pMenu->CreatePopupMenu();

        UINT nItemCount = ID_OPTIONS_MENU_START;
        auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];

        for (int i = 0; i < (int)option.m_Values.size(); i++)
        {
            pMenu->AppendMenu(MF_STRING, nItemCount, option.m_Values[i].first.c_str());
            nItemCount++;
        }

        int nCurSel = this->m_CmbValue.GetCurSel();
        pMenu->CheckMenuItem(ID_OPTIONS_MENU_START + nCurSel, MF_CHECKED);

        ::SetForegroundWindow(this->GetSafeHwnd());
        int nRet = (int)pMenu->TrackPopupMenu(TPM_RETURNCMD, point.x, point.y, this, nullptr);
        if (nRet >= ID_OPTIONS_MENU_START)
        {
            this->m_CmbValue.SetCurSel(nRet - ID_OPTIONS_MENU_START);
            this->OnCbnSelchangeComboSetting();
        }
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

        if (szBuff.Compare(this->pConfig->GetString(0x00207003).c_str()) == 0)
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
                this->m_EdtRawSamplerate.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
            }
            else
            {
                this->m_SpnRawSampleRate.SetPos(nPos);
            }
        }

        auto& preset = GetCurrentPreset();
        preset.m_RawInput.nRawSampleRate = nPos;
    }

    void CMainDlg::OnEnChangeEditRawChannels()
    {
        CString szBuff;
        int nPos;

        this->m_EdtRawChannels.GetWindowText(szBuff);

        if (szBuff.Compare(_T("")) == 0)
            return;

        if (szBuff.Compare(this->pConfig->GetString(0x00207003).c_str()) == 0)
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
                this->m_EdtRawChannels.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
            }
            else
            {
                this->m_SpnRawChannels.SetPos(nPos);
            }
        }

        auto& preset = GetCurrentPreset();
        preset.m_RawInput.nRawChannels = nPos;
    }

    void CMainDlg::OnEnChangeEditThreads()
    {
        CString szBuff;
        int nPos;

        this->m_EdtThreads.GetWindowText(szBuff);

        if (szBuff.Compare(_T("")) == 0)
            return;

        if (szBuff.Compare(this->pConfig->GetString(0x00207002).c_str()) == 0)
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
                this->m_EdtThreads.SetWindowText(this->pConfig->GetString(0x00207002).c_str());
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

        if (this->pConfig->bMultipleMonoInput == true)
            this->pConfig->szOutputFile = szBuff;
        else
            this->pConfig->szOutputPath = szBuff;
    }

    void CMainDlg::OnEnSetfocusEditOutputPath()
    {
        CString szBuff;
        this->m_EdtOutPath.GetWindowText(szBuff);
        if (szBuff.Compare(this->pConfig->GetString(0x00207004).c_str()) == 0 || szBuff.Compare(this->pConfig->GetString(0x00207005).c_str()) == 0)
            this->m_EdtOutPath.SetWindowText(_T(""));
    }

    void CMainDlg::OnEnSetfocusEditRawSampleRate()
    {
        CString szBuff;
        this->m_EdtRawSamplerate.GetWindowText(szBuff);
        if (szBuff.Compare(this->pConfig->GetString(0x00207003).c_str()) == 0)
            this->m_EdtRawSamplerate.SetWindowText(_T(""));
    }

    void CMainDlg::OnEnSetfocusEditRawChannels()
    {
        CString szBuff;
        this->m_EdtRawChannels.GetWindowText(szBuff);
        if (szBuff.Compare(this->pConfig->GetString(0x00207003).c_str()) == 0)
            this->m_EdtRawChannels.SetWindowText(_T(""));
    }

    void CMainDlg::OnEnSetfocusEditThreads()
    {
        CString szBuff;
        this->m_EdtThreads.GetWindowText(szBuff);
        if (szBuff.Compare(this->pConfig->GetString(0x00207002).c_str()) == 0)
            this->m_EdtThreads.SetWindowText(_T(""));
    }

    void CMainDlg::OnEnKillfocusEditOutputPath()
    {
        CString szBuff;
        this->m_EdtOutPath.GetWindowText(szBuff);
        if (szBuff.Compare(_T("")) == 0)
        {
            if (this->pConfig->bMultipleMonoInput == true)
                this->m_EdtOutPath.SetWindowText(this->pConfig->GetString(0x00207005).c_str());
            else
                this->m_EdtOutPath.SetWindowText(this->pConfig->GetString(0x00207004).c_str());
        }
        else
        {
            if (this->pConfig->bMultipleMonoInput == true)
                this->pConfig->szOutputFile = szBuff;
            else
                this->pConfig->szOutputPath = szBuff;
        }
    }

    void CMainDlg::OnEnKillfocusEditRawSampleRate()
    {
        CString szBuff;
        this->m_EdtRawSamplerate.GetWindowText(szBuff);
        if (szBuff.Compare(_T("")) == 0)
            this->m_EdtRawSamplerate.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
    }

    void CMainDlg::OnEnKillfocusEditRawChannels()
    {
        CString szBuff;
        this->m_EdtRawChannels.GetWindowText(szBuff);
        if (szBuff.Compare(_T("")) == 0)
            this->m_EdtRawChannels.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
    }

    void CMainDlg::OnEnKillfocusEditThreads()
    {
        CString szBuff;
        this->m_EdtThreads.GetWindowText(szBuff);
        if (szBuff.Compare(_T("")) == 0)
            this->m_EdtThreads.SetWindowText(this->pConfig->GetString(0x00207002).c_str());
    }

    void CMainDlg::InitTitle()
    {
        CString szDialogTitle = _T("");
        szDialogTitle.Format(_T("WAV to AC3 Encoder %s"), _T(VER_FILE_VERSION_SHORT_STR));
        this->SetWindowText(szDialogTitle);
    }

    void CMainDlg::InitTooltips()
    {
        this->m_SldBitrate.SetTooltipText(this->pConfig->GetString(0x00206001).c_str());
        this->m_ChkVbr.SetTooltipText(this->pConfig->GetString(0x00206002).c_str());
        this->m_CmbPresets.SetTooltipText(this->pConfig->GetString(0x00206003).c_str());

        CString szTmpText = this->pConfig->GetString(0x00206004).c_str();
        this->m_ChkSimdMMX.SetTooltipText(this->pConfig->GetString(0x00206005).c_str() + szTmpText);
        this->m_ChkSimdSSE.SetTooltipText(this->pConfig->GetString(0x00206006).c_str() + szTmpText);
        this->m_ChkSimdSSE2.SetTooltipText(this->pConfig->GetString(0x00206007).c_str() + szTmpText);

        this->m_ChkSimdSSE3.SetTooltipText(this->pConfig->GetString(0x00206008).c_str() + szTmpText);
        this->m_CmbRawSampleFormat.SetTooltipText(this->pConfig->GetString(0x00206009).c_str());
        this->m_EdtRawSamplerate.SetTooltipText(this->pConfig->GetString(0x0020600A).c_str());
        this->m_EdtRawChannels.SetTooltipText(this->pConfig->GetString(0x0020600B).c_str());
        this->m_EdtThreads.SetTooltipText(this->pConfig->GetString(0x0020600C).c_str());
        this->m_CmbEngines.SetTooltipText(this->pConfig->GetString(0x0020600D).c_str());
        this->m_EdtOutPath.SetTooltipText(this->pConfig->GetString(0x0020600E).c_str());
        this->m_ChkMultipleMonoInput.SetTooltipText(this->pConfig->GetString(0x0020600F).c_str());
        this->m_BtnEngines.SetTooltipText(this->pConfig->GetString(0x00206010).c_str());
    }

    void CMainDlg::InitSettingsList()
    {
        int nGroupCounter = -1;

        LVITEM li = { 0 };
        li.mask = LVIF_TEXT | LVIF_GROUPID | LVIF_COLUMNS;

        HWND listSettings = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();

        ListView_DeleteAllItems(listSettings);

        int nOptionsSize = (int)this->pConfig->m_EncoderOptions.m_Options.size();
        for (int i = 0; i < nOptionsSize; i++)
        {
            auto& option = this->pConfig->m_EncoderOptions.m_Options[i];
            if (option.bBeginGroup == true)
                nGroupCounter++;

            if (nGroupCounter >= 0 && nGroupCounter < nOptionsSize)
            {
                LPWSTR pszText = (LPTSTR)(LPCTSTR)option.szName.c_str();
                li.pszText = pszText;
                li.iItem = i;
                li.iSubItem = 0;
                li.iGroupId = 101 + nGroupCounter;

                LPWSTR pszSetting = (LPTSTR)(LPCTSTR)option.m_Values[option.nDefaultValue].first.c_str();
                ListView_InsertItem(listSettings, &li);
                ListView_SetItemText(listSettings, i, 1, pszSetting);

                CString szTip = CString(option.szHelpText.c_str());
                this->m_LstSettings.listTooltips.AddTail(szTip);
            }
        }

        this->m_LstSettings.bUseTooltipsList = true;
        this->m_LstSettings.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    }

    void CMainDlg::InitDefaultPreset()
    {
        int nOptionsSize = (int)this->pConfig->m_EncoderOptions.m_Options.size();
        for (int i = 0; i < nOptionsSize; i++)
        {
            auto& option = this->pConfig->m_EncoderOptions.m_Options[i];
            pConfig->m_DefaultPreset.nOptions[i] = option.nDefaultValue;
        }

        pConfig->m_DefaultPreset.szName = this->pConfig->GetString(0x00207001);
        pConfig->m_DefaultPreset.nMode = AFTEN_ENC_MODE_CBR;
        pConfig->m_DefaultPreset.nBitrate = 0;
        pConfig->m_DefaultPreset.nQuality = 240;
        pConfig->m_DefaultPreset.m_RawInput.nRawChannels = 0;
        pConfig->m_DefaultPreset.m_RawInput.nRawSampleFormat = 0;
        pConfig->m_DefaultPreset.m_RawInput.nRawSampleRate = 0;
        pConfig->m_DefaultPreset.nCurrentEngine = 0;
        pConfig->m_DefaultPreset.nThreads = 0;
        pConfig->m_DefaultPreset.nUsedSIMD[0] = 1;
        pConfig->m_DefaultPreset.nUsedSIMD[1] = 1;
        pConfig->m_DefaultPreset.nUsedSIMD[2] = 1;
        pConfig->m_DefaultPreset.nUsedSIMD[3] = 1;

        this->pConfig->nCurrentPreset = 0;
        this->m_CmbPresets.InsertString(0, pConfig->m_DefaultPreset.szName.c_str());
        this->m_CmbPresets.SetCurSel(this->pConfig->nCurrentPreset);

        this->m_CmbRawSampleFormat.SetCurSel(pConfig->m_DefaultPreset.m_RawInput.nRawSampleFormat);

        if (pConfig->m_DefaultPreset.m_RawInput.nRawSampleRate == 0)
        {
            this->m_EdtRawSamplerate.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), pConfig->m_DefaultPreset.m_RawInput.nRawSampleRate);
            this->m_EdtRawSamplerate.SetWindowText(szBuff);
        }

        if (pConfig->m_DefaultPreset.m_RawInput.nRawChannels == 0)
        {
            this->m_EdtRawChannels.SetWindowText(this->pConfig->GetString(0x00207003).c_str());
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), pConfig->m_DefaultPreset.m_RawInput.nRawChannels);
            this->m_EdtRawChannels.SetWindowText(szBuff);
        }

        this->m_ChkSimdMMX.SetCheck(pConfig->m_DefaultPreset.nUsedSIMD[0] == 1 ? BST_CHECKED : BST_UNCHECKED);
        this->m_ChkSimdSSE.SetCheck(pConfig->m_DefaultPreset.nUsedSIMD[1] == 1 ? BST_CHECKED : BST_UNCHECKED);
        this->m_ChkSimdSSE2.SetCheck(pConfig->m_DefaultPreset.nUsedSIMD[2] == 1 ? BST_CHECKED : BST_UNCHECKED);
        this->m_ChkSimdSSE3.SetCheck(pConfig->m_DefaultPreset.nUsedSIMD[3] == 1 ? BST_CHECKED : BST_UNCHECKED);

        if (pConfig->m_DefaultPreset.nThreads == 0)
        {
            this->m_EdtThreads.SetWindowText(this->pConfig->GetString(0x00207002).c_str());
        }
        else
        {
            CString szBuff;
            szBuff.Format(_T("%d"), pConfig->m_DefaultPreset.nThreads);
            this->m_EdtThreads.SetWindowText(szBuff);
        }

        if (pConfig->m_DefaultPreset.nMode == AFTEN_ENC_MODE_CBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, (int)this->pConfig->m_EncoderOptions.nValidCbrBitrates.size() - 1, TRUE);
            this->m_SldBitrate.SetPos(this->pConfig->FindValidBitratePos(pConfig->m_DefaultPreset.nBitrate));
            this->m_ChkVbr.SetCheck(BST_UNCHECKED);
        }
        else if (pConfig->m_DefaultPreset.nMode == AFTEN_ENC_MODE_VBR)
        {
            this->m_SldBitrate.SetTic(1);
            this->m_SldBitrate.SetRange(0, 1023, TRUE);
            this->m_SldBitrate.SetPos(pConfig->m_DefaultPreset.nQuality);
            this->m_ChkVbr.SetCheck(BST_CHECKED);
        }

        auto preset = pConfig->m_DefaultPreset;
        pConfig->m_Presets.emplace_back(preset);
    }

    void CMainDlg::InitRawSamleFormatComboBox()
    {
        this->pConfig->m_EncoderOptions.szRawSampleFormats[0] = this->pConfig->GetString(0x00207003).c_str();

        this->m_CmbRawSampleFormat.ResetContent();

        for (int i = 0; i < (int)this->pConfig->m_EncoderOptions.szRawSampleFormats.size(); i++)
            this->m_CmbRawSampleFormat.InsertString(i, this->pConfig->m_EncoderOptions.szRawSampleFormats[i].c_str());
    }

    void CMainDlg::InitSettingsListGroups()
    {
        LVGROUP lg;
        lg.cbSize = sizeof(LVGROUP);
        lg.mask = LVGF_HEADER | LVGF_GROUPID;

        HWND listView = this->GetDlgItem(IDC_LIST_SETTINGS)->GetSafeHwnd();

        ListView_RemoveAllGroups(listView);

        for (int i = 0; i < (int)this->pConfig->m_EncoderOptions.szGroups.size(); i++)
        {
            std::wstring szHeader = this->pConfig->GetString(0x00208000 + i + 1);
            LPWSTR pszHeader = (LPTSTR)(LPCTSTR)szHeader.c_str();
            lg.pszHeader = pszHeader;
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
            this->pConfig->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);

        if (this->pConfig->bMultipleMonoInput == true)
            this->m_EdtOutPath.SetWindowText(this->pConfig->GetString(0x00207005).c_str());
        else
            this->m_EdtOutPath.SetWindowText(this->pConfig->GetString(0x00207004).c_str());
    }

    BOOL CMainDlg::OnInitDialog()
    {
        CMyDialogEx::OnInitDialog();

        SetIcon(m_hIcon, TRUE);
        SetIcon(m_hIcon, FALSE);

        m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_MAIN_MENU));

        try
        {
            this->InitTitle();
            this->InitDialogControls();
            this->InitLang(true);
            this->InitDefaultPreset();
            this->UpdateBitrateText();
        }
        catch (...)
        {
            OutputDebugString(_T("Failed to init dialog."));
        }

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

    void SetListCtrlColumnText(CListCtrl& listCtrl, int nCol, std::wstring& text)
    {
        LPWSTR pszText = (LPTSTR)(LPCTSTR)text.c_str();
        LVCOLUMN lvCol;
        ::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
        lvCol.mask = LVCF_TEXT;
        listCtrl.GetColumn(nCol, &lvCol);
        lvCol.pszText = pszText;
        listCtrl.SetColumn(nCol, &lvCol);
    }

    void CMainDlg::InitLang(bool initLangMenu)
    {
        this->pConfig->SetEncoderOptions();

        this->InitRawSamleFormatComboBox();

        this->InitSettingsListGroups();

        if (initLangMenu == true)
            this->InitLangMenu();

        this->InitLangButtons();
        this->InitLangStaticText();
        this->InitLangFilesList();
        this->InitLangSettingsList();

        this->InitSettingsList();

        if (initLangMenu == false)
        {
            auto& preset = GetCurrentPreset();
            this->ApplyPresetToDlg(preset);
        }

        this->InitLangMainMenu();

        this->InitTooltips();

        std::wstring szBuff = this->pConfig->bMultipleMonoInput == true ? this->pConfig->szOutputFile : this->pConfig->szOutputPath;
        if (szBuff.empty() || szBuff.substr(0, 1) == L"<")
            this->m_EdtOutPath.SetWindowText(this->pConfig->bMultipleMonoInput == true ? this->pConfig->GetString(0x00207005).c_str() : this->pConfig->GetString(0x00207004).c_str());
        else
            this->m_EdtOutPath.SetWindowText(this->pConfig->bMultipleMonoInput == true ? this->pConfig->szOutputFile.c_str() : this->pConfig->szOutputPath.c_str());
    }

    void CMainDlg::InitLangButtons()
    {
        m_BtnEncode.SetWindowTextW(this->pConfig->GetString(0x00201001).c_str());
        m_BtnResetCurrent.SetWindowTextW(this->pConfig->GetString(0x00201002).c_str());
        m_BtnRemove.SetWindowTextW(this->pConfig->GetString(0x00201003).c_str());
        m_BtnAddNew.SetWindowTextW(this->pConfig->GetString(0x00201004).c_str());
        m_BtnAddFiles.SetWindowTextW(this->pConfig->GetString(0x00201005).c_str());
        m_BtnBrowse.SetWindowTextW(this->pConfig->GetString(0x00201006).c_str());
        m_BtnMuxWizard.SetWindowTextW(this->pConfig->GetString(0x00201007).c_str());
        m_BtnEngines.SetWindowTextW(this->pConfig->GetString(0x00201008).c_str());
    }

    void CMainDlg::InitLangStaticText()
    {
        this->GetDlgItem(IDC_STATIC_PRESET)->SetWindowTextW(this->pConfig->GetString(0x00202001).c_str());

        if (this->m_ChkVbr.GetCheck() == BST_CHECKED)
            this->GetDlgItem(IDC_STATIC_QUALITY)->SetWindowTextW(this->pConfig->GetString(0x00202002).c_str());
        else
            this->GetDlgItem(IDC_STATIC_QUALITY)->SetWindowTextW(this->pConfig->GetString(0x00202003).c_str());

        this->GetDlgItem(IDC_STATIC_OPTION_VALUE)->SetWindowTextW(this->pConfig->GetString(0x00202004).c_str());
        this->GetDlgItem(IDC_STATIC_SAMPLE_FORMAT)->SetWindowTextW(this->pConfig->GetString(0x00202005).c_str());
        this->GetDlgItem(IDC_STATIC_SAMPLE_RATE)->SetWindowTextW(this->pConfig->GetString(0x00202006).c_str());
        this->GetDlgItem(IDC_STATIC_CHANNELS)->SetWindowTextW(this->pConfig->GetString(0x00202007).c_str());
        this->GetDlgItem(IDC_STATIC_ENGINE)->SetWindowTextW(this->pConfig->GetString(0x00202008).c_str());
        this->GetDlgItem(IDC_STATIC_THREADS)->SetWindowTextW(this->pConfig->GetString(0x00202009).c_str());
        this->GetDlgItem(IDC_CHECK_MULTIPLE_MONO_INPUT)->SetWindowTextW(this->pConfig->GetString(0x0020200A).c_str());

        if (this->m_ChkMultipleMonoInput.GetCheck() == BST_CHECKED)
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200B).c_str());
        else
            this->GetDlgItem(IDC_STATIC_OUTPUT)->SetWindowText(this->pConfig->GetString(0x0020200C).c_str());
    }

    void CMainDlg::InitLangFilesList()
    {
        SetListCtrlColumnText(this->m_LstFiles, 0, this->pConfig->GetString(0x00203001));
        SetListCtrlColumnText(this->m_LstFiles, 1, this->pConfig->GetString(0x00203002));
    }

    void CMainDlg::InitLangFilesListContextMenu(CMenu &m_hMenu)
    {
        m_hMenu.ModifyMenuW(0, MF_STRING | MF_BYPOSITION, 0, this->pConfig->GetString(0x00204001).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_ADDFILES, 0, ID_LIST_ADDFILES, this->pConfig->GetString(0x00204002).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_ADDDIRECTORY, 0, ID_LIST_ADDDIRECTORY, this->pConfig->GetString(0x00204003).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_MUXWIZARD, 0, ID_LIST_MUXWIZARD, this->pConfig->GetString(0x00204004).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_LOADLIST, 0, ID_LIST_LOADLIST, this->pConfig->GetString(0x00204005).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_SAVELIST, 0, ID_LIST_SAVELIST, this->pConfig->GetString(0x00204006).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_MOVEUP, 0, ID_LIST_MOVEUP, this->pConfig->GetString(0x00204007).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_MOVEDOWN, 0, ID_LIST_MOVEDOWN, this->pConfig->GetString(0x00204008).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_DELFILES, 0, ID_LIST_DELFILES, this->pConfig->GetString(0x00204009).c_str());
        m_hMenu.ModifyMenuW(ID_LIST_CLEARLIST, 0, ID_LIST_CLEARLIST, this->pConfig->GetString(0x0020400A).c_str());
    }

    void CMainDlg::InitLangSettingsList()
    {
        SetListCtrlColumnText(this->m_LstSettings, 0, this->pConfig->GetString(0x00205001));
        SetListCtrlColumnText(this->m_LstSettings, 1, this->pConfig->GetString(0x00205002));
    }

    void CMainDlg::InitLangMainMenu()
    {
        CMenu *m_hMenu = this->GetMenu();

        m_hMenu->ModifyMenuW(0, MF_STRING | MF_BYPOSITION, 0, this->pConfig->GetString(0x00101001).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_ADDFILES, 0, ID_FILE_ADDFILES, this->pConfig->GetString(0x00101002).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_ADDDIRECTORY, 0, ID_FILE_ADDDIRECTORY, this->pConfig->GetString(0x00101003).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_MUXWIZARD, 0, ID_FILE_MUXWIZARD, this->pConfig->GetString(0x00101004).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_LOADFILESLIST, 0, ID_FILE_LOADFILESLIST, this->pConfig->GetString(0x00101005).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_SAVEFILESLIST, 0, ID_FILE_SAVEFILESLIST, this->pConfig->GetString(0x00101006).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_LOADPRESETS, 0, ID_FILE_LOADPRESETS, this->pConfig->GetString(0x00101007).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_SAVEPRESETS, 0, ID_FILE_SAVEPRESETS, this->pConfig->GetString(0x00101008).c_str());
        m_hMenu->ModifyMenuW(ID_FILE_EXIT, 0, ID_FILE_EXIT, this->pConfig->GetString(0x00101009).c_str());

        m_hMenu->ModifyMenuW(1, MF_STRING | MF_BYPOSITION, 1, this->pConfig->GetString(0x00102001).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_DISABLEALLWARNINGS, 0, ID_OPTIONS_DISABLEALLWARNINGS, this->pConfig->GetString(0x00102002).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_SAVECONFIGURATIONONEXIT, 0, ID_OPTIONS_SAVECONFIGURATIONONEXIT, this->pConfig->GetString(0x00102003).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_LOADCONFIGURATION, 0, ID_OPTIONS_LOADCONFIGURATION, this->pConfig->GetString(0x00102004).c_str());
        m_hMenu->ModifyMenuW(ID_OPTIONS_SAVECONFIGURATION, 0, ID_OPTIONS_SAVECONFIGURATION, this->pConfig->GetString(0x00102005).c_str());

        m_hMenu->ModifyMenuW(2, MF_STRING | MF_BYPOSITION, 2, this->pConfig->GetString(0x00103001).c_str());
        m_hMenu->ModifyMenuW(ID_LANGUAGE_DEFAULT, 0, ID_LANGUAGE_DEFAULT, this->pConfig->GetString(0x00103002).c_str());

        m_hMenu->ModifyMenuW(3, MF_STRING | MF_BYPOSITION, 3, this->pConfig->GetString(0x00104001).c_str());
        m_hMenu->ModifyMenuW(ID_HELP_WEBSITE, 0, ID_HELP_WEBSITE, this->pConfig->GetString(0x00104002).c_str());
        m_hMenu->ModifyMenuW(ID_HELP_ABOUT, 0, ID_HELP_ABOUT, this->pConfig->GetString(0x00104003).c_str());

        m_hMenu->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS,
            this->pConfig->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);

        m_hMenu->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
            this->pConfig->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);

        this->DrawMenuBar();
    }

    void CMainDlg::InitLangMenu()
    {
        if (this->pConfig->m_Languages.size() > 0)
        {
            CMenu *m_hMenu = this->GetMenu();
            CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

            m_hLangMenu->DeleteMenu(ID_LANGUAGE_DEFAULT, 0);

            for (int i = 0; i < (int)this->pConfig->m_Languages.size(); i++)
            {
                auto& lang = this->pConfig->m_Languages[i];
                std::wstring szBuff = lang.szEnglishName + L" (" + lang.szTargetName  + L")";
                m_hLangMenu->AppendMenu(MF_STRING, ID_LANGUAGE_MENU_START + i, szBuff.c_str());

                if (this->pConfig->m_nLangId == i)
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
        std::vector<std::pair<int, int>> items;

        pos = this->m_LstFiles.GetFirstSelectedItemPosition();
        while (pos != nullptr)
        {
            int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
            if ((nItem < this->m_LstFiles.GetItemCount() - 1) && (this->m_LstFiles.GetItemCount() >= 2))
            {
                items.emplace_back(std::make_pair(nItem, nItem + 1));
            }
            else
            {
                return;
            }
        }

        for (int i = (int)items.size() - 1; i >= 0; i--)
        {
            auto& item = items[i];

            szPath[0] = this->m_LstFiles.GetItemText(item.first, 0);
            szSize[0] = this->m_LstFiles.GetItemText(item.first, 1);
            szPath[1] = this->m_LstFiles.GetItemText(item.second, 0);
            szSize[1] = this->m_LstFiles.GetItemText(item.second, 1);

            this->m_LstFiles.SetItemText(item.first, 0, szPath[1]);
            this->m_LstFiles.SetItemText(item.first, 1, szSize[1]);
            this->m_LstFiles.SetItemText(item.second, 0, szPath[0]);
            this->m_LstFiles.SetItemText(item.second, 1, szSize[0]);
            this->m_LstFiles.SetItemState(item.second, LVIS_SELECTED, LVIS_SELECTED);
            this->m_LstFiles.SetItemState(item.first, 0, LVIS_SELECTED);
        }
    }

    void CMainDlg::OnListDelFiles()
    {
        std::vector<int> list;
        POSITION pos;

        pos = this->m_LstFiles.GetFirstSelectedItemPosition();
        while (pos != nullptr)
        {
            int nItem = this->m_LstFiles.GetNextSelectedItem(pos);
            list.emplace_back(nItem);
        }

        for (int i = (int)list.size() - 1; i >= 0; i--)
        {
            this->m_LstFiles.DeleteItem(list[i]);
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
                preset.nOptions[nItem] = nVal;

                auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];
                std::wstring szName = option.m_Values[nVal].first;
                this->m_LstSettings.SetItemText(nItem, 1, szName.c_str());
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
                preset.nOptions[nItem] = nVal;

                auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];
                std::wstring szName = option.m_Values[nVal].first;
                this->m_LstSettings.SetItemText(nItem, 1, szName.c_str());
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
            auto& option = this->pConfig->m_EncoderOptions.m_Options[nItem];
            this->MessageBox(option.szHelpText.c_str(), option.szName.c_str(), MB_ICONINFORMATION | MB_OK);
        }

        *pResult = 0;
    }

    bool CMainDlg::GetAvisynthFileInfo(std::wstring szFileName, AvsAudioInfo *pInfoAVS)
    {
        if (pInfoAVS == nullptr)
            return false;

        memset(pInfoAVS, 0, sizeof(AvsAudioInfo));

        CAvs2Raw decoderAVS;
        char szInputFileAVS[MAX_PATH] = "";

        util::Utilities::ConvertUnicodeToAnsi(szFileName.c_str(), szInputFileAVS, szFileName.length());
        if (decoderAVS.OpenAvisynth(szInputFileAVS) == false)
        {
            OutputDebugString(_T("Error: Failed to initialize Avisynth!"));
            this->MessageBox(this->pConfig->GetString(0x00207022).c_str(),
                this->pConfig->GetString(0x00207010).c_str(),
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
            std::wstring szFileName = m_LstFiles.GetItemText(nItem, 0);
            std::wstring szExt = util::Utilities::GetFileExtension(szFileName);
            if (util::StringHelper::TowLower(szExt) == L"avs")
            {
                AvsAudioInfo infoAVS;
                memset(&infoAVS, 0, sizeof(AvsAudioInfo));
                if (GetAvisynthFileInfo(szFileName, &infoAVS))
                {
                    std::wstring szInfo;

                    szInfo += this->pConfig->GetString(0x00207023) + _T("\t: ");

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
                        szInfo += this->pConfig->GetString(0x00207024) + _T("\n");
                        break;
                    }

                    CString szBuff;

                    szBuff.Format(_T("%s\t: %d\n"), this->pConfig->GetString(0x00207025), infoAVS.nSamplesPerSecond);
                    szInfo += szBuff;

                    szBuff.Format(_T("%s\t: %d\n"), this->pConfig->GetString(0x00207026), infoAVS.nAudioChannels);
                    szInfo += szBuff;

                    szBuff.Format(_T("%s\t: %I64d\n"), this->pConfig->GetString(0x00207027), infoAVS.nAudioSamples);
                    szInfo += szBuff;

                    szBuff.Format(_T("%s\t: %I64d"), this->pConfig->GetString(0x00207028), infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels);
                    szInfo += szBuff;

                    this->MessageBox(szInfo.c_str(),
                        this->pConfig->GetString(0x00207029).c_str(),
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
                    this->pConfig->GetString(0x00207009).c_str(),
                    this->pConfig->GetString(0x0020700A).c_str(),
                    MB_OK | MB_ICONERROR);
                return;
            }

            ZeroMemory(pFiles, dwMaxSize);

            std::wstring szFilter = this->pConfig->GetSupportedInputFilesFilter();

            CFileDialog fd(TRUE,
                this->pConfig->m_EncoderOptions.szSupportedInputExt[0].c_str(),
                _T(""),
                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_ALLOWMULTISELECT,
                szFilter.c_str(),
                this);

            fd.m_ofn.lpstrFile = pFiles;
            fd.m_ofn.nMaxFile = (dwMaxSize) / 2;

            if (fd.DoModal() == IDOK)
            {
                POSITION pos = fd.GetStartPosition();
                while (pos != nullptr)
                {
                    std::wstring szFileName = fd.GetNextPathName(pos);
                    this->AddItemToFileList(szFileName);
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

        auto szTitle = this->pConfig->GetString(0x0020700B).c_str();

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
                std::wstring szPath = lpBuffer;
                SearchFolderForFiles(szPath, true);
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
        dlg.pConfig = this->pConfig;

        int nItemsCount = this->m_LstFiles.GetItemCount();
        if (nItemsCount > 6)
            nItemsCount = 6;

        for (int i = 0; i < nItemsCount; i++)
        {
            dlg.szTmpInputFiles[i] = this->m_LstFiles.GetItemText(i, 0);
        }

        int nIndexAcmod = this->pConfig->FindOptionIndex(_T("acmod"));
        int nIndexLfe = this->pConfig->FindOptionIndex(_T("lfe"));
        int nIndexChconfig = this->pConfig->FindOptionIndex(_T("chconfig"));
        bool bUpdateChconfig = false;

        auto& preset = GetCurrentPreset();
        int nOptionValue = preset.nOptions[nIndexChconfig];
        auto& optionChconfig = this->pConfig->m_EncoderOptions.m_Options[nIndexChconfig];

        if (optionChconfig.nIgnoreValue != nOptionValue)
        {
            int nValue = optionChconfig.m_Values[nOptionValue].second;
            auto& channelConfig = this->pConfig->m_EncoderOptions.m_ChannelConfig[nValue];
            dlg.nChannelConfig = channelConfig.acmod;
            dlg.bLFE = (channelConfig.lfe == 1) ? true : false;
            bUpdateChconfig = true;
        }
        else
        {
            int nOptionValueAcmod = preset.nOptions[nIndexAcmod];
            auto& optionAcmod = this->pConfig->m_EncoderOptions.m_Options[nIndexAcmod];
            if (optionAcmod.nIgnoreValue != nOptionValueAcmod)
            {
                dlg.nChannelConfig = nOptionValueAcmod;
            }
            else
            {
                int nDefault = (int)(optionAcmod.m_Values.size() - 2);
                dlg.nChannelConfig = optionAcmod.m_Values[nDefault].second;
            }

            int nOptionValueLFE = preset.nOptions[nIndexLfe];
            dlg.bLFE = (nOptionValueLFE == 1) ? true : false;
            bUpdateChconfig = false;
        }

        if (dlg.DoModal() == IDOK)
        {
            int nRet = IDYES;

            if (this->pConfig->bDisableAllWarnings == false)
            {
                nRet = this->MessageBox(
                    this->pConfig->GetString(0x0020700C).c_str(),
                    this->pConfig->GetString(0x0020700D).c_str(),
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

                auto& optionAcmod = this->pConfig->m_EncoderOptions.m_Options[nIndexAcmod];
                preset.nOptions[nIndexAcmod] = (bUpdateChconfig == true) ? optionAcmod.nIgnoreValue : dlg.nChannelConfig;
                int nOptionValueAcmod = preset.nOptions[nIndexAcmod];
                this->m_LstSettings.SetItemText(nIndexAcmod, 1, optionAcmod.m_Values[nOptionValueAcmod].first.c_str());

                auto& optionLfe = this->pConfig->m_EncoderOptions.m_Options[nIndexLfe];
                preset.nOptions[nIndexLfe] = (bUpdateChconfig == true) ? optionLfe.nIgnoreValue : ((dlg.bLFE == true) ? 1 : 0);
                int nOptionValueLFE = preset.nOptions[nIndexLfe];
                this->m_LstSettings.SetItemText(nIndexLfe, 1, optionLfe.m_Values[nOptionValueLFE].first.c_str());

                auto& optionChconfig = this->pConfig->m_EncoderOptions.m_Options[nIndexChconfig];

                if (bUpdateChconfig == true)
                {
                    int acmod = dlg.nChannelConfig;
                    int lfe = (dlg.bLFE == true) ? 1 : 0;

                    for (int i = 0; i < (int)this->pConfig->m_EncoderOptions.m_ChannelConfig.size(); i++)
                    {
                        auto& channelConfig = this->pConfig->m_EncoderOptions.m_ChannelConfig[i];
                        if ((channelConfig.acmod == acmod) && (channelConfig.lfe == lfe))
                        {
                            preset.nOptions[nIndexChconfig] = i;
                            break;
                        }
                    }
                }
                else
                {
                    preset.nOptions[nIndexChconfig] = optionChconfig.nIgnoreValue;
                }

                int nOptionValue = preset.nOptions[nIndexChconfig];
                this->m_LstSettings.SetItemText(nIndexChconfig, 1, optionChconfig.m_Values[nOptionValue].first.c_str());

                if (this->pConfig->bMultipleMonoInput == false)
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
            this->pConfig->GetString(0x0020700E).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            LoadFilesList(szFileName);
        }
    }

    void CMainDlg::OnFileSaveFilesList()
    {
        CFileDialog fd(FALSE,
            _T("files"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            this->pConfig->GetString(0x0020700E).c_str(),
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

    void CMainDlg::OnFileLoadPresets()
    {
        CFileDialog fd(TRUE,
            _T("presets"),
            _T(""),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
            this->pConfig->GetString(0x0020700F).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            if (this->pConfig->LoadPresets(this->pConfig->m_Presets, szFileName, this->pConfig->m_DefaultPreset) == true)
            {
                this->m_CmbPresets.ResetContent();

                for (int i = 0; i < (int)pConfig->m_Presets.size(); i++)
                {
                    this->m_CmbPresets.AddString(pConfig->m_Presets[i].szName.c_str());
                }

                util::Utilities::SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS, 15);

                this->pConfig->nCurrentPreset = 0;
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
            this->pConfig->GetString(0x0020700F).c_str(),
            this);

        if (fd.DoModal() == IDOK)
        {
            std::wstring szFileName = fd.GetPathName();
            this->pConfig->SavePresets(this->pConfig->m_Presets, szFileName, this->pConfig->m_DefaultPreset);
        }
    }

    void CMainDlg::OnFileExit()
    {
        this->EndDialog(IDOK);
    }

    void CMainDlg::OnOptionsDisableAllWarnings()
    {
        this->pConfig->bDisableAllWarnings = this->pConfig->bDisableAllWarnings ? false : true;
        this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS,
            this->pConfig->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);
    }

    void CMainDlg::OnOptionsSaveConfigurationOnExit()
    {
        this->pConfig->bSaveConfig = this->pConfig->bSaveConfig ? false : true;
        this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
            this->pConfig->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);
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
        this->pConfig->m_nLangId = -1;
        this->pConfig->pStrings = nullptr;
        this->pConfig->m_szLangFileName = _T("");

        CMenu *m_hMenu = this->GetMenu();
        CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

        m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_CHECKED);

        for (int i = 0; i < (int)this->pConfig->m_Languages.size(); i++)
        {
            m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);
        }

        InitLang(false);
    }

    void CMainDlg::OnLanguageChange(UINT nID)
    {
        int nLangId = nID - ID_LANGUAGE_MENU_START;
        if (nLangId >= 0 && nLangId < (int)this->pConfig->m_Languages.size())
        {
            auto& lang = this->pConfig->m_Languages[nLangId];
            this->pConfig->m_nLangId = nID - ID_LANGUAGE_MENU_START;
            this->pConfig->pStrings = &lang.m_Strings;
            this->pConfig->m_szLangFileName = lang.szFileName;
        }
        CMenu *m_hMenu = this->GetMenu();
        CMenu *m_hLangMenu = m_hMenu->GetSubMenu(2);

        m_hLangMenu->CheckMenuItem(ID_LANGUAGE_DEFAULT, MF_UNCHECKED);

        for (int i = 0; i < (int)this->pConfig->m_Languages.size(); i++)
        {
            auto& lang = this->pConfig->m_Languages[i];
            if (this->pConfig->m_nLangId == i)
                m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_CHECKED);
            else
                m_hLangMenu->CheckMenuItem(ID_LANGUAGE_MENU_START + i, MF_UNCHECKED);
        }

        this->InitLang(false);
    }

    void CMainDlg::OnHelpWebsite()
    {
        util::Utilities::LaunchAndWait(_T("https://github.com/wieslawsoltes/wavtoac3encoder/"), _T(""), FALSE);
    }

    void CMainDlg::OnHelpAbout()
    {
        CAboutDlg dlg;
        dlg.pConfig = this->pConfig;
        dlg.DoModal();
    }
}
