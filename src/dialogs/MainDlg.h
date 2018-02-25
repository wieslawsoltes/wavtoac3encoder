#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <afxcmn.h>
#include <afxwin.h>
#include "controls\MyDialogEx.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"
#include "controls\MyStatusBarCtrl.h"
#include "controls\MySpinButtonCtrl.h"
#include "utilities\StringHelper.h"
#include "logger\Log.h"
#include "configuration\Configuration.h"
#include "avs\src\Avs2Raw\Avs2Raw.h"

#define IDC_STATUSBAR 1500
#define IDC_COMBOBOX 1501
#define ID_OPTIONS_MENU_START 4000
#define ID_LANGUAGE_MENU_START 2000
#define ID_LANGUAGE_MENU_MAX 2999

namespace dialogs
{
    class CMainDlg : public controls::CMyDialogEx
    {
        DECLARE_DYNAMIC(CMainDlg)
    public:
        CMainDlg(CWnd* pParent = nullptr);
        virtual ~CMainDlg();
        enum { IDD = IDD_DIALOG_MAIN };
    protected:
        HICON m_hIcon;
        HACCEL m_hAccelTable;
        CImageList m_ImageListLarge;
        CImageList m_ImageListSmall;
    public:
        controls::CMyStatusBarCtrl m_StatusBar;
        controls::CMySliderCtrl m_SldBitrate;
        controls::CMyListCtrl m_LstSettings;
        controls::CMyListCtrl m_LstFiles;
        controls::CMyStatic m_StcBitrate;
        controls::CMyStatic m_StcSelected;
        controls::CMyStatic m_StcPreconfigured;
        controls::CMyStatic m_StcQualityBitrate;
        controls::CMyComboBox m_CmbRawSampleFormat;
        controls::CMyComboBox m_CmbValue;
        controls::CMyComboBox m_CmbPresets;
        controls::CMyComboBox m_CmbEngines;
        controls::CMyComboBoxEdit m_EdtCmbPresetName;
        controls::CMyEdit m_EdtRawSamplerate;
        controls::CMyEdit m_EdtRawChannels;
        controls::CMyEdit m_EdtOutPath;
        controls::CMyEdit m_EdtThreads;
        controls::CMyButton m_ChkVbr;
        controls::CMyButton m_BtnEncode;
        controls::CMyButton m_ChkSimdMMX;
        controls::CMyButton m_ChkSimdSSE;
        controls::CMyButton m_ChkSimdSSE2;
        controls::CMyButton m_ChkSimdSSE3;
        controls::CMyButton m_BtnResetCurrent;
        controls::CMyButton m_BtnRemove;
        controls::CMyButton m_BtnAddNew;
        controls::CMyButton m_BtnAddFiles;
        controls::CMyButton m_BtnBrowse;
        controls::CMyButton m_ChkMultipleMonoInput;
        controls::CMyButton m_BtnMuxWizard;
        controls::CMyButton m_BtnEngines;
        controls::CMySpinButtonCtrl m_SpnRawSampleRate;
        controls::CMySpinButtonCtrl m_SpnRawChannels;
        controls::CMySpinButtonCtrl m_SpnThreads;
    public:
        bool bVisible;
        config::CConfiguration * pConfig;
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);
        DECLARE_MESSAGE_MAP()
    public:
        virtual BOOL OnInitDialog();
        virtual BOOL PreTranslateMessage(MSG* pMsg);
    public:
        void InitLang(bool initLangMenu);
        void InitLangButtons();
        void InitLangStaticText();
        void InitLangFilesList();
        void InitLangSettingsList();
        void InitLangMainMenu();
        void InitLangFilesListContextMenu(CMenu &m_hMenu);
        void InitLangMenu();
        void InitTooltips();
        void InitSettingsList();
        void InitDefaultPreset();
        void InitRawSamleFormatComboBox();
        void InitSettingsListGroups();
        void InitDialogControls();
        void HandleDropFiles(HDROP hDropInfo);
        void ShowOptionPopup(bool bUseRect);
        void RedrawFiles();
        void UpdatePresets();
        void UpdateEngines();
        void UpdateBitrateText();
        void UpdateSettingsComboBox(int nItem);
        void ApplyPresetToDlg(config::CPreset &preset);
        void ApplyEngineToDlg(config::CEngine &engine);
        bool LoadPresets(const std::wstring& szFileName);
        bool SavePresets(const std::wstring& szFileName);
        bool LoadEngines(const std::wstring& szFileName);
        bool SaveEngines(const std::wstring& szFileName);
        bool LoadFiles(const std::wstring& szFileName);
        bool SaveFiles(const std::wstring& szFileName, const int nFormat);
        bool LoadConfig(const std::wstring& szFileName);
        bool SaveConfig(const std::wstring& szFileName);
        void LoadConfiguration();
        void SaveConfiguration();
        void SearchFolderForFiles(std::wstring szFile, const bool bRecurse);
        bool GetAvisynthFileInfo(std::wstring szFileName, AvsAudioInfo *pInfoAVS);
        ULONGLONG GetFileSize(const std::wstring& szPath);
        bool AddFile(const std::wstring& szPath);
        bool AddPath(const std::wstring pattern);
        bool AddFiles(const std::vector<std::wstring>& files);
    protected:
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
    public:
        afx_msg void OnDropFiles(HDROP hDropInfo);
        afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnClose();
        afx_msg void OnDestroy();
        afx_msg void OnFileAddFiles();
        afx_msg void OnFileAddDirectory();
        afx_msg void OnFileMuxWizard();
        afx_msg void OnFileLoadFilesList();
        afx_msg void OnFileSaveFilesList();
        afx_msg void OnFileLoadPresets();
        afx_msg void OnFileSavePresets();
        afx_msg void OnFileExit();
        afx_msg void OnOptionsDisableAllWarnings();
        afx_msg void OnOptionsSaveConfigurationOnExit();
        afx_msg void OnOptionsLoadConfiguration();
        afx_msg void OnOptionsSaveConfiguration();
        afx_msg void OnLanguageChangeDefault();
        afx_msg void OnLanguageChange(UINT nID);
        afx_msg void OnHelpWebsite();
        afx_msg void OnHelpAbout();
        afx_msg void OnListAddFiles();
        afx_msg void OnListAddDirectory();
        afx_msg void OnListMuxWizard();
        afx_msg void OnListLoadList();
        afx_msg void OnListSavelist();
        afx_msg void OnListMoveUp();
        afx_msg void OnListMoveDown();
        afx_msg void OnListDelFiles();
        afx_msg void OnListClearList();
        afx_msg void OnBnClickedCheckVbr();
        afx_msg void OnBnClickedButtonPresetAdd();
        afx_msg void OnBnClickedButtonPresetDel();
        afx_msg void OnBnClickedButtonAdd();
        afx_msg void OnBnClickedButtonEncode();
        afx_msg void OnBnClickedCheckSimdMmx();
        afx_msg void OnBnClickedCheckSimdSse();
        afx_msg void OnBnClickedCheckSimdSse2();
        afx_msg void OnBnClickedCheckSimdSse3();
        afx_msg void OnBnClickedButtonBrowse();
        afx_msg void OnBnClickedCheckMultipleMonoInput();
        afx_msg void OnBnClickedButtonPresetsDefaults();
        afx_msg void OnBnClickedButtonMuxWizard();
        afx_msg void OnBnClickedButtonEngines();
        afx_msg void OnCbnSelchangeComboSetting();
        afx_msg void OnCbnSelchangeComboPresets();
        afx_msg void OnCbnSelchangeComboRawSampleFormat();
        afx_msg void OnCbnSelchangeComboEngines();
        afx_msg LRESULT EditChangeComboPresets(WPARAM wParam, LPARAM lParam);
        afx_msg void OnLvnItemchangedListSettings(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnLvnGetdispinfoListFiles(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnLvnKeydownListFiles(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnLvnColumnclickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnLvnKeydownListSettings(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnNMRclickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnNMRclickListSettings(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnNMDblclkListSettings(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnEnChangeEditRawSampleRate();
        afx_msg void OnEnChangeEditRawChannels();
        afx_msg void OnEnChangeEditThreads();
        afx_msg void OnEnChangeEditOutputPath();
        afx_msg void OnEnSetfocusEditOutputPath();
        afx_msg void OnEnSetfocusEditRawSampleRate();
        afx_msg void OnEnSetfocusEditRawChannels();
        afx_msg void OnEnSetfocusEditThreads();
        afx_msg void OnEnKillfocusEditOutputPath();
        afx_msg void OnEnKillfocusEditRawSampleRate();
        afx_msg void OnEnKillfocusEditRawChannels();
        afx_msg void OnEnKillfocusEditThreads();
    };
}
