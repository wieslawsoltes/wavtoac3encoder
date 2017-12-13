#pragma once

#ifndef __AFXWIN_H__
#error "Include 'StdAfx.h' before including this file for PCH"
#endif

#include "res\resource.h"
#include "configuration\Config.h"

class CEncWAVtoAC3App : public CWinAppEx
{
public:
    CEncWAVtoAC3App();
    virtual ~CEncWAVtoAC3App();
public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
public:
    LangMap_t * m_Lang;
    LangList_t m_LangLst;
    CString m_szLangFileName = _T("");
    BOOL m_bHaveLang = FALSE;
    int m_nLangId = -1;
public:
    bool m_bIsPortable = true;
    CString m_szPresetsFilePath;
    CString m_szConfigFilePath;
    CString m_szEnginesFilePath;
    CString m_szFilesListFilePath;
    CString m_szLangFilePath;
public:
    bool LoadConfig(CString &szFileName, ConfigList_t &cl);
    bool SaveConfig(CString &szFileName, ConfigList_t &cl);
public:
    void SearchFolderForLang(CString szPath, const bool bRecurse, LangList_t& m_LangLst);
    void CleanLangList(LangList_t& m_LangLst);
    bool LoadLang(CString &szFileName, LangMap_t *lm);
public:
    bool LoadLangConfig(CString &szFileName);
    bool SaveLangConfig(CString &szFileName);
public:
    void LoadLangStrings();
    BOOL HaveLangStrings();
    CString& GetLangString(int id);
};

extern CEncWAVtoAC3App theApp;
