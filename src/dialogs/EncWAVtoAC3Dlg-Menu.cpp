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
#include "..\utilities\Utilities.h"
#include "..\utilities\MyFile.h"
#include "EncWorkThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CEncWAVtoAC3Dlg::OnFileAddFiles()
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

void CEncWAVtoAC3Dlg::OnFileAddDirectory()
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

void CEncWAVtoAC3Dlg::OnFileMuxWizard()
{
    CEncWAVtoAC3MuxDlg dlg;

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

void CEncWAVtoAC3Dlg::OnFileLoadFilesList()
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

void CEncWAVtoAC3Dlg::OnFileSaveFilesList()
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

void CEncWAVtoAC3Dlg::OnFileLoadPresets()
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

void CEncWAVtoAC3Dlg::OnFileSavePresets()
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

void CEncWAVtoAC3Dlg::OnFileExit()
{
    this->EndDialog(IDOK);
}

void CEncWAVtoAC3Dlg::OnOptionsDisableAllWarnings()
{
    this->bDisableAllWarnings = this->bDisableAllWarnings ? false : true;
    this->GetMenu()->CheckMenuItem(ID_OPTIONS_DISABLEALLWARNINGS,
        this->bDisableAllWarnings ? MF_CHECKED : MF_UNCHECKED);
}

void CEncWAVtoAC3Dlg::OnOptionsSaveConfigurationOnExit()
{
    this->bSaveConfig = this->bSaveConfig ? false : true;
    this->GetMenu()->CheckMenuItem(ID_OPTIONS_SAVECONFIGURATIONONEXIT,
        this->bSaveConfig ? MF_CHECKED : MF_UNCHECKED);
}

void CEncWAVtoAC3Dlg::OnOptionsLoadConfiguration()
{
    this->LoadAllConfiguration();
}

void CEncWAVtoAC3Dlg::OnOptionsSaveConfiguration()
{
    // save configuration to disk (works also in read-only mode)
    this->SaveAllConfiguration();
}

void CEncWAVtoAC3Dlg::OnLanguageChangeDefault()
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

void CEncWAVtoAC3Dlg::OnLanguageChange(UINT nID)
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

void CEncWAVtoAC3Dlg::OnHelpCommandLine()
{
    ShowCommandLineHelp(this->GetSafeHwnd());
}

void CEncWAVtoAC3Dlg::OnHelpWebsite()
{
    // go to program website using default Internet browser
    LaunchAndWait(ENCWAVTOAC3_URL_HOME, _T(""), FALSE);
}

void CEncWAVtoAC3Dlg::OnHelpAbout()
{
    CAboutDlg dlg;

    // show program about dialog box
    dlg.DoModal();
}
