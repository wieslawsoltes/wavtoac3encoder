#pragma once

#include "configuration\Configuration.h"
#include "utilities\ListT.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"
#include "utilities\TimeCount.h"
#ifndef DISABLE_AVISYNTH
#include "Avs2Raw.h"
#endif

class CWorkerParam
{
public:
    AftenAPI api;
public:
    CListT<CString> *m_FilesList;
    CListT<bool> *m_StatusList;
public:
    CEncoderPreset *m_Preset;
public:
    bool bUseOutPath;
    CString szOutPath;
    bool bMultiMonoInput;
    int nThreads;
public:
    double fTimeTotal;
    double fTimeEncoding;
    double fTimeIORead;
    double fTimeIOWrite;
    __int64 nInTotalSize;
    __int64 nOutTotalSize;
public:
    volatile bool bTerminate;
    volatile bool bCanUpdateWindow;
    HANDLE hThread;
    DWORD dwThreadId;
    __int64 nTotalSize;
    double m_ElapsedTimeFile;
    double m_ElapsedTimeTotal;
    int nCount;
public:
    CString szSpeedEncoderAvg;
    CString szSpeedReadsAvg;
    CString szSpeedWritesAvg;
};

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
