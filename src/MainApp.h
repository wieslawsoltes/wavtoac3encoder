#pragma once

#ifndef __AFXWIN_H__
#error "include 'StdAfx.h' before including this file for PCH"
#endif

#include "res\resource.h"

typedef CMap<int, int, CString, CString&> LangMap_t;

typedef struct TLang
{
    CString szFileName;
    CString szEnglishName;
    CString szTargetName;
    LangMap_t *lm;
} Lang;

typedef CList<Lang, Lang&> LangList_t;

class CLangManager
{
public:
    LangMap_t *m_Lang;
    LangList_t m_LangLst;
    CString m_szLangFileName = _T("");
    BOOL m_bHaveLang = FALSE;
    int m_nLangId = -1;
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
}

class CEncWAVtoAC3App : public CWinAppEx
{
public:
    CEncWAVtoAC3App();
    virtual ~CEncWAVtoAC3App();
public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
public:
    bool m_bIsPortable = true;
    CString m_szPresetsFilePath;
    CString m_szConfigFilePath;
    CString m_szEnginesFilePath;
    CString m_szFilesListFilePath;
    CString m_szLangFilePath;
    CString m_szLogFilePath;
};

extern CLangManager theLangManager;
extern CEncWAVtoAC3App theApp;
