#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#if defined(_WIN32) & !defined(_WIN64)
#include "avisynth\src\internal.h"
#else
#include "avisynth64\src\internal.h"
#endif

typedef struct TAvsAudioInfo
{
    int nSampleType;
    int nSamplesPerSecond;
    int nAudioChannels;
    __int64 nAudioSamples;
    int nBytesPerChannelSample;
} AvsAudioInfo, *PAvsAudioInfo;

typedef struct TAvs2RawStatus
{
    __int64 nStart;
    __int64 nSamples;
    __int64 nSamplesLeft;
    int nSamplesToRead;
} Avs2RawStatus, *PAvs2RawStatus;

class CAvs2Raw
{
public:
    CAvs2Raw()
    {
        this->bHaveDLL = false;
        hAvisynthDLL = nullptr;
        _szAvsFileName = nullptr;
        memset(&this->info, 0, sizeof(AvsAudioInfo));
        CreateEnv = nullptr;
        env = nullptr;
        Video = nullptr;
    }
    virtual ~CAvs2Raw()
    {
        this->CloseAvisynth();
    }
private:
    HINSTANCE hAvisynthDLL;
    IScriptEnvironment* (__stdcall *CreateEnv) (int);
    IScriptEnvironment *env;
    PClip *Video;
private:
    AvsAudioInfo info;
    const char *_szAvsFileName;
    bool bHaveDLL;
public:
    bool OpenAvisynth(const char *szAvsFileName)
    {
        if (this->bHaveDLL == true)
        {
            this->CloseAvisynth();
        }

        _szAvsFileName = szAvsFileName;

        hAvisynthDLL = LoadLibrary(_T("avisynth"));
        if (!hAvisynthDLL)
        {
            OutputDebugString(_T("Avisynth Error: Could not load avisynth.dll!"));
            return false;
        }

        CreateEnv = (IScriptEnvironment *(__stdcall *)(int)) GetProcAddress(hAvisynthDLL,
            "CreateScriptEnvironment");
        if (!CreateEnv)
        {
            OutputDebugString(_T("Avisynth Error: Could not access CreateScriptEnvironment!"));
            return false;
        }

        env = CreateEnv(AVISYNTH_INTERFACE_VERSION);
        if (!env)
        {
            OutputDebugString(_T("Avisynth Error: Could not create scriptenvironment!"));
            return false;
        }

        AVSValue args[1] = { szAvsFileName };
        try
        {
            Video = new PClip();
            if (Video)
            {
                *Video = env->Invoke("Import", AVSValue(args, 1)).AsClip();
                if (!(*Video))
                {
                    delete Video;
                    delete env;
                    Video = nullptr;
                    env = nullptr;
                    return false;
                }
            }
            else
            {
                delete env;
                env = nullptr;
                return false;
            }
        }
        catch (AvisynthError e)
        {
            OutputDebugString(_T("Avisynth Error: Loading Avisynth script message: "));
            OutputDebugStringA(e.msg);
            delete env;
            env = nullptr;
            return false;
        }
        catch (...)
        {
            OutputDebugString(_T("Avisynth Error: Unknown error while loading Avisynth script!"));
            delete env;
            env = nullptr;
            return false;
        }

        if ((*Video)->GetVideoInfo().HasAudio())
        {
            if ((*Video)->GetVideoInfo().SampleType() != SAMPLE_FLOAT)
            {
                AVSValue args_conv[1] = { *Video };
                *Video = env->Invoke("ConvertAudioToFloat", AVSValue(args_conv, 1)).AsClip();
            }

            this->info.nAudioChannels = (*Video)->GetVideoInfo().AudioChannels();
            this->info.nAudioSamples = (*Video)->GetVideoInfo().num_audio_samples;
            this->info.nBytesPerChannelSample = (*Video)->GetVideoInfo().BytesPerChannelSample();
            this->info.nSamplesPerSecond = (*Video)->GetVideoInfo().SamplesPerSecond();
            this->info.nSampleType = (*Video)->GetVideoInfo().SampleType();

            this->bHaveDLL = true;
            return true;
        }
        else
        {
            OutputDebugString(_T("Avisynth Error: No audio stream!"));
            delete Video;
            delete env;
            Video = nullptr;
            env = nullptr;
            this->bHaveDLL = true;
            return false;
        }
    }
    bool CloseAvisynth()
    {
        try
        {
            if (Video)
            {
                delete Video;
                Video = nullptr;
            }

            if (env)
            {
                delete env;
                env = nullptr;
            }

            if (hAvisynthDLL)
                FreeLibrary(hAvisynthDLL);

            this->bHaveDLL = false;
            hAvisynthDLL = nullptr;
            _szAvsFileName = nullptr;
            memset(&this->info, 0, sizeof(AvsAudioInfo));
            CreateEnv = nullptr;
            env = nullptr;
            Video = nullptr;
        }
        catch (...)
        {
            OutputDebugString(_T("Avisynth Error: Failed to close Avs2Raw!"));
        }

        return true;
    }
    AvsAudioInfo GetInputInfo()
    {
        return info;
    }
    int GetAudio(void* pBuffer, Avs2RawStatus *pStatus)
    {
        if (pStatus->nSamplesLeft <= 0)
            return 0;

        if (pStatus->nSamplesToRead > pStatus->nSamplesLeft)
            pStatus->nSamplesToRead = (int)pStatus->nSamplesLeft;

        try
        {
            (*Video)->GetAudio(pBuffer, pStatus->nStart, pStatus->nSamplesToRead, env);
        }
        catch (AvisynthError e)
        {
            OutputDebugString(_T("Avisynth Error: GetAudio() error message: "));
            OutputDebugStringA(e.msg);
            delete Video;
            delete env;
            return -1;
        }
        catch (...)
        {
            OutputDebugString(_T("Avisynth Error: Unknown error in GetAudio()!"));
            delete Video;
            delete env;
            return -1;
        }

        pStatus->nStart += pStatus->nSamplesToRead;
        pStatus->nSamplesLeft -= pStatus->nSamplesToRead;
        return pStatus->nSamplesToRead;
    }
};
