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

void SetListCtrlColumnText(CListCtrl& listCtrl, int nCol, CString& text)
{
	LVCOLUMN lvCol;
	::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
	lvCol.mask = LVCF_TEXT;
	listCtrl.GetColumn(nCol, &lvCol);
	lvCol.pszText = (LPTSTR)(LPCTSTR)text;
	listCtrl.SetColumn(nCol, &lvCol);
}

void CEncWAVtoAC3Dlg::InitLang(bool initLangMenu)
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

void CEncWAVtoAC3Dlg::InitLangButtons()
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

void CEncWAVtoAC3Dlg::InitLangStaticText()
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

void CEncWAVtoAC3Dlg::InitLangFilesList()
{
	SetListCtrlColumnText(this->m_LstFiles, 0, GetLangString(0x00203001));
	SetListCtrlColumnText(this->m_LstFiles, 1, GetLangString(0x00203002));
}

void CEncWAVtoAC3Dlg::InitLangFilesListContextMenu(CMenu &m_hMenu)
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

void CEncWAVtoAC3Dlg::InitLangSettingsList()
{
	SetListCtrlColumnText(this->m_LstSettings, 0, GetLangString(0x00205001));
	SetListCtrlColumnText(this->m_LstSettings, 1, GetLangString(0x00205002));
}

void CEncWAVtoAC3Dlg::InitLangMainMenu()
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

void CEncWAVtoAC3Dlg::InitLangMenu()
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
