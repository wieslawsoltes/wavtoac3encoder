#pragma once

typedef struct TAftenOpt
{
    int pad_start;
    int chmap;
    int read_to_eof;
    int raw_input;
    int raw_sr;
    int raw_ch;
    enum PcmSampleFormat raw_fmt;
    int raw_order;
} AftenOpt;

typedef struct TSingleWorkerData
{
    // input and output files
    CString szInPath;
    CString szOutPath;
    // aften related data
    AftenOpt opt;
    AftenContext s;
    // work stats
    int nProgress;
    double fEncTime;
    // termination flag
    bool bTerminate;
    // file id
    int nID;
    // success flag
    bool bSuccess;
} SingleWorkerData;

#define WM_WORKER_PROGRESS WM_USER + 0x10

DWORD WINAPI EncWorkThread(LPVOID pParam);
