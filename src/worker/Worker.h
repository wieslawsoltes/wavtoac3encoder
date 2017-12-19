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
private:
    __int64 nTotalSizeCounter;
    int nInputFiles;
    CString szInPath[6];
    CString szOutPath;
private:
    AftenOpt opt;
    AftenContext s;
    PcmContext pf;
    uint8_t *frame;
    FLOAT *fwav;
    FILE *ifp[CEncoderDefaults::nNumMaxInputFiles];
    FILE *ofp;
private:
    bool bAvisynthInput;
#ifndef DISABLE_AVISYNTH
    AvsAudioInfo infoAVS;
    CAvs2Raw decoderAVS;
    Avs2RawStatus statusAVS;
#endif
public:
    static void InitContext(const CEncoderPreset *preset, const AftenAPI &api, AftenOpt &opt, AftenContext &s);
public:
    void UpdateProgress();
    BOOL HandleError(LPTSTR pszMessage);
    int Run();
    BOOL Encode();
};

DWORD WINAPI EncWorkThread(LPVOID pParam);
