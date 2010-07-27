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
#include "EncWorkThread.h"
#include "EncWAVtoAC3WorkDlg.h"
#include "EncoderOptions.h"
#include "Utilities.h"
#include "MyFile.h"

#ifndef DISABLE_AVISYNTH
#include "Avs2Raw.h"
#endif

CList<SingleWorkerData,SingleWorkerData> workList;

CRITICAL_SECTION csQue;
int nNextInQue;
int nTotalFiles;
HANDLE *phThreads;
DWORD *pdwThreadsID;
DWORD dwWorkThreads;
bool *pbTerminate;

void SetAftenOptions(AftenAPI &api, 
                     AftenContext &s, 
                     EncoderPreset &preset, 
                     AftenOpt &opt,
                     WorkerParam *pWork)
{
    // get default settings from aften library
    api.LibAften_aften_set_defaults(&s);

    // set Aften qaulity options
    s.params.encoding_mode = preset.nMode;
    s.params.bitrate = preset.nBitrate;
    s.params.quality = preset.nQuality;

    // set number of Aften threads
    s.system.n_threads = preset.nThreads;

    // enable of disable specific SIMD instructions
    s.system.wanted_simd_instructions.mmx = preset.nUsedSIMD[0];
    s.system.wanted_simd_instructions.sse = preset.nUsedSIMD[1];
    s.system.wanted_simd_instructions.sse2 = preset.nUsedSIMD[2];
    s.system.wanted_simd_instructions.sse3 = preset.nUsedSIMD[3];

    // set raw audio input sample format
    if(preset.nRawSampleFormat != 0)
    {
        switch(preset.nRawSampleFormat)
        {
        case 1: opt.raw_fmt = PCM_SAMPLE_FMT_U8; opt.raw_order = PCM_BYTE_ORDER_LE; break; // u8
        case 2: opt.raw_fmt = PCM_SAMPLE_FMT_S8; opt.raw_order = PCM_BYTE_ORDER_LE; break; // s8
        case 3: opt.raw_fmt = PCM_SAMPLE_FMT_S16; opt.raw_order = PCM_BYTE_ORDER_LE; break; // s16_le
        case 4: opt.raw_fmt = PCM_SAMPLE_FMT_S16; opt.raw_order = PCM_BYTE_ORDER_BE; break; // s16_be
        case 5: opt.raw_fmt = PCM_SAMPLE_FMT_S20; opt.raw_order = PCM_BYTE_ORDER_LE; break; // s20_le
        case 6: opt.raw_fmt = PCM_SAMPLE_FMT_S20; opt.raw_order = PCM_BYTE_ORDER_BE; break; // s20_be
        case 7: opt.raw_fmt = PCM_SAMPLE_FMT_S24; opt.raw_order = PCM_BYTE_ORDER_LE; break; // s24_le
        case 8: opt.raw_fmt = PCM_SAMPLE_FMT_S24; opt.raw_order = PCM_BYTE_ORDER_BE; break; // s24_be
        case 9: opt.raw_fmt = PCM_SAMPLE_FMT_S32; opt.raw_order = PCM_BYTE_ORDER_LE; break; // s32_le
        case 10: opt.raw_fmt = PCM_SAMPLE_FMT_S32; opt.raw_order = PCM_BYTE_ORDER_BE; break; // s32_be
        case 11: opt.raw_fmt = PCM_SAMPLE_FMT_FLT; opt.raw_order = PCM_BYTE_ORDER_LE; break; // float_le
        case 12: opt.raw_fmt = PCM_SAMPLE_FMT_FLT; opt.raw_order = PCM_BYTE_ORDER_BE; break; // float_be
        case 13: opt.raw_fmt = PCM_SAMPLE_FMT_DBL; opt.raw_order = PCM_BYTE_ORDER_LE; break; // double_le
        case 14: opt.raw_fmt = PCM_SAMPLE_FMT_DBL; opt.raw_order = PCM_BYTE_ORDER_BE; break; // double_be
        };

        opt.raw_input = 1;
    }

    // set raw audio input sample rate
    if(preset.nRawSampleRate != 0)
    {
        opt.raw_sr = preset.nRawSampleRate;

        opt.raw_input = 1;
    }

    // set raw audio input channels
    if(preset.nRawChannels != 0)
    {
        opt.raw_ch = preset.nRawChannels;

        opt.raw_input = 1;
    }

    // holds current setting index
    int nSetting;

    // use this macro to prepare aften settings
    #define SET_AFTEN_SETTING(set, type) \
    if(encOpt[nSetting].nIgnoreValue != preset.nSetting[nSetting]) \
    (set) = (type) encOpt[nSetting].listOptValues.GetAt(encOpt[nSetting].listOptValues.FindIndex(preset.nSetting[nSetting]));

    // process all aften options for encoder context

    // fba
    nSetting = 0; SET_AFTEN_SETTING(s.params.bitalloc_fast, int)

    // exps
    nSetting++; SET_AFTEN_SETTING(s.params.expstr_search, int)

    // pad
    nSetting++; SET_AFTEN_SETTING(opt.pad_start, int)

    // w
    nSetting++; SET_AFTEN_SETTING(s.params.bwcode, int)

    // wmin
    nSetting++; SET_AFTEN_SETTING(s.params.min_bwcode, int)

    // wmax
    nSetting++; SET_AFTEN_SETTING(s.params.max_bwcode, int)

    // m
    nSetting++; SET_AFTEN_SETTING(s.params.use_rematrixing, int)

    // s
    nSetting++; SET_AFTEN_SETTING(s.params.use_block_switching, int)

    // cmix
    nSetting++; SET_AFTEN_SETTING(s.meta.cmixlev, int)

    // smix
    nSetting++; SET_AFTEN_SETTING(s.meta.surmixlev, int)

    // dsur
    nSetting++; SET_AFTEN_SETTING(s.meta.dsurmod, int)

    // dnorm
    nSetting++; SET_AFTEN_SETTING(s.meta.dialnorm, int)

    // dynrng
    nSetting++; SET_AFTEN_SETTING(s.params.dynrng_profile, DynRngProfile)

    // acmod
    nSetting++; SET_AFTEN_SETTING(s.acmod, int)

    // lfe
    nSetting++; SET_AFTEN_SETTING(s.lfe, int)

    // chconfig
    nSetting++; 
    if(encOpt[nSetting].nIgnoreValue != preset.nSetting[nSetting])
    {
        s.acmod = ccAften[encOpt[nSetting].listOptValues.GetAt(encOpt[nSetting].listOptValues.FindIndex(preset.nSetting[nSetting]))].acmod;
        s.lfe = ccAften[encOpt[nSetting].listOptValues.GetAt(encOpt[nSetting].listOptValues.FindIndex(preset.nSetting[nSetting]))].lfe;
    }

    // chmap
    nSetting++; SET_AFTEN_SETTING(opt.chmap, int)

    // readtoeof
    nSetting++; SET_AFTEN_SETTING(opt.read_to_eof, int)

    // bwfilter
    nSetting++; SET_AFTEN_SETTING(s.params.use_bw_filter, int)

    // dcfilter
    nSetting++; SET_AFTEN_SETTING(s.params.use_dc_filter, int)

    // lfefilter
    nSetting++; SET_AFTEN_SETTING(s.params.use_lfe_filter, int)

    // xbsi1
    nSetting++; SET_AFTEN_SETTING(s.meta.xbsi1e, int)

    // dmixmod
    nSetting++; SET_AFTEN_SETTING(s.meta.dmixmod, int)

    // ltrtcmix
    nSetting++; SET_AFTEN_SETTING(s.meta.ltrtcmixlev, int)

    // ltrtsmix
    nSetting++; SET_AFTEN_SETTING(s.meta.ltrtsmixlev, int)

    // lorocmix
    nSetting++; SET_AFTEN_SETTING(s.meta.lorocmixlev, int)

    // lorosmix
    nSetting++; SET_AFTEN_SETTING(s.meta.lorosmixlev, int)

    // xbsi2
    nSetting++; SET_AFTEN_SETTING(s.meta.xbsi2e, int)

    // dsurexmod
    nSetting++; SET_AFTEN_SETTING(s.meta.dsurexmod, int)

    // dheadphon
    nSetting++; SET_AFTEN_SETTING(s.meta.dheadphonmod, int)

    // adconvtyp
    nSetting++; SET_AFTEN_SETTING(s.meta.adconvtyp, int)
}

#ifndef DISABLE_AVISYNTH
void ShowCurrentJobInfo(int nInputFiles,  
                        PcmContext &pf, 
                        WorkerParam *pWork, 
                        AftenContext &s,
                        bool bAvisynthInput,
                        AvsAudioInfo &infoAVS)
#else
void ShowCurrentJobInfo(int nInputFiles,  
                        PcmContext &pf, 
                        WorkerParam *pWork, 
                        AftenContext &s,
                        bool bAvisynthInput)
#endif
{
    CString szInputInfo = _T("");
    CString szOutputInfo = _T("");
    CString szSimdInfo = _T("");

    // input info (using code from pcm/pcm.c)
    if(bAvisynthInput == false)
    {
        for(int i = 0; i < nInputFiles; i++)
        {
            PcmFile *pf_info = &pf.pcm_file[i];
            TCHAR *type, *chan, *order;
            TCHAR fmt[64] = _T(""); 

            type = _T("?");
            chan = _T("?-channel");
            order = _T("");

            if(pf_info->sample_type == PCM_SAMPLE_TYPE_INT) 
            {
                if(pf_info->source_format == PCM_SAMPLE_FMT_U8) 
                    type = _T("Unsigned");
                else 
                    type = _T("Signed");
            } 
            else if(pf_info->sample_type == PCM_SAMPLE_TYPE_FLOAT) 
            {
                type = _T("Floating-point");
            } 
            else 
            {
                type = _T("[unsupported type]");
            }

            if(pf_info->ch_mask & 0x08) 
            {
                switch(pf_info->channels-1) 
                {
                case 1: chan = _T("1.1-channel"); break;
                case 2: chan = _T("2.1-channel"); break;
                case 3: chan = _T("3.1-channel"); break;
                case 4: chan = _T("4.1-channel"); break;
                case 5: chan = _T("5.1-channel"); break;
                default: chan = _T("multi-channel with LFE"); break;
                }
            } 
            else 
            {
                switch(pf_info->channels) 
                {
                case 1: chan = _T("mono"); break;
                case 2: chan = _T("stereo"); break;
                case 3: chan = _T("3-channel"); break;
                case 4: chan = _T("4-channel"); break;
                case 5: chan = _T("5-channel"); break;
                case 6: chan = _T("6-channel"); break;
                default: chan = _T("multi-channel"); break;
                }
            }

            if(pf_info->pcm_format)
            {
#ifdef _UNICODE
                ConvertAnsiToUnicode(pf_info->pcm_format->long_name, fmt, strlen(pf_info->pcm_format->long_name));
#else
                sprintf(fmt, _T("%s"), pf_info->pcm_format->long_name);
#endif
            }
            else
            {
                _stprintf(fmt, _T("%s"), _T("unknown"));
            }

            if(pf_info->source_format > PCM_SAMPLE_FMT_S8) 
            {
                switch(pf_info->order) 
                {
                case PCM_BYTE_ORDER_LE: order = _T("little-endian"); break;
                case PCM_BYTE_ORDER_BE: order = _T("big-endian"); break;
                }
            } 
            else 
            {
                order = _T("\b");
            }

            szInputInfo.Format(_T("\t%s %s %d-bit %s %d Hz %s"), 
                fmt, type, pf_info->bit_width, order, pf_info->sample_rate, chan);

            pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDInInfo[i])->SetWindowText(szInputInfo);
        }
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        // NOTE: type is always Raw PCM but floating-point may change in future and endianes
        TCHAR *chan;
        chan = _T("?-channel");

        switch(infoAVS.nAudioChannels) 
        {
        case 1: chan = _T("mono"); break;
        case 2: chan = _T("stereo"); break;
        case 3: chan = _T("3-channel"); break;
        case 4: chan = _T("4-channel"); break;
        case 5: chan = _T("5-channel"); break;
        case 6: chan = _T("6-channel"); break;
        default: chan = _T("multi-channel"); break;
        }

        szInputInfo.Format(_T("\tAvisynth: Raw PCM Floating-point 32-bit little-endian %d Hz %s"), 
            infoAVS.nSamplesPerSecond, chan);

        pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDInInfo[0])->SetWindowText(szInputInfo);
#endif
    }

    // output info (using code from aften/aften.c)
    {
        static const TCHAR *acmod_str[8] = 
        { 
            _T("dual mono (1+1)"), 
            _T("mono (1/0)"), 
            _T("stereo (2/0)"), 
            _T("3/0"), 
            _T("2/1"), 
            _T("3/1"), 
            _T("2/2"), 
            _T("3/2")
        };

        szOutputInfo.Format(_T("\tAC3 %d Hz %s"), s.samplerate, acmod_str[s.acmod]);
        if(s.lfe) 
            szOutputInfo += _T(" + LFE");
    }

    // SIMD usage info
    {
        int nCountSimd = 0;
        szSimdInfo = _T("SIMD:");

        if(s.system.wanted_simd_instructions.mmx && s.system.available_simd_instructions.mmx) 
        { 
            szSimdInfo += _T(" MMX"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.sse && s.system.available_simd_instructions.sse) 
        { 
            szSimdInfo += _T(" SSE"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.sse2 && s.system.available_simd_instructions.sse2) 
        { 
            szSimdInfo += _T(" SSE2"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.sse3 && s.system.available_simd_instructions.sse3) 
        { 
            szSimdInfo += _T(" SSE3"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.ssse3 && s.system.available_simd_instructions.ssse3) 
        { 
            szSimdInfo += _T(" SSSE3"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.amd_3dnow && s.system.available_simd_instructions.amd_3dnow) 
        { 
            szSimdInfo += _T(" 3DNOW"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.amd_3dnowext && s.system.available_simd_instructions.amd_3dnowext) 
        { 
            szSimdInfo += _T(" 3DNOWEXT"); nCountSimd++; 
        }
        if(s.system.wanted_simd_instructions.amd_sse_mmx && s.system.available_simd_instructions.amd_sse_mmx) 
        { 
            szSimdInfo += _T(" SSE-MMX"); nCountSimd++; 
        }

        if(nCountSimd == 0) 
            szSimdInfo += _T(" NONE");

        CString szBuff;
        if(s.system.n_threads == 0)
            szBuff = _T(" | Threads: Auto");
        else
            szBuff.Format(_T(" | Threads: %i"), s.system.n_threads);

        szSimdInfo += szBuff;
    }

    // update work dialog informations
    pWork->pWorkDlg->m_StcOutInfo.SetWindowText(szOutputInfo);
    pWork->pWorkDlg->m_StcSimdInfo.SetWindowText(szSimdInfo);
}

int RunAftenEncoder(AftenAPI &api, 
                    AftenContext &s, 
                    AftenOpt &opt, 
                    WorkerParam *pWork, 
                    CString szInPath[6], 
                    CString szOutPath, 
                    int nInputFiles = 1, 
                    __int64 *nTotalSizeCounter = NULL,
                    SingleWorkerData *pworkData = NULL)
{
	// default prefix for encoder log messages
	const CString szLogMessage = _T("Encoder Error: ");

    // function is using modified code from aften.c (C) by Justin Ruggles
    void (*aften_remap)(void *samples, int n, int ch, A52SampleFormat fmt, int acmod) = NULL;
    uint8_t *frame = NULL;
    FLOAT *fwav = NULL;
    int nr, fs;
    FILE *ifp[NUM_MAX_INPUT_FILES];
    FILE *ofp = NULL;
    PcmContext pf;
    uint32_t samplecount, bytecount, t0, t1, percent;
    FLOAT kbps, qual, bw;
    int last_frame;
    int frame_cnt;
    int done;
    int input_file_format;
    enum PcmSampleFormat read_format;

    // indicate avisynth script as input file
    bool bAvisynthInput = false;

#ifndef DISABLE_AVISYNTH
    // check if we have avisynth script as input
    if(GetFileExt(szInPath[0]).MakeLower() == _T("avs"))
        bAvisynthInput = true;
#endif

	// total size of input file(s)
    pWork->nInTotalSize = 0;

    // encoding, IO reading, IO writing counters
    CMyCounter cEncoding, cIORead, cIOWrite;

    // init counters
    cEncoding.Init();
    cIORead.Init();
    cIOWrite.Init();

    // get string buffers
    TCHAR *pszInPath[6] = { { NULL } };
    TCHAR *pszOutPath = NULL;

    for(int i = 0; i < nInputFiles; i++)
        pszInPath[i] = szInPath[i].GetBuffer();

    pszOutPath = szOutPath.GetBuffer();
    memset(ifp, 0, NUM_MAX_INPUT_FILES * sizeof(FILE *));

#ifndef DISABLE_AVISYNTH
    // avisynth data
    AvsAudioInfo infoAVS;
    Avs2RawStatus statusAVS;
    CAvs2Raw decoderAVS;
    char szInputFileAVS[MAX_PATH] = "";
#endif

    if(bAvisynthInput == true)
	{
#ifndef DISABLE_AVISYNTH

        // initialize Avisynth - only one input file supported
        // NOTE: only Ansi file names supported
#ifdef _UNICODE
        ConvertUnicodeToAnsi(pszInPath[0], szInputFileAVS, lstrlen(pszInPath[0])); 
        if(decoderAVS.OpenAvisynth(szInputFileAVS) == false)
#else
        if(decoderAVS.OpenAvisynth(pszInPath[0]) == false)
#endif
        {
            if(logCtx.bInit)
            {
                ::LogMessage(&logCtx, szLogMessage + _T("Failed to initialize Avisynth."));
            }

            return(WORKDLG_RETURN_FAILURE);
        }
        else
        {
            // get input Audio stream information from Avisynth
            infoAVS = decoderAVS.GetInputInfo();

            // calculate total size of input raw data
            pWork->nInTotalSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;

            if(logCtx.bInit)
            {
                ::LogMessage(&logCtx, _T("Avisynth initialized successfully."));
            }
        }
#endif
    }
    else
    {
        // open input files
        for(int i = 0; i < nInputFiles; i++)
        {
            ifp[i] = _tfopen(pszInPath[i], _T("rb"));
            if(!ifp[i]) 
            {
                // stop file timer
				pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
				pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(_T("Elapsed time: 00:00:00"));
				pWork->pWorkDlg->m_ElapsedTimeFile = 0L;

                if(logCtx.bInit)
                { 
                    ::LogMessage(&logCtx, szLogMessage + _T("Failed to open input file:") + pszInPath[i]);
                }

                pWork->pWorkDlg->bTerminate = true;
                ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

                return(WORKDLG_RETURN_FAILURE);
            }
            else
            {
                pWork->nInTotalSize += GetFileSizeInt64(ifp[i]);
            }
        }
    }

    // open output file
    ofp = _tfopen(pszOutPath, _T("wb"));
    if(!ofp) 
    {
        // stop file timer
		pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
		pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(_T("Elapsed time: 00:00:00"));
		pWork->pWorkDlg->m_ElapsedTimeFile = 0L;

        for(int i = 0; i < nInputFiles; i++)
        {
            if(ifp[i]) 
                fclose(ifp[i]);
        }

		if(logCtx.bInit)
		{ 
			::LogMessage(&logCtx, szLogMessage + _T("Failed to create output file: ") + pszOutPath);
		}

        pWork->pWorkDlg->bTerminate = true;
        ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

        return(WORKDLG_RETURN_FAILURE);
    }

    // begin clean-up code used after error
    //
#define HandleEncoderError(message) \
    pWork->pWorkDlg->KillTimer(WM_FILE_TIMER); \
    pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(_T("Elapsed time: 00:00:00")); \
    pWork->pWorkDlg->m_ElapsedTimeFile = 0L; \
    \
    if(fwav) \
        free(fwav); \
    \
    if(frame) \
        free(frame); \
    \
    if(bAvisynthInput == false) \
    { \
        pcm_close(&pf); \
    \
        for(int i = 0; i < nInputFiles; i++) \
        { \
            if(ifp[i]) \
                fclose(ifp[i]); \
        } \
    } \
    else \
    { \
        /* decoderAVS.CloseAvisynth(); */ \
    } \
    \
    if(ofp) \
        fclose(ofp); \
    \
    api.LibAften_aften_encode_close(&s); \
    \
    for(int i = 0; i < nInputFiles; i++) \
        szInPath[i].ReleaseBuffer(); \
    \
    szOutPath.ReleaseBuffer(); \
    \
    if(logCtx.bInit) \
    { \
        ::LogMessage(&logCtx, szLogMessage + message); \
    } \
    pWork->pWorkDlg->bTerminate = true; \
    ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0); \
    \
    return(WORKDLG_RETURN_FAILURE);
    //
    // end clean-up code used after error

#ifdef CONFIG_DOUBLE
    read_format = PCM_SAMPLE_FMT_DBL;
#else
    read_format = PCM_SAMPLE_FMT_FLT;
#endif

    input_file_format = PCM_FORMAT_UNKNOWN;

    if((opt.raw_input) || (bAvisynthInput == true))
    {
        // user selectd raw input format
        input_file_format = PCM_FORMAT_RAW;
    }
    else
    {
        // check file extension to set input file format (using only first file)
        input_file_format = GetSupportedInputFormat(GetFileExt(szInPath[0]));
    }

    if(bAvisynthInput == false)
    {
        if(pcm_init(&pf, nInputFiles, ifp, read_format, input_file_format)) 
        {
            HandleEncoderError(_T("Failed to initialize PCM library."));
        }

        if(opt.read_to_eof)
            pcm_set_read_to_eof(&pf, 1);

        if(opt.raw_input)
        {
            pcm_set_source_params(&pf, opt.raw_ch, opt.raw_fmt, opt.raw_order, opt.raw_sr);
        }
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        if(opt.raw_input)
        {
            // NOTE: raw audio settings are ignored at this time, using avisynth settings
        }
#endif
    }

    if(bAvisynthInput == true)
    {
#ifndef DISABLE_AVISYNTH
        // init avisynth read status structure
        statusAVS.nStart = 0;
        statusAVS.nSamples = infoAVS.nAudioSamples;
        statusAVS.nSamplesLeft = infoAVS.nAudioSamples;
        statusAVS.nSamplesToRead = A52_SAMPLES_PER_FRAME;

        // 'pf' is not used by avisynth, only needed for stats (to share same code with pcm lib)
        pf.samples = infoAVS.nAudioSamples;
        pf.sample_rate = infoAVS.nSamplesPerSecond;
        pf.channels = infoAVS.nAudioChannels;
        pf.ch_mask = 0xFFFFFFFF; // NOTE: plain WAVE channel selection is assumed

        // TODO: need to set this for proper encoding and check if user has selected other settings
        // NOTE: currently using ch_mask to set this
        //s.acmod = ;
        //s.lfe = ;
#endif
    }

    // TODO: need to test this with Avisynth input
    if(s.acmod >= 0) 
    {
        static const int acmod_to_ch[8] = { 2, 1, 2, 3, 3, 4, 4, 5 };
        int ch = acmod_to_ch[s.acmod];
        if(ch == pf.channels) 
        {
            if(s.lfe < 0) 
            {
                s.lfe = 0;
            } 
            else 
            {
                if(s.lfe != 0) 
                {
                    HandleEncoderError(_T("Invalid channel configuration."));
                }
            }
        } 
        else if(ch == (pf.channels - 1)) 
        {
            if(s.lfe < 0) 
            {
                s.lfe = 1;
            } 
            else 
            {
                if(s.lfe != 1) 
                {
                    HandleEncoderError(_T("Invalid channel configuration."));
                }
            }
        } 
        else 
        {
            HandleEncoderError(_T("Invalid channel configuration."));
        }
    } 
    else 
    {
        int ch = pf.channels;
        if(s.lfe >= 0) 
        {
            if(s.lfe == 0 && ch == 6) 
            {
                HandleEncoderError(_T("Invalid channel configuration."));
            } 
            else if(s.lfe == 1 && ch == 1) 
            {
                HandleEncoderError(_T("Invalid channel configuration."));
            }

            if(s.lfe) 
            {
                pf.ch_mask |= 0x08;
            }
        }

        if(api.LibAften_aften_wav_channels_to_acmod(ch, pf.ch_mask, &s.acmod, &s.lfe)) 
        {
            HandleEncoderError(_T("Invalid channel configuration."));
        }
    }

    if(bAvisynthInput == false)
    {
#ifdef CONFIG_DOUBLE
        s.sample_format = A52_SAMPLE_FMT_DBL;
#else
        s.sample_format = A52_SAMPLE_FMT_FLT;
#endif

        s.channels = pf.channels;
        s.samplerate = pf.sample_rate;
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        // Avs2Raw converts all formats to FLOAT by default
        /*
        switch(infoAVS.nSampleType)
        {
        case SAMPLE_INT8: s.sample_format = A52_SAMPLE_FMT_S8; break;
        case SAMPLE_INT16: s.sample_format = A52_SAMPLE_FMT_S16; break;
        case SAMPLE_INT24: s.sample_format = A52_SAMPLE_FMT_S24; break;
        case SAMPLE_INT32: s.sample_format = A52_SAMPLE_FMT_S32; break;
        case SAMPLE_FLOAT: s.sample_format = A52_SAMPLE_FMT_FLT; break;
        default: s.sample_format = A52_SAMPLE_FMT_FLT; break;
        }
        */
        s.sample_format = A52_SAMPLE_FMT_FLT;

        s.channels = infoAVS.nAudioChannels;
        s.samplerate = infoAVS.nSamplesPerSecond; 
#endif
    }

    // allocate memory for audio data
    frame = (uint8_t *) calloc(A52_MAX_CODED_FRAME_SIZE, 1);
    fwav = (FLOAT *) calloc(A52_SAMPLES_PER_FRAME * s.channels, sizeof(FLOAT));
    if(frame == NULL || fwav == NULL) 
    {
        HandleEncoderError(_T("Failed to allocate memory."));
    }

    samplecount = bytecount = t0 = t1 = percent = 0;
    qual = bw = 0.0;
    last_frame = 0;
    frame_cnt = 0;
    done = 0;
    fs = 0;
    nr = 0;

    if(opt.chmap == 0)
        aften_remap = api.LibAften_aften_remap_wav_to_a52;
    else if(opt.chmap == 2)
        aften_remap = api.LibAften_aften_remap_mpeg_to_a52;

    if(!opt.pad_start) 
    {
        int diff;

        if(bAvisynthInput == false)
        {
            cIORead.Start();
            nr = pcm_read_samples(&pf, fwav, 256);
            cIORead.Stop();
        }
        else
        {
#ifndef DISABLE_AVISYNTH
            statusAVS.nSamplesToRead = 256;

            cIORead.Start();
            nr = decoderAVS.GetAudio(fwav, &statusAVS);
            cIORead.Stop();
#endif
        }

        diff = 256 - nr;
        if(diff > 0) 
        {
            memmove(fwav + diff * s.channels, fwav, nr);
            memset(fwav, 0, diff * s.channels * sizeof(FLOAT));
        }

        if(aften_remap)
            aften_remap(fwav + diff, nr, s.channels, s.sample_format, s.acmod);

        s.initial_samples = fwav;
    }

    // init aften encoder
    if(api.LibAften_aften_encode_init(&s)) 
    {
        HandleEncoderError(_T("Failed to initialize encoder."));
    }

    // show current job information in work dialog
#ifndef DISABLE_AVISYNTH
    ShowCurrentJobInfo(nInputFiles, pf, pWork, s, bAvisynthInput, infoAVS);
#else
    ShowCurrentJobInfo(nInputFiles, pf, pWork, s, bAvisynthInput);
#endif

	int nCurTotalPos = 0; // 0% - 100%
	__int64 nCurPos = 0;
	bool bUpdateSpeed = false;

	__int64 nInPrevCurPos = 0;
	__int64 nOutPrevCurPos = 0;
    double fPrevTimeEncoding = 0.0;
    double fPrevTimeIORead = 0.0;
    double fPrevTimeIOWrite = 0.0;


    // main encoding loop
    do
    {
        // check if we are can continue job
        if(pWork->pWorkDlg->bTerminate == true)
        {
            // flush remaining data from encoder
            while(fs > 0)
            {
                // encode frame
                cEncoding.Start();
                fs = api.LibAften_aften_encode_frame(&s, frame, NULL, 0);
                cEncoding.Stop();

                if(fs > 0) 
                    fwrite(frame, 1, fs, ofp);
            }

            HandleEncoderError(_T("User has terminated encoding."));
        }

        // read input data
        if(bAvisynthInput == false)
        {
            cIORead.Start();
            nr = pcm_read_samples(&pf, fwav, A52_SAMPLES_PER_FRAME);
            cIORead.Stop();
        }
        else
        {
#ifndef DISABLE_AVISYNTH
            statusAVS.nSamplesToRead = A52_SAMPLES_PER_FRAME;

            cIORead.Start();
            nr = decoderAVS.GetAudio(fwav, &statusAVS);
            cIORead.Stop();
#endif
        }

        if(aften_remap)
            aften_remap(fwav, nr, s.channels, s.sample_format, s.acmod);

        // encode frame
        cEncoding.Start();
        fs = api.LibAften_aften_encode_frame(&s, frame, fwav, nr);
        cEncoding.Stop();

        if(fs < 0) 
        {
            // error encoding frame
            // break;
            HandleEncoderError(_T("Failed to encode frame."));
        } 
        else 
        {
            if(fs > 0) 
            {
                samplecount += A52_SAMPLES_PER_FRAME;
                bytecount += fs;
                qual += s.status.quality;
                bw += s.status.bwcode;
            }

            t1 = samplecount / pf.sample_rate;
            if(frame_cnt > 0 && (t1 > t0 || samplecount >= pf.samples))
			{
                kbps = (bytecount * FCONST(8.0) * pf.sample_rate) / (FCONST(1000.0) * samplecount);
                percent = 0;
                if(pf.samples > 0) 
                {
                    percent = (uint32_t)((samplecount * FCONST(100.0)) / pf.samples);
                    percent = CLIP(percent, 0, 100);
                }

                // int nCurTotalPos = 0; // 0% - 100%
                // __int64 nCurPos = 0;

                if(bAvisynthInput == false)
                {
                    if(pWork->bMultiMonoInput == false)
                    {
                        nCurPos = _ftelli64(ifp[0]);
                    }
                    else
                    {
                        for(int i = 0; i < nInputFiles; i++)
                        {
                            nCurPos += _ftelli64(ifp[i]);
                        }
                    }
                }
                else
                {
#ifndef DISABLE_AVISYNTH
                    // TODO: use 'nr' to count read samples instead of 'samplecount'
                    nCurPos = samplecount * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
#endif
                }

                if(pWork->pWorkDlg->bCanUpdateWindow == true)
                {
					pWork->pWorkDlg->bCanUpdateWindow = false;
					CString szTmpBuff;

					// update current encoder speed
					szTmpBuff.Format(_T("%0.1lf"), ((double) (nCurPos) / 1048576.0f) / (cEncoding.Time() + 1.0e-16));
					pWork->pWorkDlg->szSpeedEncoderAvg = szTmpBuff;

					// update current read speed
					szTmpBuff.Format(_T("%0.1lf"), ((double) (nCurPos) / 1048576.0f) / (cIORead.Time() + 1.0e-16));
					pWork->pWorkDlg->szSpeedReadsAvg = szTmpBuff;

					nInPrevCurPos = nCurPos;
					fPrevTimeEncoding = cEncoding.Time();
					fPrevTimeIORead = cIORead.Time();

					pWork->pWorkDlg->bCanUpdateWindow = true;
				}

                // use ftell to get encoding progress
                percent = (100 * nCurPos) / pWork->nInTotalSize;

                // update progress bars              
                if(pWork->pWorkDlg->bCanUpdateWindow == true)
                {
                    pWork->pWorkDlg->bCanUpdateWindow = false;
                    pWork->pWorkDlg->m_PrgCurrent.SetPos(percent);
                    // TRACE(_T("PRG_CUR=%3d%%\n"), percent);
                    pWork->pWorkDlg->bCanUpdateWindow = true;
                }

                if(nTotalSizeCounter != NULL)
                {
                    nCurTotalPos = (100 * (*nTotalSizeCounter + nCurPos)) / pWork->pWorkDlg->nTotalSize;

                    if(pWork->pWorkDlg->bCanUpdateWindow == true)
                    {
                        pWork->pWorkDlg->bCanUpdateWindow = false;
                        pWork->pWorkDlg->m_PrgTotal.SetPos(nCurTotalPos);
                        // TRACE(_T("PRG_TOT=%3d%%\n"), nCurTotalPos);
                        pWork->pWorkDlg->bCanUpdateWindow = true;
                    }
                }
            }
            t0 = t1;

            // write encoded data
            cIOWrite.Start();
            fwrite(frame, 1, fs, ofp);
            cIOWrite.Stop();

			if(pWork->pWorkDlg->bCanUpdateWindow == true)
			{
				// update current write speed
				pWork->pWorkDlg->bCanUpdateWindow = false;

				CString szTmpBuff;

				szTmpBuff.Format(_T("%0.1lf"), ((double) (_ftelli64(ofp)) / 1048576.0f) / (cIOWrite.Time() + 1.0e-16));
				pWork->pWorkDlg->szSpeedWritesAvg = szTmpBuff;

				nOutPrevCurPos = _ftelli64(ofp);
    			fPrevTimeIOWrite = cIOWrite.Time();

				pWork->pWorkDlg->bCanUpdateWindow = true;
			}

            // update frame counter
            frame_cnt++;
            last_frame = nr;
        }
    }
    while(nr > 0 || fs > 0 || !frame_cnt);

    // gather performance statistics
    pWork->fTimeEncoding = cEncoding.Time();
    pWork->fTimeIORead = cIORead.Time();
    pWork->fTimeIOWrite = cIOWrite.Time();

	// get output file size
	pWork->nOutTotalSize = GetFileSizeInt64(ofp);

    // log preformance stats
    if(logCtx.bInit)
    {
		// log input file(s) path(s)
        CString szTmpBuff;
        for(int i = 0; i < nInputFiles; i++)
        {
            szTmpBuff.Format(_T("Input[%d]=%s"), i, szInPath[i]);
            ::LogMessage(&logCtx, szTmpBuff);
        }

		// log output file path
        szTmpBuff.Format(_T("Output=%s"), szOutPath);
        ::LogMessage(&logCtx, szTmpBuff);

		// calculate total time
        pWork->fTimeTotal = pWork->fTimeEncoding + pWork->fTimeIORead + pWork->fTimeIOWrite;

		// log encoder speed
        szTmpBuff.Format(_T("Tenc=%0.3lfs (%.0lf%%), %I64d Bytes, %0.3lf MBytes/s"), 
			pWork->fTimeEncoding, 
			((pWork->fTimeEncoding + 1.0e-16) * 100.0f) /  (pWork->fTimeTotal + 1.0e-16),
			pWork->nInTotalSize,
			((double) (pWork->nInTotalSize) / 1048576.0f) / (pWork->fTimeEncoding + 1.0e-16));
        ::LogMessage(&logCtx, szTmpBuff);

		// log read speed
        szTmpBuff.Format(_T("Tread=%0.3lfs (%.0lf%%), %I64d Bytes, %0.3lf MBytes/s"), 
			pWork->fTimeIORead, 
			((pWork->fTimeIORead + 1.0e-16) * 100.0f) /  (pWork->fTimeTotal + 1.0e-16),
			pWork->nInTotalSize,
			((double) (pWork->nInTotalSize) / 1048576.0f) / (pWork->fTimeIORead + 1.0e-16));
        ::LogMessage(&logCtx, szTmpBuff);

		// log write speed
        szTmpBuff.Format(_T("Twrite=%0.3lfs (%.0lf%%), %I64d Bytes, %0.3lf MBytes/s"), 
			pWork->fTimeIOWrite, 
			((pWork->fTimeIOWrite + 1.0e-16) * 100.0f) /  (pWork->fTimeTotal + 1.0e-16),
			pWork->nOutTotalSize,
			((double) (pWork->nOutTotalSize) / 1048576.0f) / (pWork->fTimeIOWrite + 1.0e-16));
        ::LogMessage(&logCtx, szTmpBuff);

		// log total time
        szTmpBuff.Format(_T("Ttotal=%0.3lfs"), pWork->fTimeTotal);
        ::LogMessage(&logCtx, szTmpBuff);
    }

    // clean-up used memory
    if(fwav)
        free(fwav);

    if(frame)
        free(frame);

    if(bAvisynthInput == false)
    {
	    // close PCM file context
        pcm_close(&pf);
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        decoderAVS.CloseAvisynth();
#endif
    }

    // update total counter
    if(bAvisynthInput == false)
    {
        if(pWork->bMultiMonoInput == false)
        {
            *nTotalSizeCounter += _ftelli64(ifp[0]);
        }
        else
        {
            for(int i = 0; i < nInputFiles; i++)
            {
                *nTotalSizeCounter += _ftelli64(ifp[i]);
            }
        }
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        *nTotalSizeCounter += pWork->nInTotalSize;
#endif
    }

    // close input files
    for(int i = 0; i < nInputFiles; i++)
    {
        if(ifp[i]) 
            fclose(ifp[i]);
    }

	// close output file
    if(ofp)
        fclose(ofp);

	// close encoder context
    api.LibAften_aften_encode_close(&s);

    // reset file timer
    pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
    pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(_T("Elapsed time: 00:00:00"));
    pWork->pWorkDlg->m_ElapsedTimeFile = 0L;

    // release string buffers
    for(int i = 0; i < nInputFiles; i++)
        szInPath[i].ReleaseBuffer();

    szOutPath.ReleaseBuffer();

    // return succes from worker thread
    return(WORKDLG_RETURN_SUCCESS);
}

DWORD WINAPI EncWorkThread(LPVOID pParam)
{
    // get worker configuration data
    WorkerParam *pWork = (WorkerParam *) pParam;
    __int64 nTotalSizeCounter = 0;
    AftenAPI api = pWork->api;

    // command-line string for Aften
    CString szCommandLine = _T("");
    CString szBuff = _T("");
#ifdef _UNICODE
    const unsigned int nAnsiBuffSize = 8192;
    char szAnsiBuff[nAnsiBuffSize] = "";
    int nChars = 0;
#endif // _UNICODE

    // check if we have valid worker dialog object
    if(pWork->pWorkDlg == NULL)
    {
        // fatal error, invalid object
        return(WORKDLG_RETURN_FAILURE);
    }

    CList<CString,CString> *list = pWork->list;
    CList<bool,bool> *listStatus = pWork->listStatus;
    POSITION pos;
    POSITION posStatus;

    // update progress bars
    pWork->pWorkDlg->m_PrgCurrent.SetRange(0, 100);
    pWork->pWorkDlg->m_PrgTotal.SetRange32(0, 100);

    pWork->pWorkDlg->m_PrgCurrent.SetPos(0);
    pWork->pWorkDlg->m_PrgTotal.SetPos(0);

    // start total timer
    pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
    pWork->pWorkDlg->m_ElapsedTimeTotal = 0L;
    pWork->pWorkDlg->m_StcTimeTotal.SetWindowText(_T("Total elapsed time: 00:00:00"));
    pWork->pWorkDlg->SetTimer(WM_TOTAL_TIMER, 250, NULL);

    int nFileCounter = 0;
    int nTotalFiles = list->GetSize();

    // get first status item from the list
    posStatus = listStatus->GetHeadPosition();

    if(pWork->bMultiMonoInput == false)
    {
        // process all filse in list
        pos = list->GetHeadPosition();
        while(pos != NULL)
        {
            CString szInPath[6] = { _T("-"), _T("-"), _T("-"), _T("-"), _T("-"), _T("-") };
            CString szOutPath = _T("");

            // get next file path
            szInPath[0] = list->GetNext(pos);

            // prepare output file name
            szOutPath = szInPath[0];
            szOutPath.Truncate(szOutPath.GetLength() - GetFileExt(szOutPath).GetLength());
            szOutPath.Append(szSupportedOutputExt[0]);

            // use different output path
            if(pWork->bUseOutPath == true)
            {
                CString szFile = GetFileName(szOutPath);

                if((pWork->szOutPath[pWork->szOutPath.GetLength() - 1] == '\\') || 
                    (pWork->szOutPath[pWork->szOutPath.GetLength() - 1] == '/'))
                    szOutPath = pWork->szOutPath + szFile;
                else
                    szOutPath = pWork->szOutPath + '\\' + szFile;
            }

            // update encoding windows title
            CString szTitle;
            szTitle.Format(_T("Encoding file %d of %d"), nFileCounter + 1, nTotalFiles);
            pWork->pWorkDlg->SetWindowText(szTitle);

            // update input and output file labels
            pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDIn[0])->SetWindowText(_T("From:\t") + szInPath[0]);
            pWork->pWorkDlg->m_StcOut.SetWindowText(_T("To:\t") + szOutPath);

            // start file timer
            pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(_T("Elapsed time: 00:00:00"));
            pWork->pWorkDlg->m_ElapsedTimeFile = 0L;
            pWork->pWorkDlg->SetTimer(WM_FILE_TIMER, 250, NULL);

            // update progress bars
            pWork->pWorkDlg->m_PrgCurrent.SetPos(0);

            // variables used by Aften encoder
            AftenContext s;
            AftenOpt opt;

            // zero options
            ZeroMemory(&opt, sizeof(AftenOpt));

            // get currently selected preset
            EncoderPreset preset = GetCurrentPreset();

            // prepare aften context for encoding process
            SetAftenOptions(api, s, preset, opt, pWork);

            // encode input .wav file to output .ac3 file
            if(RunAftenEncoder(api, s, opt, pWork, szInPath, szOutPath, 1, &nTotalSizeCounter) == WORKDLG_RETURN_FAILURE)
            {
                listStatus->SetAt(posStatus, false);
                return(WORKDLG_RETURN_FAILURE);
            }
            else
            {
                listStatus->SetAt(posStatus, true);
            }

            // update status list position
            listStatus->GetNext(posStatus);

            // update progress bars
            nFileCounter++;

            // update work dialog file counter
            pWork->pWorkDlg->nCount = nFileCounter;
        }
    }
    else
    {
        CString szInPath[6] = { _T("-"), _T("-"), _T("-"), _T("-"), _T("-"), _T("-") };
        CString szOutPath = _T("");

        // process all files in list
        nFileCounter = 0;
        pos = list->GetHeadPosition();
        while(pos != NULL)
        {
            // get next file path
            szInPath[nFileCounter] = list->GetNext(pos);

            nFileCounter++;
        }

        // prepare output file name (using first file path from the list)
        szOutPath = szInPath[0];
        szOutPath.Truncate(szOutPath.GetLength() - GetFileExt(szOutPath).GetLength());
        szOutPath.Append(szSupportedOutputExt[0]);

        // use user selected output file path
        if(pWork->bUseOutPath == true)
        {
            szOutPath = pWork->szOutPath;
        }

        // update encoding windows title
        CString szTitle;
        szTitle.Format(_T("Encoding %d mono files"), nTotalFiles);
        pWork->pWorkDlg->SetWindowText(szTitle);

        // update input and output file labels
        pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDIn[0])->SetWindowText(_T("From:\t") + szInPath[0]);
        for(int i = 1; i < nFileCounter; i++)
        {
            pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDIn[i])->SetWindowText(_T("\t") + szInPath[i]);
        }

        pWork->pWorkDlg->m_StcOut.SetWindowText(_T("To:\t") + szOutPath);

        // start file timer
        pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(_T("Elapsed time: 00:00:00"));
        pWork->pWorkDlg->m_ElapsedTimeFile = 0L;
        pWork->pWorkDlg->SetTimer(WM_FILE_TIMER, 250, NULL);

        // update progress bars
        pWork->pWorkDlg->m_PrgCurrent.SetPos(0);

        // variables used by Aften encoder
        AftenContext s;
        AftenOpt opt;

        // zero options
        ZeroMemory(&opt, sizeof(AftenOpt));

        // get currently selected preset
        EncoderPreset preset = GetCurrentPreset();

        // prepare aften context for encoding process
        SetAftenOptions(api, s, preset, opt, pWork);

        // encode input .wav file to output .ac3 file
        if(RunAftenEncoder(api, s, opt, pWork, szInPath, szOutPath, nFileCounter, &nTotalSizeCounter) == WORKDLG_RETURN_FAILURE)
        {
            // update status list position
            while(posStatus != NULL)
            {
                listStatus->SetAt(posStatus, false);
                listStatus->GetNext(posStatus);
            }

            // update work dialog file counter
            pWork->pWorkDlg->nCount = 0;

            return(WORKDLG_RETURN_FAILURE);
        }
        else
        {
            // update status list position
            while(posStatus != NULL)
            {
                listStatus->SetAt(posStatus, true);
                listStatus->GetNext(posStatus);
            }

            // update work dialog file counter
            pWork->pWorkDlg->nCount = nFileCounter;
        }
    }

    // stop total timer
    pWork->pWorkDlg->KillTimer(WM_TOTAL_TIMER);

    // tell work dialog it is the end of work
    pWork->pWorkDlg->bTerminate = true;
    ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

    // return succes from worker thread
    return(WORKDLG_RETURN_SUCCESS);
}
