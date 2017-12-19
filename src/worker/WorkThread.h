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
    CWorker() { }
    virtual ~CWorker() { }
public:
    CWorkerParam *pWork;
    __int64 nTotalSizeCounter;
    int nInputFiles;
    CString szInPath[6];
    CString szOutPath;
public:
    AftenOpt opt;
    AftenContext s;
    PcmContext pf;
public:
    bool bAvisynthInput;
#ifndef DISABLE_AVISYNTH
    AvsAudioInfo infoAVS;
#endif
public:
    static void SetAftenOptions(const CEncoderPreset *preset, const AftenAPI &api, AftenOpt &opt, AftenContext &s);
public:
    void ShowCurrentJobInfo();
    int RunAftenEncoder();
    BOOL EncWork();
};

DWORD WINAPI EncWorkThread(LPVOID pParam);
