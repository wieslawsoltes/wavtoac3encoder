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

#include "StdAfx.h"
#include "MainApp.h"
#include "EncWAVtoAC3Dlg.h"
#include "EncWAVtoAC3WorkDlg.h"
#include "EncWAVtoAC3MuxDlg.h"
#include "EncWAVtoAC3EngDlg.h"
#include "AboutDlg.h"
#include "..\utilities\Utilities.h"
#include "..\utilities\MyFile.h"
#include "EncWorkThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CEncWAVtoAC3Dlg::LoadProgramConfig(CString szFileName)
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

bool CEncWAVtoAC3Dlg::SaveProgramConfig(CString szFileName)
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

bool CEncWAVtoAC3Dlg::UpdateProgramEngines()
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

bool CEncWAVtoAC3Dlg::LoadProgramEngines(CString szFileName)
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

bool CEncWAVtoAC3Dlg::SaveProgramEngines(CString szFileName)
{
    // save engines configuration
    return ::SaveConfig(szFileName, this->m_EngineList);
}

bool CEncWAVtoAC3Dlg::LoadFilesList(CString &szFileName)
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

bool CEncWAVtoAC3Dlg::SaveFilesList(CString &szFileName, int nFormat)
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

void CEncWAVtoAC3Dlg::LoadAllConfiguration()
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

void CEncWAVtoAC3Dlg::SaveAllConfiguration()
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

void CEncWAVtoAC3Dlg::UpdateBitrateText()
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

EncoderPreset CEncWAVtoAC3Dlg::GetCurrentPreset()
{
    return this->encPresets.GetAt(this->encPresets.FindIndex(this->nCurrentPreset));
}

void CEncWAVtoAC3Dlg::UpdateCurrentPreset(EncoderPreset updatePreset)
{
    this->encPresets.SetAt(this->encPresets.FindIndex(this->nCurrentPreset), updatePreset);
}

void CEncWAVtoAC3Dlg::AddItemToFileList(CString szPath)
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

void CEncWAVtoAC3Dlg::ApplyPresetToDlg(EncoderPreset &Preset)
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

void CEncWAVtoAC3Dlg::HandleDropFiles(HDROP hDropInfo)
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

void CEncWAVtoAC3Dlg::UpdateSettingsComboBox(int nItem)
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

void CEncWAVtoAC3Dlg::SearchFolderForFiles(CString szPath, const bool bRecurse)
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

void CEncWAVtoAC3Dlg::ShowOptionPopup(bool bUseRect)
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
