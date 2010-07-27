//
// WAV to AC3 Encoder
// Copyright (C) 2007,2008,2009,2010 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

/* $Id$ */

#include "stdafx.h"
#include "EncWAVtoAC3.h"

#include "Avs2Raw.h"

CAvs2Raw::CAvs2Raw()
{
    this->bHaveDLL = false;
    hAvisynthDLL = NULL;
    _szAvsFileName = NULL;
    memset(&this->info, 0, sizeof(AvsAudioInfo));
    CreateEnv = NULL;
    env = NULL;
    Video = NULL;
}

CAvs2Raw::~CAvs2Raw()
{
    this->CloseAvisynth();
}

bool CAvs2Raw::OpenAvisynth(const char *szAvsFileName)
{
    if(this->bHaveDLL == true)
    {
        this->CloseAvisynth();
    }

    _szAvsFileName = szAvsFileName;

    // load avisynth.dll
    hAvisynthDLL = LoadLibrary(_T("avisynth")); 
    if(!hAvisynthDLL)
    {
        //fprintf(stderr, "ERROR: couldn't load avisynth.dll\n");
        return false;
    }

    // retrieve address of createscriptenvironment function
    CreateEnv = (IScriptEnvironment *(__stdcall *)(int)) GetProcAddress(hAvisynthDLL, 
        "CreateScriptEnvironment"); 
    if(!CreateEnv)
    {
        //fprintf(stderr, "ERROR: couldn't access CreateScriptEnvironment\n");
        return false;
    }

    // create a new scriptenvironment
    env = CreateEnv(AVISYNTH_INTERFACE_VERSION);
    if(!env)
    {
        //fprintf(stderr, "ERROR: couldn't create scriptenvironment\n");
        return false;
    }

    // load the avisynth script
    AVSValue args[1] = { szAvsFileName };
    try 
    {
        Video = new PClip(); 
        if(Video)
        {
            // NOTE: unable to debug this line in VC++
            *Video = env->Invoke("Import", AVSValue(args, 1)).AsClip();
            if(!(*Video))
            {
                delete Video;
                delete env;
                return false;
            }
        }
        else
        {
            delete env;
            return false;
        }
    }  
    //catch(AvisynthError e) 
    //{
    //    fprintf(stderr, "ERROR: (Avisynth Error) loading avisynth script\n");
    //    fprintf(stderr, "%s.\n", e.msg);
    //    delete env;
    //    return false;
    //}
    catch(...) 
    {
        //fprintf(stderr, "ERROR: (Unknown) loading avisynth script\n");
        delete env;
        return false;
    }

    // check for audio streams
    if((*Video)->GetVideoInfo().HasAudio())
    {
        // convert audio samples to float (Aften uses by default FLOAT)
        if((*Video)->GetVideoInfo().SampleType() != SAMPLE_FLOAT)
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
        // fprintf(stderr, "ERROR: no audio stream\n");
        delete Video;
        delete env;

        this->bHaveDLL = true;
        return false;
    }
}

bool CAvs2Raw::CloseAvisynth()
{
    // cleanup used memory
    if(Video)
        delete Video;

    if(env)
        delete env;

    if(hAvisynthDLL)
        FreeLibrary(hAvisynthDLL);

    this->bHaveDLL = false;
    hAvisynthDLL = NULL;
    _szAvsFileName = NULL;
    memset(&this->info, 0, sizeof(AvsAudioInfo));
    CreateEnv = NULL;
    env = NULL;
    Video = NULL;

    return true;
}

AvsAudioInfo CAvs2Raw::GetInputInfo()
{
    return info;
}

int CAvs2Raw::GetAudio(void* pBuffer, Avs2RawStatus *pStatus)
{
    // stop decoding when no more data available
    if(pStatus->nSamplesLeft <= 0)
        return 0;

    if(pStatus->nSamplesToRead > pStatus->nSamplesLeft)
        pStatus->nSamplesToRead = (int) pStatus->nSamplesLeft;

    // get next chunk of decoded audio data
    try 
    {
        (*Video)->GetAudio(pBuffer, pStatus->nStart, pStatus->nSamplesToRead, env);
    }
    //catch(AvisynthError e) 
    //{
    //    fprintf(stderr, "\nERROR: (Avisynth Error) GetAudio()\n");
    //    fprintf(stderr, "%s.\n", e.msg);
    //    delete Video;
    //    delete env;
    //    return -1;
    //}
    catch(...) 
    {
        //fprintf(stderr, "\nERROR: (Unknown) GetAudio()\n");
        delete Video;
        delete env;
        return -1;
    }

    // update read counter
    pStatus->nStart += pStatus->nSamplesToRead;
    pStatus->nSamplesLeft -= pStatus->nSamplesToRead;

    return pStatus->nSamplesToRead;
}
