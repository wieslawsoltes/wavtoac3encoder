#pragma once

#include "AftenAPI.h"

class AftenOpt
{
public:
    int pad_start;
    int chmap;
    int read_to_eof;
    int raw_input;
    int raw_sr;
    int raw_ch;
    enum PcmSampleFormat raw_fmt;
    int raw_order;
};

class SingleWorkerData
{
public:
    CString szInPath;
    CString szOutPath;
    AftenOpt opt;
    AftenContext s;
    int nProgress;
    double fEncTime;
    bool bTerminate;
    int nID;
    bool bSuccess;
};

#define WM_WORKER_PROGRESS WM_USER + 0x10

DWORD WINAPI EncWorkThread(LPVOID pParam);
