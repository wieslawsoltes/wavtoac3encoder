//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wies³aw Šoltés <wieslaw.soltes@gmail.com>
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

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboSetting()
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

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboPresets()
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

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboEngines()
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

void CEncWAVtoAC3Dlg::OnCbnSelchangeComboRawSampleFormat()
{
	EncoderPreset tmpPreset = GetCurrentPreset();
	tmpPreset.nRawSampleFormat = this->m_CmbRawSampleFormat.GetCurSel();
	UpdateCurrentPreset(tmpPreset);
}

LRESULT CEncWAVtoAC3Dlg::EditChangeComboPresets(WPARAM wParam, LPARAM lParam)
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
