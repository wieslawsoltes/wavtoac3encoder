#pragma once

#include "configuration\Configuration.h"
#include "utilities\ListT.h"
#include "utilities\Utilities.h"
#include "utilities\MyFile.h"
#include "utilities\TimeCount.h"
#ifndef DISABLE_AVISYNTH
#include "Avs2Raw.h"
#endif

class CWorkerContext
{
public:
    CConfiguration *pConfig;
public:
    AftenAPI api;
public:
    CListT<CString> *pFilesList;
    CListT<bool> *pStatusList;
public:
    CEncoderPreset * pPreset;
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
public:
    CWorkerContext() { }
    virtual ~CWorkerContext() { }
public:
    virtual void SetTitleInfo(CString szInfo) = 0;
    virtual void SetInputFileInfo(int nID, CString szInfo) = 0;
    virtual void SetInputTypeInfo(int nID, CString szInfo) = 0;
    virtual void SetOutputFileInfo(CString szInfo) = 0;
    virtual void SetOutputTypeInfo(CString szInfo) = 0;
    virtual void SetSimdInfo(CString szInfo) = 0;
    virtual void SetCurrentTimerInfo(CString szInfo) = 0;
    virtual void SetTotalTimerInfo(CString szInfo) = 0;
public:
    virtual void SetCurrentProgressRange(int nMin, int nMax) = 0;
    virtual void SetTotalProgressRange(int nMin, int nMax) = 0;
    virtual void SetCurrentProgress(int nPos) = 0;
    virtual void SetTotalProgress(int nPos) = 0;
public:
    virtual void StartCurrentTimer(int nResolution) = 0;
    virtual void StopCurrentTimer() = 0;
    virtual void StartTotalTimer(int nResolution) = 0;
    virtual void StopTotalTimer() = 0;
public:
    virtual void Close() = 0;
};

class CWorker
{
public:
    CWorker(CWorkerContext* pContext)
    { 
        this->pContext = pContext;
    }
    virtual ~CWorker() { }
public:
    CWorkerContext *pContext;
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
