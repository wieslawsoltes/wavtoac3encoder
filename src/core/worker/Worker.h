#pragma once

#include <utility>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <atlstr.h>
#include "logger\Log.h"
#include "configuration\Configuration.h"
#include "utilities\StringHelper.h"
#include "utilities\Utilities.h"
#include "avs\src\Avs2Raw\Avs2Raw.h"

namespace worker
{
    class CWorkerContext
    {
    public:
        int nEncodedFiles;
        __int64 nTotalSize;
        __int64 nTotalSizeCounter;
        double m_ElapsedTimeFile;
        double m_ElapsedTimeTotal;
        volatile bool bTerminate;
        volatile bool bCanUpdateWindow;
    public:
        CWorkerContext() { }
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
    };

    class CState
    {
    public:
        config::CPreset& preset;
        config::CEngine& engine;
    public:
        __int64 nInTotalSize;
        int nInputFiles;
        std::wstring szInPath[6];
        std::wstring szOutPath;
        AftenAPI api;
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
        CState(config::CPreset& preset, config::CEngine& engine) : preset(preset), engine(engine) { }
        virtual ~CState() { }
    };

    class CWorker
    {
    public:
        std::unique_ptr<worker::CWorkerContext>& pContext;
    public:
        CWorker(std::unique_ptr<worker::CWorkerContext>& pContext) : pContext(pContext) { }
        virtual ~CWorker() { }
    protected:
        void SetInfo(CState& state, config::CConfiguration* pConfig);
        bool OpenEngine(CState& state, config::CConfiguration* pConfig);
        bool CloseEngine(CState& state, config::CConfiguration* pConfig);
        void Clean(CState& state);
        bool EncoderError(CState& state, config::CConfiguration* pConfig, const std::wstring szMessage);
        bool Encode(CState& state, config::CConfiguration* pConfig);
    public:
        bool Run(config::CConfiguration* pConfig);
    };
}
