//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wiesław Šoltés <wieslaw.soltes@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

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
