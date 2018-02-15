#pragma once

#include <utility>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <atlstr.h>
#include "configuration\Configuration.h"
#include "utilities\StringHelper.h"
#include "utilities\Utilities.h"
#include "avs\src\Avs2Raw.h"

namespace worker
{
    class CWorkerContext;

    class CWorker
    {
    public:
        CWorkerContext* pContext;
    private:
        __int64 nTotalSizeCounter;
        int nInputFiles;
        std::wstring szInPath[6];
        std::wstring szOutPath;
        AftenOpt opt;
        AftenContext s;
        PcmContext pf;
        uint8_t *frame;
        FLOAT *fwav;
        FILE *ifp[6];
        FILE *ofp;
        bool bAvisynthInput;
        AvsAudioInfo infoAVS;
        CAvs2Raw decoderAVS;
        Avs2RawStatus statusAVS;
    public:
        CWorker(CWorkerContext* pContext) : pContext(pContext) { }
        virtual ~CWorker() { }
    public:
        void InitContext(const config::CEngine *engine, const config::CPreset *preset, const AftenAPI &api, AftenOpt &opt, AftenContext &s);
        void UpdateProgress();
        bool HandleError(LPTSTR pszMessage);
        bool Run();
        bool Encode();
    };

    class CWorkerContext
    {
    public:
        std::thread m_Thread;
        std::timed_mutex m_Mutex;
    public:
        config::CConfiguration * pConfig;
        AftenAPI api;
        std::vector<std::wstring> m_Files;
        std::vector<bool> m_Status;
        config::CPreset * pPreset;
        config::CEngine * pEngine;
        bool bUseOutPath;
        std::wstring szOutPath;
        bool bMultiMonoInput;
        int nThreads;
        __int64 nInTotalSize;
        __int64 nOutTotalSize;
        volatile bool bTerminate;
        volatile bool bCanUpdateWindow;
        __int64 nTotalSize;
        double m_ElapsedTimeFile;
        double m_ElapsedTimeTotal;
        int nCount;
    public:
        CWorkerContext(config::CConfiguration * pConfig) : pConfig(pConfig) { }
        virtual ~CWorkerContext() { }
    public:
        virtual void SetTitleInfo(std::wstring szInfo) = 0;
        virtual void SetInputFileInfo(int nID, std::wstring szInfo) = 0;
        virtual void SetInputTypeInfo(int nID, std::wstring szInfo) = 0;
        virtual void SetOutputFileInfo(std::wstring szInfo) = 0;
        virtual void SetOutputTypeInfo(std::wstring szInfo) = 0;
        virtual void SetCurrentTimerInfo(std::wstring szInfo) = 0;
        virtual void SetTotalTimerInfo(std::wstring szInfo) = 0;
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
    public:
        virtual void Start()
        {
            this->m_Thread = std::thread([this]()
            {
                std::unique_lock<std::timed_mutex> lk(this->m_Mutex);
                try
                {
                    CWorker m_Worker(this);
                    m_Worker.Encode();
                }
                catch (...) { }
                this->bTerminate = true;
                this->Close();
            });
            this->m_Thread.detach();
        }
        virtual void Wait(int ms = 0)
        {
            if (ms == 0)
                std::unique_lock<std::timed_mutex> lk(this->m_Mutex);
            else
                std::unique_lock<std::timed_mutex> lk(this->m_Mutex, std::chrono::milliseconds(ms));
        }
    };
}
