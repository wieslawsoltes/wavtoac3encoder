#pragma once

#ifndef __AFXWIN_H__
#error "Include 'StdAfx.h' before including this file for PCH"
#endif

#include "res\resource.h"
#include "configuration\Configuration.h"

class CEncWAVtoAC3App : public CWinAppEx
{
public:
    CEncWAVtoAC3App();
    virtual ~CEncWAVtoAC3App();
public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
public:
    void LoadConfig();
    void SaveConfig();
};

extern CEncWAVtoAC3App m_App;
extern CConfiguration m_Config;
