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
		::LogMessage(_T("Avisynth Error: Could not load avisynth.dll!"));
        return false;
    }
	
    // retrieve address of CreateScriptEnvironment function
    CreateEnv = (IScriptEnvironment *(__stdcall *)(int)) GetProcAddress(hAvisynthDLL, 
        "CreateScriptEnvironment"); 
    if(!CreateEnv)
    {
		::LogMessage(_T("Avisynth Error: Could not access CreateScriptEnvironment!"));
        return false;
    }

    // create a new scriptenvironment
    env = CreateEnv(AVISYNTH_INTERFACE_VERSION);
    if(!env)
    {
		::LogMessage(_T("Avisynth Error: Could not create scriptenvironment!"));
        return false;
    }

    // load the Avisynth script
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
				Video = NULL;
				env = NULL;
                return false;
            }
        }
        else
        {
            delete env;
			env = NULL;
            return false;
        }
    }  
    catch(AvisynthError e) 
    {
#ifdef _UNICODE
		::LogAnsiMessage(_T("Avisynth Error: Loading Avisynth script message"), e.msg);
#else
		CString szBuff;
		szBuff.Format(_T("Avisynth Error: Loading Avisynth script message: %s"), e.msg);
		::LogMessage(szBuff);
#endif
        delete env;
	    env = NULL;
        return false;
    }
    catch(...) 
    {
		::LogMessage(_T("Avisynth Error: Unknown error while loading Avisynth script!"));

        delete env;
		env = NULL;
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
		::LogMessage(_T("Avisynth Error: No audio stream!"));

        delete Video;
        delete env;
		Video = NULL;
		env = NULL;
        this->bHaveDLL = true;
        return false;
    }
}

bool CAvs2Raw::CloseAvisynth()
{
	try
	{
		// cleanup used memory
		if (Video)
		{
			delete Video;
			Video = NULL;
		}

		if (env)
		{
			delete env;
			env = NULL;
		}

		if (hAvisynthDLL)
			FreeLibrary(hAvisynthDLL);

		this->bHaveDLL = false;
		hAvisynthDLL = NULL;
		_szAvsFileName = NULL;
		memset(&this->info, 0, sizeof(AvsAudioInfo));
		CreateEnv = NULL;
		env = NULL;
		Video = NULL;
	}
	catch (...) 
	{ 
		::LogMessage(_T("Avisynth Error: Failed to close Avs2Raw!"));
	}

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
    catch(AvisynthError e) 
    {
#ifdef _UNICODE
		::LogAnsiMessage(_T("Avisynth Error: GetAudio() error message"), e.msg);
#else
		CString szBuff;
		szBuff.Format(_T("Avisynth Error: GetAudio() error message: %s"), e.msg);
		::LogMessage(szBuff);
#endif
        delete Video;
        delete env;
        return -1;
    }
    catch(...) 
    {
		::LogMessage(_T("Avisynth Error: Unknown error in GetAudio()!"));

        delete Video;
        delete env;
        return -1;
    }

    // update read counter
    pStatus->nStart += pStatus->nSamplesToRead;
    pStatus->nSamplesLeft -= pStatus->nSamplesToRead;

    return pStatus->nSamplesToRead;
}
