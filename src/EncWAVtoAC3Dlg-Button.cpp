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

void CEncWAVtoAC3Dlg::OnBnClickedButtonAdd()
{
	this->OnFileAddFiles();
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonEncode()
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

	CEncWAVtoAC3WorkDlg dlg;

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
	CMyCounter countTime;
	CString szText;

	countTime.Init();

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
				FormatTime(countTime.Time(), 3),
				countTime.Time());

			::LogMessage(szText);
		}
		else
		{
			szText.Format(HaveLangStrings() ? GetLangString(0x00207019) : _T("Encoded %d file%s in %s (%0.3lf sec)"),
				dlg.nCount,
				dlg.nCount == 1 ? _T("") :
				(HaveLangStrings() ? GetLangString(0x0020701A) : _T("s")),
				FormatTime(countTime.Time(), 3),
				countTime.Time());

			::LogMessage(szText);
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

void CEncWAVtoAC3Dlg::OnBnClickedButtonPresetAdd()
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

	SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS);
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonPresetDel()
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

		SetComboBoxHeight(this->GetSafeHwnd(), IDC_COMBO_PRESETS);

		// update all controls
		this->OnCbnSelchangeComboPresets();
	}
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonBrowse()
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

void CEncWAVtoAC3Dlg::OnBnClickedButtonPresetsDefaults()
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

void CEncWAVtoAC3Dlg::OnBnClickedCheckMultipleMonoInput()
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

void CEncWAVtoAC3Dlg::OnBnClickedButtonMuxWizard()
{
	this->OnFileMuxWizard();
}

void CEncWAVtoAC3Dlg::OnBnClickedButtonEngines()
{
	CEncWAVtoAC3EngDlg dlg;

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
