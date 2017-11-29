//
// WAV to AC3 Encoder
// Copyright (C) 2007-2017 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "configuration\ProgramConfig.h"
#include "configuration\Language.h"
#include "configuration\EncoderOptions.h"
#include "controls\MyResizeDialog.h"
#include "controls\MyButton.h"
#include "controls\MyStatic.h"
#include "controls\MyEdit.h"
#include "controls\MyComboBoxEdit.h"
#include "controls\MyListCtrl.h"
#include "controls\MyComboBox.h"
#include "controls\MySliderCtrl.h"

#include "libaften/aften.h"

#ifndef DISABLE_AVISYNTH
#include "Avs2Raw.h"
#endif

#include "afxpropertygridctrl.h"

// define main window status bar id
#define IDC_STATUSBAR 1500

// define settings combo box id
#define IDC_COMBOBOX 1501

// start id for context-menu used in settings list

#define ID_OPTIONS_MENU_START 4000

// start id for language menu
#define ID_LANGUAGE_MENU_START 2000
#define ID_LANGUAGE_MENU_MAX 2999

typedef struct TItemToMove
{
    int nItem0;
    int nItem1;
} ItemToMove;

class CMainDlg : public CMyResizeDialog
{
public:
    CMainDlg(CWnd* pParent = NULL);
    enum { IDD = IDD_DIALOG_MAIN };
protected:
    HICON m_hIcon;
    HACCEL m_hAccelTable;
public:
    bool bVisible;
public:
    CommandLineOpt cmdLineOpt;
public:
    AftenAPI api;
    int nSortColumn;
    bool nSortOrder[2];
    bool bSettingsValueVisible;
    ConfigList_t m_EngineList;
public:
    EncoderPresetList_t encPresets;
    EncoderPreset defaultPreset;
    int nCurrentPreset = 0;
public:
    CString szOutputPath;
    CString szOutputFile;
public:
    bool bMultipleMonoInput;
public:
    bool bDisableAllWarnings;
public:
    bool bSaveConfig;
public:
    CStatusBarCtrl m_StatusBar;
public:
    CMySliderCtrl m_SldBitrate;
public:
    CMyListCtrl m_LstSettings;
    CMyListCtrl m_LstFiles;
public:
    CMyStatic m_StcBitrate;
    CMyStatic m_StcSelected;
    CMyStatic m_StcPreconfigured;
    CMyStatic m_StcQualityBitrate;
public:
    CMyComboBox m_CmbRawSampleFormat;
    CMyComboBox m_CmbValue;
    CMyComboBox m_CmbPresets;
    CMyComboBox m_CmbEngines;
public:
    CMyComboBoxEdit m_EdtCmbPresetName;
public:
    CMyEdit m_EdtRawSamplerate;
    CMyEdit m_EdtRawChannels;
    CMyEdit m_EdtOutPath;
    CMyEdit m_EdtThreads;
public:
    CMyButton m_ChkVbr;
    CMyButton m_BtnEncode;
    CMyButton m_ChkSimdMMX;
    CMyButton m_ChkSimdSSE;
    CMyButton m_ChkSimdSSE2;
    CMyButton m_ChkSimdSSE3;
    CMyButton m_BtnResetCurrent;
    CMyButton m_BtnRemove;
    CMyButton m_BtnAddNew;
    CMyButton m_BtnAddFiles;
    CMyButton m_BtnBrowse;
    CMyButton m_ChkMultipleMonoInput;
    CMyButton m_BtnMuxWizard;
    CMyButton m_BtnEngines;
public:
    CSpinButtonCtrl m_SpnRawSampleRate;
    CSpinButtonCtrl m_SpnRawChannels;
    CSpinButtonCtrl m_SpnThreads;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    void InitTitle();
    void InitDialogAnchors();
    void InitTooltips();
    void InitSettingsList();
    void InitDefaultPreset();
    void InitRawSamleFormatComboBox();
    void InitSettingsListGroups();
    void InitDialogControls();
    void InitLang(bool initLangMenu);
    void InitLangButtons();
    void InitLangStaticText();
    void InitLangFilesList();
    void InitLangFilesListContextMenu(CMenu &m_hMenu);
    void InitLangSettingsList();
    void InitLangMainMenu();
    void InitLangMenu();
public:
    bool LoadFilesList(CString &szFileName);
    bool SaveFilesList(CString &szFileName, int nFormat);
    bool LoadProgramConfig(CString szFileName);
    bool SaveProgramConfig(CString szFileName);
    bool UpdateProgramEngines();
    bool LoadProgramEngines(CString szFileName);
    bool SaveProgramEngines(CString szFileName);
    void LoadAllConfiguration();
    void SaveAllConfiguration();
public:
    EncoderPreset GetCurrentPreset();
    void UpdateCurrentPreset(EncoderPreset updatePreset);
public:
    void HandleDropFiles(HDROP hDropInfo);
    void SearchFolderForFiles(CString szFile, const bool bRecurse);
    void AddItemToFileList(CString szPath);
    void UpdateBitrateText();
    void UpdateSettingsComboBox(int nItem);
    void ApplyPresetToDlg(EncoderPreset &Preset);
    void ShowOptionPopup(bool bUseRect);
    void UpdateView(int nMode);
public:
#ifndef DISABLE_AVISYNTH
    bool GetAvisynthFileInfo(CString szFileName, AvsAudioInfo *pInfoAVS);
#endif
protected:
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnClose();
    afx_msg void OnDestroy();
public:
    afx_msg void OnFileAddFiles();
    afx_msg void OnFileAddDirectory();
    afx_msg void OnFileMuxWizard();
    afx_msg void OnFileLoadFilesList();
    afx_msg void OnFileSaveFilesList();
    afx_msg void OnFileLoadPresets();
    afx_msg void OnFileSavePresets();
    afx_msg void OnFileExit();
public:
    afx_msg void OnOptionsDisableAllWarnings();
    afx_msg void OnOptionsSaveConfigurationOnExit();
    afx_msg void OnOptionsLoadConfiguration();
    afx_msg void OnOptionsSaveConfiguration();
public:
    afx_msg void OnLanguageChangeDefault();
    afx_msg void OnLanguageChange(UINT nID);
public:
    afx_msg void OnHelpCommandLine();
    afx_msg void OnHelpWebsite();
    afx_msg void OnHelpAbout();
public:
    afx_msg void OnListAddFiles();
    afx_msg void OnListAddDirectory();
    afx_msg void OnListMuxWizard();
    afx_msg void OnListLoadList();
    afx_msg void OnListSavelist();
    afx_msg void OnListMoveUp();
    afx_msg void OnListMoveDown();
    afx_msg void OnListDelFiles();
    afx_msg void OnListClearList();
public:
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
public:
    afx_msg void OnCbnSelchangeComboSetting();
    afx_msg void OnCbnSelchangeComboPresets();
    afx_msg void OnCbnSelchangeComboRawSampleFormat();
    afx_msg void OnCbnSelchangeComboEngines();
public:
    afx_msg void OnLvnItemchangedListSettings(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnKeydownListFiles(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnColumnclickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnKeydownListSettings(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnNMRclickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRclickListSettings(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkListSettings(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult);
public:
    afx_msg void OnEnChangeEditRawSampleRate();
    afx_msg void OnEnChangeEditRawChannels();
    afx_msg void OnEnChangeEditThreads();
    afx_msg void OnEnChangeEditOutputPath();
public:
    afx_msg void OnEnSetfocusEditOutputPath();
    afx_msg void OnEnSetfocusEditRawSampleRate();
    afx_msg void OnEnSetfocusEditRawChannels();
    afx_msg void OnEnSetfocusEditThreads();
public:
    afx_msg void OnEnKillfocusEditOutputPath();
    afx_msg void OnEnKillfocusEditRawSampleRate();
    afx_msg void OnEnKillfocusEditRawChannels();
    afx_msg void OnEnKillfocusEditThreads();
public:
    afx_msg LRESULT EditChangeComboPresets(WPARAM wParam, LPARAM lParam);
};
