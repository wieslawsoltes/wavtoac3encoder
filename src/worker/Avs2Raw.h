#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

// include Avisynth headers
#if defined(_WIN32) & !defined(_WIN64)
#include "avisynth\src\internal.h"
#else
#include "avisynth64\src\internal.h"
#endif // _WIN32

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
    CAvs2Raw();
    virtual ~CAvs2Raw();
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
    bool OpenAvisynth(const char *szAvsFileName);
    bool CloseAvisynth();
public:
    AvsAudioInfo GetInputInfo();
    int GetAudio(void* pBuffer, Avs2RawStatus *pStatus);
};
