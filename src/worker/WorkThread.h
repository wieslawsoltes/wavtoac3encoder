#pragma once

#include "dialogs\WorkDlg.h"
#include "configuration\Configuration.h"
#include "utilities\ListT.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"
#include "utilities\TimeCount.h"
#ifndef DISABLE_AVISYNTH
#include "Avs2Raw.h"
#endif

class CWorker
{
public:
    CWorker() 
    {
        this->nInputFiles = 1;
        this->nTotalSizeCounter = nullptr;
    }
    virtual ~CWorker() { }
public:
    CWorkerParam *pWork;
    CEncoderPreset *preset;
    __int64 *nTotalSizeCounter;
    int nInputFiles;
    CString szInPath[6];
    CString szOutPath;
public:
    AftenAPI api;
    AftenOpt opt;
    AftenContext s;
    PcmContext pf;
public:
#ifndef DISABLE_AVISYNTH
    bool bAvisynthInput;
    AvsAudioInfo infoAVS;
#endif
public:
    void SetAftenOptions();
    void ShowCurrentJobInfo();
    int RunAftenEncoder();
    BOOL EncWork();
};

DWORD WINAPI EncWorkThread(LPVOID pParam);
