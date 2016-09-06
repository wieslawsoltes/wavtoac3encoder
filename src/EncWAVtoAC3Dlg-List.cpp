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

int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData)
{
	CEncWAVtoAC3Dlg *pDlg = (CEncWAVtoAC3Dlg *)lParamData;

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

void CEncWAVtoAC3Dlg::OnListMoveDown()
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

void CEncWAVtoAC3Dlg::OnListDelFiles()
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

void CEncWAVtoAC3Dlg::OnListClearList()
{
	this->m_LstFiles.DeleteAllItems();
}

void CEncWAVtoAC3Dlg::OnLvnItemchangedListSettings(NMHDR *pNMHDR, LRESULT *pResult)
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

void CEncWAVtoAC3Dlg::OnLvnKeydownListFiles(NMHDR *pNMHDR, LRESULT *pResult)
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

void CEncWAVtoAC3Dlg::OnLvnColumnclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
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

void CEncWAVtoAC3Dlg::OnLvnKeydownListSettings(NMHDR *pNMHDR, LRESULT *pResult)
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

void CEncWAVtoAC3Dlg::OnNMRclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
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

void CEncWAVtoAC3Dlg::OnNMRclickListSettings(NMHDR *pNMHDR, LRESULT *pResult)
{
	// show option values in popup menu
	this->ShowOptionPopup(true);

	*pResult = 0;
}

void CEncWAVtoAC3Dlg::OnNMDblclkListSettings(NMHDR *pNMHDR, LRESULT *pResult)
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
bool CEncWAVtoAC3Dlg::GetAvisynthFileInfo(CString szFileName, AvsAudioInfo *pInfoAVS)
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

void CEncWAVtoAC3Dlg::OnNMDblclkListFiles(NMHDR *pNMHDR, LRESULT *pResult)
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
