#include "StdAfx.h"
#include "MainApp.h"
#include "Worker.h"

void CWorker::InitContext(const CEncoderPreset *preset, const AftenAPI &api, AftenOpt &opt, AftenContext &s)
{
    api.LibAften_aften_set_defaults(&s);

    s.params.encoding_mode = preset->nMode;
    s.params.bitrate = preset->nBitrate;
    s.params.quality = preset->nQuality;
    s.system.n_threads = preset->nThreads;
    s.system.wanted_simd_instructions.mmx = preset->nUsedSIMD[0];
    s.system.wanted_simd_instructions.sse = preset->nUsedSIMD[1];
    s.system.wanted_simd_instructions.sse2 = preset->nUsedSIMD[2];
    s.system.wanted_simd_instructions.sse3 = preset->nUsedSIMD[3];

    if (preset->nRawSampleFormat != 0)
    {
        switch (preset->nRawSampleFormat)
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

    if (preset->nRawSampleRate != 0)
    {
        opt.raw_sr = preset->nRawSampleRate;
        opt.raw_input = 1;
    }

    if (preset->nRawChannels != 0)
    {
        opt.raw_ch = preset->nRawChannels;
        opt.raw_input = 1;
    }

    int nSetting;

#define SET_AFTEN_SETTING(set, type) \
    if(CEncoderDefaults::encOpt[nSetting].nIgnoreValue != preset->nSetting[nSetting]) \
        (set) = (type) CEncoderDefaults::encOpt[nSetting].listOptValues.Get(preset->nSetting[nSetting]);

    nSetting = 0; SET_AFTEN_SETTING(s.params.bitalloc_fast, int)
    nSetting++; SET_AFTEN_SETTING(s.params.expstr_search, int)
    nSetting++; SET_AFTEN_SETTING(opt.pad_start, int)
    nSetting++; SET_AFTEN_SETTING(s.params.bwcode, int)
    nSetting++; SET_AFTEN_SETTING(s.params.min_bwcode, int)
    nSetting++; SET_AFTEN_SETTING(s.params.max_bwcode, int)
    nSetting++; SET_AFTEN_SETTING(s.params.use_rematrixing, int)
    nSetting++; SET_AFTEN_SETTING(s.params.use_block_switching, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.cmixlev, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.surmixlev, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.dsurmod, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.dialnorm, int)
    nSetting++; SET_AFTEN_SETTING(s.params.dynrng_profile, DynRngProfile)
    nSetting++; SET_AFTEN_SETTING(s.acmod, int)
    nSetting++; SET_AFTEN_SETTING(s.lfe, int)
    nSetting++;
    if (CEncoderDefaults::encOpt[nSetting].nIgnoreValue != preset->nSetting[nSetting])
    {
        s.acmod = CEncoderDefaults::ccAften[CEncoderDefaults::encOpt[nSetting].listOptValues.Get(preset->nSetting[nSetting])].acmod;
        s.lfe = CEncoderDefaults::ccAften[CEncoderDefaults::encOpt[nSetting].listOptValues.Get(preset->nSetting[nSetting])].lfe;
    }
    nSetting++; SET_AFTEN_SETTING(opt.chmap, int)
    nSetting++; SET_AFTEN_SETTING(opt.read_to_eof, int)
    nSetting++; SET_AFTEN_SETTING(s.params.use_bw_filter, int)
    nSetting++; SET_AFTEN_SETTING(s.params.use_dc_filter, int)
    nSetting++; SET_AFTEN_SETTING(s.params.use_lfe_filter, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.xbsi1e, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.dmixmod, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.ltrtcmixlev, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.ltrtsmixlev, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.lorocmixlev, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.lorosmixlev, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.xbsi2e, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.dsurexmod, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.dheadphonmod, int)
    nSetting++; SET_AFTEN_SETTING(s.meta.adconvtyp, int)
}

void CWorker::UpdateProgress()
{
    CString szInputInfo = _T("");
    CString szOutputInfo = _T("");
    CString szSimdInfo = _T("");

    if (bAvisynthInput == false)
    {
        for (int i = 0; i < nInputFiles; i++)
        {
            PcmFile *pf_info = &pf.pcm_file[i];
            TCHAR *type, *chan, *order;
            TCHAR fmt[64] = _T("");

            type = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02001) : _T("?");
            chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02002) : _T("?-channel");
            order = _T("");

            if (pf_info->sample_type == PCM_SAMPLE_TYPE_INT)
            {
                if (pf_info->source_format == PCM_SAMPLE_FMT_U8)
                    type = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02003) : _T("Unsigned");
                else
                    type = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02004) : _T("Signed");
            }
            else if (pf_info->sample_type == PCM_SAMPLE_TYPE_FLOAT)
            {
                type = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02005) : _T("Floating-point");
            }
            else
            {
                type = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02006) : _T("[unsupported type]");
            }

            if (pf_info->ch_mask & 0x08)
            {
                switch (pf_info->channels - 1)
                {
                case 1: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02007) : _T("1.1-channel"); break;
                case 2: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02008) : _T("2.1-channel"); break;
                case 3: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02009) : _T("3.1-channel"); break;
                case 4: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200A) : _T("4.1-channel"); break;
                case 5: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200B) : _T("5.1-channel"); break;
                default: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200C) : _T("multi-channel with LFE"); break;
                }
            }
            else
            {
                switch (pf_info->channels)
                {
                case 1: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200D) : _T("mono"); break;
                case 2: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200E) : _T("stereo"); break;
                case 3: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200F) : _T("3-channel"); break;
                case 4: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02010) : _T("4-channel"); break;
                case 5: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02011) : _T("5-channel"); break;
                case 6: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02012) : _T("6-channel"); break;
                default: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02013) : _T("multi-channel"); break;
                }
            }

            if (pf_info->pcm_format)
            {
#ifdef _UNICODE
                ConvertAnsiToUnicode(pf_info->pcm_format->long_name,
                    fmt,
                    strlen(pf_info->pcm_format->long_name));
#else
                sprintf(fmt, _T("%s"), pf_info->pcm_format->long_name);
#endif
            }
            else
            {
                _stprintf(fmt, _T("%s"),
                    theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02014) : _T("unknown"));
            }

            if (pf_info->source_format > PCM_SAMPLE_FMT_S8)
            {
                switch (pf_info->order)
                {
                case PCM_BYTE_ORDER_LE: order = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02015) : _T("little-endian"); break;
                case PCM_BYTE_ORDER_BE: order = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02016) : _T("big-endian"); break;
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
        TCHAR *chan;
        chan = _T("?-channel");

        switch (infoAVS.nAudioChannels)
        {
        case 1: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200D) : _T("mono"); break;
        case 2: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200E) : _T("stereo"); break;
        case 3: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0200F) : _T("3-channel"); break;
        case 4: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02010) : _T("4-channel"); break;
        case 5: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02011) : _T("5-channel"); break;
        case 6: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02012) : _T("6-channel"); break;
        default: chan = theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02013) : _T("multi-channel"); break;
        }

        szInputInfo.Format(_T("\t%s %d Hz %s"),
            theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A02017) : _T("Avisynth: Raw PCM Floating-point 32-bit little-endian"),
            infoAVS.nSamplesPerSecond, chan);

        pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDInInfo[0])->SetWindowText(szInputInfo);
#endif
    }

    {
        TCHAR *acmod_str[32] =
        {
            theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02018) : _T("dual mono (1+1)"),
            theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A02019) : _T("mono (1/0)"),
            theApp.m_Config.HaveLangStrings() ? (LPTSTR)(LPCTSTR)theApp.m_Config.GetLangString(0x00A0201A) : _T("stereo (2/0)"),
            _T("3/0"),
            _T("2/1"),
            _T("3/1"),
            _T("2/2"),
            _T("3/2")
        };

        szOutputInfo.Format(_T("\tAC3 %d Hz %s"), s.samplerate, acmod_str[s.acmod]);
        if (s.lfe)
            szOutputInfo += _T(" + LFE");
    }

    {
        int nCountSimd = 0;
        szSimdInfo = _T("SIMD:");

        if (s.system.wanted_simd_instructions.mmx && s.system.available_simd_instructions.mmx)
        {
            szSimdInfo += _T(" MMX"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.sse && s.system.available_simd_instructions.sse)
        {
            szSimdInfo += _T(" SSE"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.sse2 && s.system.available_simd_instructions.sse2)
        {
            szSimdInfo += _T(" SSE2"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.sse3 && s.system.available_simd_instructions.sse3)
        {
            szSimdInfo += _T(" SSE3"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.ssse3 && s.system.available_simd_instructions.ssse3)
        {
            szSimdInfo += _T(" SSSE3"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.amd_3dnow && s.system.available_simd_instructions.amd_3dnow)
        {
            szSimdInfo += _T(" 3DNOW"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.amd_3dnowext && s.system.available_simd_instructions.amd_3dnowext)
        {
            szSimdInfo += _T(" 3DNOWEXT"); nCountSimd++;
        }
        if (s.system.wanted_simd_instructions.amd_sse_mmx && s.system.available_simd_instructions.amd_sse_mmx)
        {
            szSimdInfo += _T(" SSE-MMX"); nCountSimd++;
        }

        if (nCountSimd == 0)
            szSimdInfo += _T(" ") + (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A0201B) : _T("NONE"));

        CString szBuff;
        if (s.system.n_threads == 0)
        {
            szBuff = _T(" | ") + (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A0201C) : _T("Threads: Auto"));
        }
        else
        {
            szBuff.Format(_T(" | %s %i"),
                theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A0201D) : _T("Threads:"),
                s.system.n_threads);
        }

        szSimdInfo += szBuff;
    }

    pWork->pWorkDlg->m_StcOutInfo.SetWindowText(szOutputInfo);
    pWork->pWorkDlg->m_StcSimdInfo.SetWindowText(szSimdInfo);
}

BOOL CWorker::HandleError(LPTSTR pszMessage)
{
    pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
	CString szBuff;
	szBuff.Format(_T("%s %s"),
		theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01005) : _T("Elapsed time:"),
		_T("00:00:00"));

	pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(szBuff);
    pWork->m_ElapsedTimeFile = 0L;

    if(fwav)
        free(fwav);

    if(frame)
        free(frame);

    if(bAvisynthInput == false)
    {
        pcm_close(&pf);
        for(int i = 0; i < nInputFiles; i++)
        {
            if(ifp[i])
                fclose(ifp[i]);
        }
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        decoderAVS.CloseAvisynth();
#endif
    }

    if(ofp)
        fclose(ofp);

    pWork->api.LibAften_aften_encode_close(&s);

    for(int i = 0; i < nInputFiles; i++)
        szInPath[i].ReleaseBuffer();

    szOutPath.ReleaseBuffer();

    pWork->bTerminate = true;
    ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

    return(FALSE);
}

int CWorker::Run()
{
    void(*aften_remap)(void *samples, int n, int ch, A52SampleFormat fmt, int acmod) = nullptr;
    int nr, fs;
    uint32_t samplecount, bytecount, t0, t1, percent;
    FLOAT kbps, qual, bw;
    int last_frame;
    int frame_cnt;
    int done;
    int input_file_format;
    enum PcmSampleFormat read_format;
    CTimeCount cEncoding, cIORead, cIOWrite;
    TCHAR *pszInPath[6] = { { nullptr } };
    TCHAR *pszOutPath = nullptr;

    frame = nullptr;
    fwav = nullptr;

    for (int i = 0; i < nInputFiles; i++)
        ifp[i] = nullptr;

    ofp = nullptr;

    bAvisynthInput = false;

#ifndef DISABLE_AVISYNTH
    if (GetFileExtension(szInPath[0]).MakeLower() == _T("avs"))
        bAvisynthInput = true;
#endif

    pWork->nInTotalSize = 0;

    for (int i = 0; i < nInputFiles; i++)
        pszInPath[i] = szInPath[i].GetBuffer();

    pszOutPath = szOutPath.GetBuffer();
    memset(ifp, 0, CEncoderDefaults::nNumMaxInputFiles * sizeof(FILE *));

#ifndef DISABLE_AVISYNTH
    char szInputFileAVS[MAX_PATH] = "";
#endif
    if (bAvisynthInput == true)
    {
#ifndef DISABLE_AVISYNTH
#ifdef _UNICODE
        ConvertUnicodeToAnsi(pszInPath[0], szInputFileAVS, lstrlen(pszInPath[0]));
        if (decoderAVS.OpenAvisynth(szInputFileAVS) == false)
#else
        if (decoderAVS.OpenAvisynth(pszInPath[0]) == false)
#endif
        {
            // _T("Failed to initialize Avisynth.")
            return(FALSE);
        }
        else
        {
            infoAVS = decoderAVS.GetInputInfo();
            pWork->nInTotalSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
            // _T("Avisynth initialized successfully.")
        }
#endif
    }
    else
    {
        for (int i = 0; i < nInputFiles; i++)
        {
            ifp[i] = _tfopen(pszInPath[i], _T("rb"));
            if (!ifp[i])
            {
                pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);

                CString szBuff;
                szBuff.Format(_T("%s %s"),
                    theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01005) : _T("Elapsed time:"),
                    _T("00:00:00"));

                pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(szBuff);
                pWork->m_ElapsedTimeFile = 0L;

                // _T("Failed to open input file:") + pszInPath[i]

                pWork->bTerminate = true;
                ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

                return(FALSE);
            }
            else
            {
                pWork->nInTotalSize += GetFileSizeInt64(ifp[i]);
            }
        }
    }

    ofp = _tfopen(pszOutPath, _T("wb"));
    if (!ofp)
    {
        CString szBuff;
        szBuff.Format(_T("%s %s"),
            theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01005) : _T("Elapsed time:"),
            _T("00:00:00"));

        pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
        pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(szBuff);
        pWork->m_ElapsedTimeFile = 0L;

        for (int i = 0; i < nInputFiles; i++)
        {
            if (ifp[i])
                fclose(ifp[i]);
        }

        // _T("Failed to create output file: ") + pszOutPath

        pWork->bTerminate = true;
        ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

        return(FALSE);
    }

#ifdef CONFIG_DOUBLE
    read_format = PCM_SAMPLE_FMT_DBL;
#else
    read_format = PCM_SAMPLE_FMT_FLT;
#endif

    input_file_format = PCM_FORMAT_UNKNOWN;

    if ((opt.raw_input) || (bAvisynthInput == true))
    {
        input_file_format = PCM_FORMAT_RAW;
    }
    else
    {
        input_file_format = CEncoderDefaults::GetSupportedInputFormat(GetFileExtension(szInPath[0]));
    }

    if (bAvisynthInput == false)
    {
        if (pcm_init(&pf, nInputFiles, ifp, read_format, input_file_format))
        {
            return HandleError(_T("Failed to initialize PCM library."));
        }

        if (opt.read_to_eof)
            pcm_set_read_to_eof(&pf, 1);

        if (opt.raw_input)
        {
            pcm_set_source_params(&pf, opt.raw_ch, opt.raw_fmt, opt.raw_order, opt.raw_sr);
        }
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        if (opt.raw_input)
        {
        }
#endif
    }

#ifndef DISABLE_AVISYNTH
    if (bAvisynthInput == true)
    {
        statusAVS.nStart = 0;
        statusAVS.nSamples = infoAVS.nAudioSamples;
        statusAVS.nSamplesLeft = infoAVS.nAudioSamples;
        statusAVS.nSamplesToRead = A52_SAMPLES_PER_FRAME;
        pf.samples = infoAVS.nAudioSamples;
        pf.sample_rate = infoAVS.nSamplesPerSecond;
        pf.channels = infoAVS.nAudioChannels;
        pf.ch_mask = 0xFFFFFFFF;

    }
#endif

    if (s.acmod >= 0)
    {
        static const int acmod_to_ch[8] = { 2, 1, 2, 3, 3, 4, 4, 5 };
        int ch = acmod_to_ch[s.acmod];
        if (ch == pf.channels)
        {
            if (s.lfe < 0)
            {
                s.lfe = 0;
            }
            else
            {
                if (s.lfe != 0)
                {
                    return HandleError(_T("Invalid channel configuration."));
                }
            }
        }
        else if (ch == (pf.channels - 1))
        {
            if (s.lfe < 0)
            {
                s.lfe = 1;
            }
            else
            {
                if (s.lfe != 1)
                {
                    return HandleError(_T("Invalid channel configuration."));
                }
            }
        }
        else
        {
            return HandleError(_T("Invalid channel configuration."));
        }
    }
    else
    {
        int ch = pf.channels;
        if (s.lfe >= 0)
        {
            if (s.lfe == 0 && ch == 6)
            {
                return HandleError(_T("Invalid channel configuration."));
            }
            else if (s.lfe == 1 && ch == 1)
            {
                return HandleError(_T("Invalid channel configuration."));
            }

            if (s.lfe)
            {
                pf.ch_mask |= 0x08;
            }
        }

        if (pWork->api.LibAften_aften_wav_channels_to_acmod(ch, pf.ch_mask, &s.acmod, &s.lfe))
        {
            return HandleError(_T("Invalid channel configuration."));
        }
    }

    if (bAvisynthInput == false)
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
        s.sample_format = A52_SAMPLE_FMT_FLT;
        s.channels = infoAVS.nAudioChannels;
        s.samplerate = infoAVS.nSamplesPerSecond;
#endif
    }
    frame = (uint8_t *)calloc(A52_MAX_CODED_FRAME_SIZE, 1);
    fwav = (FLOAT *)calloc(A52_SAMPLES_PER_FRAME * s.channels, sizeof(FLOAT));
    if (frame == nullptr || fwav == nullptr)
    {
        return HandleError(_T("Failed to allocate memory."));
    }

    samplecount = bytecount = t0 = t1 = percent = 0;
    qual = bw = 0.0;
    last_frame = 0;
    frame_cnt = 0;
    done = 0;
    fs = 0;
    nr = 0;

    if (opt.chmap == 0)
        aften_remap = pWork->api.LibAften_aften_remap_wav_to_a52;
    else if (opt.chmap == 2)
        aften_remap = pWork->api.LibAften_aften_remap_mpeg_to_a52;

    if (!opt.pad_start)
    {
        int diff;

        if (bAvisynthInput == false)
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
        if (diff > 0)
        {
            memmove(fwav + diff * s.channels, fwav, nr);
            memset(fwav, 0, diff * s.channels * sizeof(FLOAT));
        }

        if (aften_remap)
            aften_remap(fwav + diff, nr, s.channels, s.sample_format, s.acmod);

        s.initial_samples = fwav;
    }

    if (pWork->api.LibAften_aften_encode_init(&s))
    {
        return HandleError(_T("Failed to initialize encoder."));
    }

    UpdateProgress();

    int nCurTotalPos = 0;
    __int64 nCurPos = 0;
    bool bUpdateSpeed = false;
    __int64 nInPrevCurPos = 0;
    __int64 nOutPrevCurPos = 0;
    double fPrevTimeEncoding = 0.0;
    double fPrevTimeIORead = 0.0;
    double fPrevTimeIOWrite = 0.0;

    do
    {
        if (pWork->bTerminate == true)
        {
            while (fs > 0)
            {
                cEncoding.Start();
                fs = pWork->api.LibAften_aften_encode_frame(&s, frame, nullptr, 0);
                cEncoding.Stop();

                if (fs > 0)
                    fwrite(frame, 1, fs, ofp);
            }

            return HandleError(_T("User has terminated encoding."));
        }

        if (bAvisynthInput == false)
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

        if (aften_remap)
            aften_remap(fwav, nr, s.channels, s.sample_format, s.acmod);

        cEncoding.Start();
        fs = pWork->api.LibAften_aften_encode_frame(&s, frame, fwav, nr);
        cEncoding.Stop();

        if (fs < 0)
        {
            return HandleError(_T("Failed to encode frame."));
        }
        else
        {
            if (fs > 0)
            {
                samplecount += A52_SAMPLES_PER_FRAME;
                bytecount += fs;
                qual += s.status.quality;
                bw += s.status.bwcode;
            }

            t1 = samplecount / pf.sample_rate;
            if (frame_cnt > 0 && (t1 > t0 || samplecount >= pf.samples))
            {
                kbps = (bytecount * FCONST(8.0) * pf.sample_rate) / (FCONST(1000.0) * samplecount);
                percent = 0;
                if (pf.samples > 0)
                {
                    percent = (uint32_t)((samplecount * FCONST(100.0)) / pf.samples);
                    percent = CLIP(percent, 0, 100);
                }

                if (bAvisynthInput == false)
                {
                    if (pWork->bMultiMonoInput == false)
                    {
                        nCurPos = _ftelli64(ifp[0]);
                    }
                    else
                    {
                        for (int i = 0; i < nInputFiles; i++)
                        {
                            nCurPos += _ftelli64(ifp[i]);
                        }
                    }
                }
                else
                {
#ifndef DISABLE_AVISYNTH
                    nCurPos = samplecount * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
#endif
                }

                if (pWork->bCanUpdateWindow == true)
                {
                    pWork->bCanUpdateWindow = false;
                    CString szTmpBuff;

                    szTmpBuff.Format(_T("%0.1lf"), ((double)(nCurPos) / 1048576.0f) / (cEncoding.ElapsedTime() + 1.0e-16));
                    pWork->szSpeedEncoderAvg = szTmpBuff;

                    szTmpBuff.Format(_T("%0.1lf"), ((double)(nCurPos) / 1048576.0f) / (cIORead.ElapsedTime() + 1.0e-16));
                    pWork->szSpeedReadsAvg = szTmpBuff;

                    nInPrevCurPos = nCurPos;
                    fPrevTimeEncoding = cEncoding.ElapsedTime();
                    fPrevTimeIORead = cIORead.ElapsedTime();

                    pWork->bCanUpdateWindow = true;
                }

                percent = (100 * nCurPos) / pWork->nInTotalSize;
           
                if (pWork->bCanUpdateWindow == true)
                {
                    pWork->bCanUpdateWindow = false;
                    pWork->pWorkDlg->m_PrgCurrent.SetPos(percent);
                    pWork->bCanUpdateWindow = true;
                }

                nCurTotalPos = (100 * (nTotalSizeCounter + nCurPos)) / pWork->nTotalSize;

                if (pWork->bCanUpdateWindow == true)
                {
                    pWork->bCanUpdateWindow = false;
                    pWork->pWorkDlg->m_PrgTotal.SetPos(nCurTotalPos);
                    pWork->bCanUpdateWindow = true;
                }
            }
            t0 = t1;

            cIOWrite.Start();
            fwrite(frame, 1, fs, ofp);
            cIOWrite.Stop();

            if (pWork->bCanUpdateWindow == true)
            {
                pWork->bCanUpdateWindow = false;

                CString szTmpBuff;

                szTmpBuff.Format(_T("%0.1lf"), ((double)(_ftelli64(ofp)) / 1048576.0f) / (cIOWrite.ElapsedTime() + 1.0e-16));
                pWork->szSpeedWritesAvg = szTmpBuff;

                nOutPrevCurPos = _ftelli64(ofp);
                fPrevTimeIOWrite = cIOWrite.ElapsedTime();

                pWork->bCanUpdateWindow = true;
            }

            frame_cnt++;
            last_frame = nr;
        }
    } while (nr > 0 || fs > 0 || !frame_cnt);

    pWork->fTimeEncoding = cEncoding.ElapsedTime();
    pWork->fTimeIORead = cIORead.ElapsedTime();
    pWork->fTimeIOWrite = cIOWrite.ElapsedTime();
    pWork->nOutTotalSize = GetFileSizeInt64(ofp);
    pWork->fTimeTotal = pWork->fTimeEncoding + pWork->fTimeIORead + pWork->fTimeIOWrite;

    if (fwav)
        free(fwav);

    if (frame)
        free(frame);

    if (bAvisynthInput == false)
    {
        pcm_close(&pf);
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        decoderAVS.CloseAvisynth();
#endif
    }

    if (bAvisynthInput == false)
    {
        if (pWork->bMultiMonoInput == false)
        {
            nTotalSizeCounter += _ftelli64(ifp[0]);
        }
        else
        {
            for (int i = 0; i < nInputFiles; i++)
            {
                nTotalSizeCounter += _ftelli64(ifp[i]);
            }
        }
    }
    else
    {
#ifndef DISABLE_AVISYNTH
        nTotalSizeCounter += pWork->nInTotalSize;
#endif
    }

    for (int i = 0; i < nInputFiles; i++)
    {
        if (ifp[i])
            fclose(ifp[i]);
    }

    if (ofp)
        fclose(ofp);

    pWork->api.LibAften_aften_encode_close(&s);

    pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
    CString szBuff;
    szBuff.Format(_T("%s %s"),
        theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01005) : _T("Elapsed time:"),
        _T("00:00:00"));
    pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(szBuff);
    pWork->m_ElapsedTimeFile = 0L;

    for (int i = 0; i < nInputFiles; i++)
        szInPath[i].ReleaseBuffer();

    szOutPath.ReleaseBuffer();

    return(TRUE);
}

BOOL CWorker::Encode()
{
    CString szCommandLine = _T("");
    CString szBuff = _T("");
#ifdef _UNICODE
    const unsigned int nAnsiBuffSize = 8192;
    char szAnsiBuff[nAnsiBuffSize] = "";
    int nChars = 0;
#endif

    if (pWork->pWorkDlg == nullptr)
    {
        return(FALSE);
    }

    pWork->pWorkDlg->m_PrgCurrent.SetRange(0, 100);
    pWork->pWorkDlg->m_PrgTotal.SetRange32(0, 100);
    pWork->pWorkDlg->m_PrgCurrent.SetPos(0);
    pWork->pWorkDlg->m_PrgTotal.SetPos(0);
    pWork->pWorkDlg->KillTimer(WM_FILE_TIMER);
    pWork->m_ElapsedTimeTotal = 0L;
    szBuff.Format(_T("%s %s"),
        theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01006) : _T("Total elapsed time:"),
        _T("00:00:00"));
    pWork->pWorkDlg->m_StcTimeTotal.SetWindowText(szBuff);
    pWork->pWorkDlg->SetTimer(WM_TOTAL_TIMER, 250, nullptr);

    int nFileCounter = 0;
    int nTotalFiles = pWork->list->Count();
    int posStatus = 0;

    nTotalSizeCounter = 0;

    if (pWork->bMultiMonoInput == false)
    {
        for (int i = 0; i < pWork->list->Count(); i++)
        {
            szInPath[0] = _T("-");
            szInPath[1] = _T("-");
            szInPath[2] = _T("-");
            szInPath[3] = _T("-");
            szInPath[4] = _T("-");
            szInPath[5] = _T("-");
            szOutPath = _T("");

            szInPath[0] = pWork->list->Get(i);
            szOutPath = szInPath[0];
            szOutPath.Truncate(szOutPath.GetLength() - GetFileExtension(szOutPath).GetLength());
            szOutPath.Append(CEncoderDefaults::szSupportedOutputExt[0]);

            if (pWork->bUseOutPath == true)
            {
                CString szFile = GetFileName(szOutPath);

                if ((pWork->szOutPath[pWork->szOutPath.GetLength() - 1] == '\\') ||
                    (pWork->szOutPath[pWork->szOutPath.GetLength() - 1] == '/'))
                    szOutPath = pWork->szOutPath + szFile;
                else
                    szOutPath = pWork->szOutPath + '\\' + szFile;
            }

            CString szTitle;
            szTitle.Format(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A0100C) : _T("Encoding file %d of %d"),
                nFileCounter + 1,
                nTotalFiles);
            pWork->pWorkDlg->SetWindowText(szTitle);

            szBuff.Format(_T("%s\t%s"),
                theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01003) : _T("From:"),
                szInPath[0]);
            pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDIn[0])->SetWindowText(szBuff);

            szBuff.Format(_T("%s\t%s"),
                theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01004) : _T("To:"),
                szOutPath);
            pWork->pWorkDlg->m_StcOut.SetWindowText(szBuff);

            szBuff.Format(_T("%s %s"),
                theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01005) : _T("Elapsed time:"),
                _T("00:00:00"));
            pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(szBuff);
            pWork->m_ElapsedTimeFile = 0L;
            pWork->pWorkDlg->SetTimer(WM_FILE_TIMER, 250, nullptr);
            pWork->pWorkDlg->m_PrgCurrent.SetPos(0);

            ZeroMemory(&s, sizeof(AftenContext));
            ZeroMemory(&opt, sizeof(AftenOpt));
            InitContext(pWork->preset, pWork->api, opt, s);

            nInputFiles = 1;

            if (Run() == FALSE)
            {
                bool result = false;
                pWork->listStatus->Set(result, posStatus);
                return(FALSE);
            }
            else
            {
                bool result = true;
                pWork->listStatus->Set(result, posStatus);
            }

            posStatus++;
            nFileCounter++;
            pWork->nCount = nFileCounter;
        }
    }
    else
    {
        szInPath[0] = _T("-");
        szInPath[1] = _T("-");
        szInPath[2] = _T("-");
        szInPath[3] = _T("-");
        szInPath[4] = _T("-");
        szInPath[5] = _T("-");
        szOutPath = _T("");

        nFileCounter = pWork->list->Count();

        for (int i = 0; i < nFileCounter; i++)
        {
            szInPath[i] = pWork->list->Get(i);
        }

        szOutPath = szInPath[0];
        szOutPath.Truncate(szOutPath.GetLength() - GetFileExtension(szOutPath).GetLength());
        szOutPath.Append(CEncoderDefaults::szSupportedOutputExt[0]);

        if (pWork->bUseOutPath == true)
            szOutPath = pWork->szOutPath;

        CString szTitle;
        szTitle.Format(theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A0100D) : _T("Encoding %d mono files"),
            nTotalFiles);
        pWork->pWorkDlg->SetWindowText(szTitle);

        szBuff.Format(_T("%s\t%s"),
            theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01003) : _T("From:"),
            szInPath[0]);
        pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDIn[0])->SetWindowText(szBuff);

        for (int i = 1; i < nFileCounter; i++)
            pWork->pWorkDlg->GetDlgItem(pWork->pWorkDlg->nIDIn[i])->SetWindowText(_T("\t") + szInPath[i]);

        szBuff.Format(_T("%s\t%s"),
            theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01004) : _T("To:"),
            szOutPath);
        pWork->pWorkDlg->m_StcOut.SetWindowText(szBuff);

        szBuff.Format(_T("%s %s"),
            theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00A01005) : _T("Elapsed time:"),
            _T("00:00:00"));
        pWork->pWorkDlg->m_StcTimeCurrent.SetWindowText(szBuff);
        pWork->m_ElapsedTimeFile = 0L;
        pWork->pWorkDlg->SetTimer(WM_FILE_TIMER, 250, nullptr);
        pWork->pWorkDlg->m_PrgCurrent.SetPos(0);

        ZeroMemory(&s, sizeof(AftenContext));
        ZeroMemory(&opt, sizeof(AftenOpt));
        InitContext(pWork->preset, pWork->api, opt, s);

        nInputFiles = nFileCounter;

        if (Run() == FALSE)
        {
            for (int i = 0; i < pWork->listStatus->Count(); i++)
            {
                bool result = false;
                pWork->listStatus->Set(result, i);
            }

            pWork->nCount = 0;

            return(FALSE);
        }
        else
        {
            for (int i = 0; i < pWork->listStatus->Count(); i++)
            {
                bool result = true;
                pWork->listStatus->Set(result, i);
            }

            pWork->nCount = nFileCounter;
        }
    }

    pWork->pWorkDlg->KillTimer(WM_TOTAL_TIMER);
    pWork->bTerminate = true;
    ::PostMessage(pWork->pWorkDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);

    return(TRUE);
}

DWORD WINAPI EncWorkThread(LPVOID pParam)
{
    CWorkerParam *pWork = (CWorkerParam *)pParam;
    CWorker m_Worker;
	m_Worker.pWork = pWork;
    return m_Worker.Encode();
}
