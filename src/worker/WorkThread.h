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
    void SetAftenOptions(AftenAPI &api, AftenContext &s, CEncoderPreset *preset, AftenOpt &opt, CWorkerParam *pWork);
#ifndef DISABLE_AVISYNTH
    void ShowCurrentJobInfo(int nInputFiles, PcmContext &pf, CWorkerParam *pWork, AftenContext &s, bool bAvisynthInput, AvsAudioInfo &infoAVS);
#else
    void ShowCurrentJobInfo(int nInputFiles, PcmContext &pf, CWorkerParam *pWork, AftenContext &s, bool bAvisynthInput);
#endif
    int RunAftenEncoder(AftenAPI &api, AftenContext &s, AftenOpt &opt, CWorkerParam *pWork, CString szInPath[6], CString szOutPath, int nInputFiles = 1, __int64 *nTotalSizeCounter = nullptr);
    BOOL EncWork(CWorkerParam *pWork);
};

DWORD WINAPI EncWorkThread(LPVOID pParam);
