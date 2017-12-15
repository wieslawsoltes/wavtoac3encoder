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

        // load avisynth.dll
        hAvisynthDLL = LoadLibrary(_T("avisynth"));
        if (!hAvisynthDLL)
        {
            // _T("Avisynth Error: Could not load avisynth.dll!"))
            return false;
        }

        // retrieve address of CreateScriptEnvironment function
        CreateEnv = (IScriptEnvironment *(__stdcall *)(int)) GetProcAddress(hAvisynthDLL,
            "CreateScriptEnvironment");
        if (!CreateEnv)
        {
            // _T("Avisynth Error: Could not access CreateScriptEnvironment!")
            return false;
        }

        // create a new scriptenvironment
        env = CreateEnv(AVISYNTH_INTERFACE_VERSION);
        if (!env)
        {
            // _T("Avisynth Error: Could not create scriptenvironment!")
            return false;
        }

        // load the Avisynth script
        AVSValue args[1] = { szAvsFileName };
        try
        {
            Video = new PClip();
            if (Video)
            {
                // NOTE: unable to debug this line in VC++
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
        catch (AvisynthError)
        {
    #ifdef _UNICODE
            // _T("Avisynth Error: Loading Avisynth script message"), e.msg
    #else
            // _T("Avisynth Error: Loading Avisynth script message: %s"), e.msg
    #endif
            delete env;
            env = nullptr;
            return false;
        }
        catch (...)
        {
            // _T("Avisynth Error: Unknown error while loading Avisynth script!")

            delete env;
            env = nullptr;
            return false;
        }

        // check for audio streams
        if ((*Video)->GetVideoInfo().HasAudio())
        {
            // convert audio samples to float (Aften uses by default FLOAT)
            if ((*Video)->GetVideoInfo().SampleType() != SAMPLE_FLOAT)
            {
                AVSValue args_conv[1] = { *Video };
                *Video = env->Invoke("ConvertAudioToFloat", AVSValue(args_conv, 1)).AsClip();
            }

            // fill info struct
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
            // _T("Avisynth Error: No audio stream!")

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
            // _T("Avisynth Error: Failed to close Avs2Raw!")
        }

        return true;
    }
    AvsAudioInfo GetInputInfo()
    {
        return info;
    }
    int GetAudio(void* pBuffer, Avs2RawStatus *pStatus)
    {
        // stop decoding when no more data available
        if (pStatus->nSamplesLeft <= 0)
            return 0;

        if (pStatus->nSamplesToRead > pStatus->nSamplesLeft)
            pStatus->nSamplesToRead = (int)pStatus->nSamplesLeft;

        // get next chunk of decoded audio data
        try
        {
            (*Video)->GetAudio(pBuffer, pStatus->nStart, pStatus->nSamplesToRead, env);
        }
        catch (AvisynthError)
        {
    #ifdef _UNICODE
            // _T("Avisynth Error: GetAudio() error message"), e.msg
    #else
            // _T("Avisynth Error: GetAudio() error message: %s"), e.msg
    #endif
            delete Video;
            delete env;
            return -1;
        }
        catch (...)
        {
            // _T("Avisynth Error: Unknown error in GetAudio()!")

            delete Video;
            delete env;
            return -1;
        }

        // update read counter
        pStatus->nStart += pStatus->nSamplesToRead;
        pStatus->nSamplesLeft -= pStatus->nSamplesToRead;

        return pStatus->nSamplesToRead;
    }
};
