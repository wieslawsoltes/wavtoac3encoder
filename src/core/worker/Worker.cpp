#include "StdAfx.h"
#include "Worker.h"

namespace worker
{
    void CWorker::InitContext(const config::CPreset *preset, const AftenAPI &api, AftenOpt &opt, AftenContext &s)
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

        if (preset->m_RawInput.nRawSampleFormat != 0)
        {
            switch (preset->m_RawInput.nRawSampleFormat)
            {
            case 1: opt.raw_fmt = PCM_SAMPLE_FMT_U8; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 2: opt.raw_fmt = PCM_SAMPLE_FMT_S8; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 3: opt.raw_fmt = PCM_SAMPLE_FMT_S16; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 4: opt.raw_fmt = PCM_SAMPLE_FMT_S16; opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 5: opt.raw_fmt = PCM_SAMPLE_FMT_S20; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 6: opt.raw_fmt = PCM_SAMPLE_FMT_S20; opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 7: opt.raw_fmt = PCM_SAMPLE_FMT_S24; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 8: opt.raw_fmt = PCM_SAMPLE_FMT_S24; opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 9: opt.raw_fmt = PCM_SAMPLE_FMT_S32; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 10: opt.raw_fmt = PCM_SAMPLE_FMT_S32; opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 11: opt.raw_fmt = PCM_SAMPLE_FMT_FLT; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 12: opt.raw_fmt = PCM_SAMPLE_FMT_FLT; opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 13: opt.raw_fmt = PCM_SAMPLE_FMT_DBL; opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 14: opt.raw_fmt = PCM_SAMPLE_FMT_DBL; opt.raw_order = PCM_BYTE_ORDER_BE; break;
            };

            opt.raw_input = 1;
        }

        if (preset->m_RawInput.nRawSampleRate != 0)
        {
            opt.raw_sr = preset->m_RawInput.nRawSampleRate;
            opt.raw_input = 1;
        }

        if (preset->m_RawInput.nRawChannels != 0)
        {
            opt.raw_ch = preset->m_RawInput.nRawChannels;
            opt.raw_input = 1;
        }

        #define SetSetting(set, type) \
            nOption++; \
            { \
                auto& option = config::m_Config.m_EncoderOptions.m_Options[nOption]; \
                int nOptionValue = preset->nOptions.at(nOption); \
                if(option.nIgnoreValue != nOptionValue) \
                { \
                    int nValue = option.m_Values[nOptionValue].second; \
                    (set) = (type) nValue; \
                } \
            }

        int nOption = -1;

        SetSetting(s.params.bitalloc_fast, int)
        SetSetting(s.params.expstr_search, int)
        SetSetting(opt.pad_start, int)
        SetSetting(s.params.bwcode, int)
        SetSetting(s.params.min_bwcode, int)
        SetSetting(s.params.max_bwcode, int)
        SetSetting(s.params.use_rematrixing, int)
        SetSetting(s.params.use_block_switching, int)
        SetSetting(s.meta.cmixlev, int)
        SetSetting(s.meta.surmixlev, int)
        SetSetting(s.meta.dsurmod, int)
        SetSetting(s.meta.dialnorm, int)
        SetSetting(s.params.dynrng_profile, DynRngProfile)
        SetSetting(s.acmod, int)
        SetSetting(s.lfe, int)

        nOption++;
        {
            auto& option = config::m_Config.m_EncoderOptions.m_Options[nOption];
            int nOptionValue = preset->nOptions.at(nOption);
            if (option.nIgnoreValue != nOptionValue)
            {
                int nValue = option.m_Values[nOptionValue].second;
                s.acmod = config::m_Config.m_EncoderOptions.ccAften[nValue].acmod;
                s.lfe = config::m_Config.m_EncoderOptions.ccAften[nValue].lfe;
            }
        }

        SetSetting(opt.chmap, int)
        SetSetting(opt.read_to_eof, int)
        SetSetting(s.params.use_bw_filter, int)
        SetSetting(s.params.use_dc_filter, int)
        SetSetting(s.params.use_lfe_filter, int)
        SetSetting(s.meta.xbsi1e, int)
        SetSetting(s.meta.dmixmod, int)
        SetSetting(s.meta.ltrtcmixlev, int)
        SetSetting(s.meta.ltrtsmixlev, int)
        SetSetting(s.meta.lorocmixlev, int)
        SetSetting(s.meta.lorosmixlev, int)
        SetSetting(s.meta.xbsi2e, int)
        SetSetting(s.meta.dsurexmod, int)
        SetSetting(s.meta.dheadphonmod, int)
        SetSetting(s.meta.adconvtyp, int)

        #undef SetSetting
    }

    void CWorker::UpdateProgress()
    {
        if (bAvisynthInput == false)
        {
            CAtlString szInputInfo = _T("");

            for (int i = 0; i < nInputFiles; i++)
            {
                PcmFile *pf_info = &pf.pcm_file[i];
                std::wstring type, chan, order;
                std::wstring fmt = L"";

                type = pContext->pConfig->GetString(0x00A02001).c_str();
                chan = pContext->pConfig->GetString(0x00A02002).c_str();
                order = L"";

                if (pf_info->sample_type == PCM_SAMPLE_TYPE_INT)
                {
                    if (pf_info->source_format == PCM_SAMPLE_FMT_U8)
                        type = pContext->pConfig->GetString(0x00A02003).c_str();
                    else
                        type = pContext->pConfig->GetString(0x00A02004).c_str();
                }
                else if (pf_info->sample_type == PCM_SAMPLE_TYPE_FLOAT)
                {
                    type = pContext->pConfig->GetString(0x00A02005).c_str();
                }
                else
                {
                    type = pContext->pConfig->GetString(0x00A02006).c_str();
                }

                if (pf_info->ch_mask & 0x08)
                {
                    switch (pf_info->channels - 1)
                    {
                    case 1: chan = pContext->pConfig->GetString(0x00A02007).c_str(); break;
                    case 2: chan = pContext->pConfig->GetString(0x00A02008).c_str(); break;
                    case 3: chan = pContext->pConfig->GetString(0x00A02009).c_str(); break;
                    case 4: chan = pContext->pConfig->GetString(0x00A0200A).c_str(); break;
                    case 5: chan = pContext->pConfig->GetString(0x00A0200B).c_str(); break;
                    default: chan = pContext->pConfig->GetString(0x00A0200C).c_str(); break;
                    }
                }
                else
                {
                    switch (pf_info->channels)
                    {
                    case 1: chan = pContext->pConfig->GetString(0x00A0200D).c_str(); break;
                    case 2: chan = pContext->pConfig->GetString(0x00A0200E).c_str(); break;
                    case 3: chan = pContext->pConfig->GetString(0x00A0200F).c_str(); break;
                    case 4: chan = pContext->pConfig->GetString(0x00A02010).c_str(); break;
                    case 5: chan = pContext->pConfig->GetString(0x00A02011).c_str(); break;
                    case 6: chan = pContext->pConfig->GetString(0x00A02012).c_str(); break;
                    default: chan = pContext->pConfig->GetString(0x00A02013).c_str(); break;
                    }
                }

                if (pf_info->pcm_format)
                {
                    std::string szLongName = pf_info->pcm_format->long_name;
                    fmt = util::StringHelper::Convert(szLongName);
                }
                else
                {
                    fmt = pContext->pConfig->GetString(0x00A02014);
                }

                if (pf_info->source_format > PCM_SAMPLE_FMT_S8)
                {
                    switch (pf_info->order)
                    {
                    case PCM_BYTE_ORDER_LE: order = pContext->pConfig->GetString(0x00A02015).c_str(); break;
                    case PCM_BYTE_ORDER_BE: order = pContext->pConfig->GetString(0x00A02016).c_str(); break;
                    }
                }
                else
                {
                    order = _T("\b");
                }

                szInputInfo.Format(_T("\t%s %s %d-bit %s %d Hz %s"),
                    fmt.c_str(), type.c_str(), pf_info->bit_width, order.c_str(), pf_info->sample_rate, chan.c_str());
                std::wstring szInputInfoStr = szInputInfo;
                pContext->SetInputTypeInfo(i, szInputInfoStr);
            }
        }
        else
        {
            CAtlString szInputInfo = _T("");
            std::wstring chan = L"?-channel";

            switch (infoAVS.nAudioChannels)
            {
            case 1: chan = pContext->pConfig->GetString(0x00A0200D).c_str(); break;
            case 2: chan = pContext->pConfig->GetString(0x00A0200E).c_str(); break;
            case 3: chan = pContext->pConfig->GetString(0x00A0200F).c_str(); break;
            case 4: chan = pContext->pConfig->GetString(0x00A02010).c_str(); break;
            case 5: chan = pContext->pConfig->GetString(0x00A02011).c_str(); break;
            case 6: chan = pContext->pConfig->GetString(0x00A02012).c_str(); break;
            default: chan = pContext->pConfig->GetString(0x00A02013).c_str(); break;
            }

            szInputInfo.Format(_T("\t%s %d Hz %s"),
                pContext->pConfig->GetString(0x00A02017).c_str(),
                infoAVS.nSamplesPerSecond, chan.c_str());
            std::wstring szInputInfoStr = szInputInfo;
            pContext->SetInputTypeInfo(0, szInputInfoStr);
        }

        {
        CAtlString szOutputInfo = _T("");
            std::wstring acmod_str[] =
            {
                pContext->pConfig->GetString(0x00A02018),
                pContext->pConfig->GetString(0x00A02019),
                pContext->pConfig->GetString(0x00A0201A),
                L"3/0",
                L"2/1",
                L"3/1",
                L"2/2",
                L"3/2"
            };

            szOutputInfo.Format(_T("\tAC3 %d Hz %s"), s.samplerate, acmod_str[s.acmod].c_str());
            if (s.lfe)
                szOutputInfo += _T(" + LFE");
            std::wstring szOutputInfoStr = szOutputInfo;
            pContext->SetOutputTypeInfo(szOutputInfoStr);
        }
    }

    BOOL CWorker::HandleError(LPTSTR pszMessage)
    {
        pContext->StopCurrentTimer();
        std::wstring szBuff = pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00");
        pContext->SetCurrentTimerInfo(szBuff);
        pContext->m_ElapsedTimeFile = 0L;

        if (fwav)
            free(fwav);

        if (frame)
            free(frame);

        if (bAvisynthInput == false)
        {
            pcm_close(&pf);
            for (int i = 0; i < nInputFiles; i++)
            {
                if (ifp[i])
                    fclose(ifp[i]);
            }
        }
        else
        {
            decoderAVS.CloseAvisynth();
        }

        if (ofp)
            fclose(ofp);

        pContext->api.LibAften_aften_encode_close(&s);

        pContext->bTerminate = true;
        pContext->Close();

        return(FALSE);
    }

    BOOL CWorker::Run()
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

        frame = nullptr;
        fwav = nullptr;

        for (int i = 0; i < nInputFiles; i++)
            ifp[i] = nullptr;

        ofp = nullptr;

        bAvisynthInput = false;
        std::wstring szExt = util::Utilities::GetFileExtension(szInPath[0]);
        if (util::StringHelper::TowLower(szExt) == L"avs")
            bAvisynthInput = true;

        pContext->nInTotalSize = 0;

        memset(ifp, 0, 6 * sizeof(FILE *));

        char szInputFileAVS[MAX_PATH] = "";
        if (bAvisynthInput == true)
        {
            util::Utilities::ConvertUnicodeToAnsi(szInPath[0].c_str(), szInputFileAVS, szInPath[0].length());
            if (decoderAVS.OpenAvisynth(szInputFileAVS) == false)
            {
                OutputDebugString(_T("Failed to initialize Avisynth."));
                return(FALSE);
            }
            else
            {
                infoAVS = decoderAVS.GetInputInfo();
                pContext->nInTotalSize = infoAVS.nAudioSamples * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
                OutputDebugString(_T("Avisynth initialized successfully."));
            }
        }
        else
        {
            for (int i = 0; i < nInputFiles; i++)
            {
                errno_t error = _tfopen_s(&ifp[i], szInPath[i].c_str(), _T("rb"));
                if (error != 0)
                {
                    OutputDebugString(_T("Failed to open input file: ") + CAtlString(szInPath[i].c_str()));
                    pContext->StopCurrentTimer();

                    std::wstring szBuff = pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00");
                    pContext->SetCurrentTimerInfo(szBuff);

                    pContext->m_ElapsedTimeFile = 0L;
                    pContext->bTerminate = true;
                    pContext->Close();

                    return(FALSE);
                }
                else
                {
                    pContext->nInTotalSize += util::Utilities::GetFileSizeInt64(ifp[i]);
                }
            }
        }

        errno_t error = _tfopen_s(&ofp, szOutPath.c_str(), _T("wb"));
        if (error != 0)
        {
            std::wstring szBuff = pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00");
            pContext->SetCurrentTimerInfo(szBuff);
            pContext->StopCurrentTimer();
            pContext->m_ElapsedTimeFile = 0L;

            for (int i = 0; i < nInputFiles; i++)
            {
                if (ifp[i])
                    fclose(ifp[i]);
            }

            OutputDebugString(_T("Failed to create output file: ") + CAtlString(szOutPath.c_str()));

            pContext->bTerminate = true;
            pContext->Close();

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
            input_file_format = config::m_Config.m_EncoderOptions.GetSupportedInputFormat(util::Utilities::GetFileExtension(szInPath[0]));
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
            if (opt.raw_input)
            {
            }
        }

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

            if (pContext->api.LibAften_aften_wav_channels_to_acmod(ch, pf.ch_mask, &s.acmod, &s.lfe))
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
            s.sample_format = A52_SAMPLE_FMT_FLT;
            s.channels = infoAVS.nAudioChannels;
            s.samplerate = infoAVS.nSamplesPerSecond;
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
            aften_remap = pContext->api.LibAften_aften_remap_wav_to_a52;
        else if (opt.chmap == 2)
            aften_remap = pContext->api.LibAften_aften_remap_mpeg_to_a52;

        if (!opt.pad_start)
        {
            int diff;

            if (bAvisynthInput == false)
            {
                nr = pcm_read_samples(&pf, fwav, 256);
            }
            else
            {
                statusAVS.nSamplesToRead = 256;
                nr = decoderAVS.GetAudio(fwav, &statusAVS);
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

        if (pContext->api.LibAften_aften_encode_init(&s))
        {
            return HandleError(_T("Failed to initialize encoder."));
        }

        UpdateProgress();

        int nCurTotalPos = 0;
        __int64 nCurPos = 0;
        __int64 nInPrevCurPos = 0;
        __int64 nOutPrevCurPos = 0;

        do
        {
            if (pContext->bTerminate == true)
            {
                while (fs > 0)
                {
                    fs = pContext->api.LibAften_aften_encode_frame(&s, frame, nullptr, 0);
                    if (fs > 0)
                        fwrite(frame, 1, fs, ofp);
                }

                return HandleError(_T("User has terminated encoding."));
            }

            if (bAvisynthInput == false)
            {
                nr = pcm_read_samples(&pf, fwav, A52_SAMPLES_PER_FRAME);
            }
            else
            {
                statusAVS.nSamplesToRead = A52_SAMPLES_PER_FRAME;
                nr = decoderAVS.GetAudio(fwav, &statusAVS);
            }

            if (aften_remap)
                aften_remap(fwav, nr, s.channels, s.sample_format, s.acmod);

            fs = pContext->api.LibAften_aften_encode_frame(&s, frame, fwav, nr);

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
                        if (pContext->bMultiMonoInput == false)
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
                        nCurPos = samplecount * infoAVS.nBytesPerChannelSample * infoAVS.nAudioChannels;
                    }

                    if (pContext->bCanUpdateWindow == true)
                    {
                        pContext->bCanUpdateWindow = false;
                        nInPrevCurPos = nCurPos;
                        pContext->bCanUpdateWindow = true;
                    }

                    percent = (100 * nCurPos) / pContext->nInTotalSize;

                    if (pContext->bCanUpdateWindow == true)
                    {
                        pContext->bCanUpdateWindow = false;
                        pContext->SetCurrentProgress(percent);
                        pContext->bCanUpdateWindow = true;
                    }

                    nCurTotalPos = (100 * (nTotalSizeCounter + nCurPos)) / pContext->nTotalSize;

                    if (pContext->bCanUpdateWindow == true)
                    {
                        pContext->bCanUpdateWindow = false;
                        pContext->SetTotalProgress(nCurTotalPos);
                        pContext->bCanUpdateWindow = true;
                    }
                }
                t0 = t1;

                fwrite(frame, 1, fs, ofp);

                if (pContext->bCanUpdateWindow == true)
                {
                    pContext->bCanUpdateWindow = false;
                    nOutPrevCurPos = _ftelli64(ofp);
                    pContext->bCanUpdateWindow = true;
                }

                frame_cnt++;
                last_frame = nr;
            }
        } while (nr > 0 || fs > 0 || !frame_cnt);

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
            decoderAVS.CloseAvisynth();
        }

        if (bAvisynthInput == false)
        {
            if (pContext->bMultiMonoInput == false)
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
            nTotalSizeCounter += pContext->nInTotalSize;
        }

        for (int i = 0; i < nInputFiles; i++)
        {
            if (ifp[i])
                fclose(ifp[i]);
        }

        if (ofp)
            fclose(ofp);

        pContext->api.LibAften_aften_encode_close(&s);

        pContext->StopCurrentTimer();
        std::wstring szBuff = pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00");
        pContext->SetCurrentTimerInfo(szBuff);
        pContext->m_ElapsedTimeFile = 0L;

        return(TRUE);
    }

    BOOL CWorker::Encode()
    {
        std::wstring szBuff = L"";
        const unsigned int nAnsiBuffSize = 8192;
        char szAnsiBuff[nAnsiBuffSize] = "";
        int nChars = 0;

        pContext->SetCurrentProgressRange(0, 100);
        pContext->SetTotalProgressRange(0, 100);
        pContext->SetCurrentProgress(0);
        pContext->SetTotalProgress(0);
        pContext->StopCurrentTimer();
        pContext->m_ElapsedTimeTotal = 0L;

        szBuff = pContext->pConfig->GetString(0x00A01006) + std::wstring(L" 00:00:00");

        pContext->SetTotalTimerInfo(szBuff);
        pContext->StartTotalTimer(250);

        int nFileCounter = 0;
        int nTotalFiles = (int)pContext->pFilesList->size();
        int posStatus = 0;

        nTotalSizeCounter = 0;

        if (pContext->bMultiMonoInput == false)
        {
            for (int i = 0; i < (int)pContext->pFilesList->size(); i++)
            {
                szInPath[0] = L"-";
                szInPath[1] = L"-";
                szInPath[2] = L"-";
                szInPath[3] = L"-";
                szInPath[4] = L"-";
                szInPath[5] = L"-";
                szOutPath = L"";

                szInPath[0] = (*pContext->pFilesList)[i];
                szOutPath = szInPath[0];
                
                std::wstring szExt = util::Utilities::GetFileExtension(szOutPath);
                szOutPath = szOutPath.substr(0, szOutPath.length() - szExt.length()) + L"." + config::m_Config.m_EncoderOptions.szSupportedOutputExt[0];

                if (pContext->bUseOutPath == true)
                {
                    std::wstring szFile = util::Utilities::GetFileName(szOutPath);
                    szOutPath = util::Utilities::CombinePath(pContext->szOutPath, szFile);
                }

                CAtlString szTitle;
                szTitle.Format(pContext->pConfig->GetString(0x00A0100C).c_str(),
                    nFileCounter + 1,
                    nTotalFiles);
                std::wstring szTitleStr = szTitle;
                pContext->SetTitleInfo(szTitleStr);

                szBuff = pContext->pConfig->GetString(0x00A01003) + L"\t" + szInPath[0];
                pContext->SetInputFileInfo(0, szBuff);

                szBuff = pContext->pConfig->GetString(0x00A01004) + L"\t" + szOutPath;
                pContext->SetOutputFileInfo(szBuff);

                szBuff = pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00");
                pContext->SetCurrentTimerInfo(szBuff);

                pContext->m_ElapsedTimeFile = 0L;
                pContext->StartCurrentTimer(250);
                pContext->SetCurrentProgress(0);

                ZeroMemory(&s, sizeof(AftenContext));
                ZeroMemory(&opt, sizeof(AftenOpt));
                InitContext(pContext->pPreset, pContext->api, opt, s);

                nInputFiles = 1;

                if (Run() == FALSE)
                {
                    (*pContext->pStatusList)[posStatus] = false;
                    return(FALSE);
                }
                else
                {
                    (*pContext->pStatusList)[posStatus] = true;
                }

                posStatus++;
                nFileCounter++;
                pContext->nCount = nFileCounter;
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

            nFileCounter = (int)pContext->pFilesList->size();

            for (int i = 0; i < nFileCounter; i++)
            {
                szInPath[i] = (*pContext->pFilesList)[i];
            }

            szOutPath = szInPath[0];

            std::wstring szExt = util::Utilities::GetFileExtension(szOutPath);
            szOutPath = szOutPath.substr(0, szOutPath.length() - szExt.length()) + L"." + config::m_Config.m_EncoderOptions.szSupportedOutputExt[0];

            if (pContext->bUseOutPath == true)
                szOutPath = pContext->szOutPath;

            CAtlString szTitle;
            szTitle.Format(pContext->pConfig->GetString(0x00A0100D).c_str(),
                nTotalFiles);
            std::wstring szTitleStr = szTitle;
            pContext->SetTitleInfo(szTitleStr);

            szBuff = pContext->pConfig->GetString(0x00A01003) + L"\t" + szInPath[0];
            pContext->SetInputFileInfo(0, szBuff);

            for (int i = 1; i < nFileCounter; i++)
                pContext->SetInputFileInfo(i, L"\t" + szInPath[i]);

            szBuff = pContext->pConfig->GetString(0x00A01004) + L"\t" + szOutPath;
            pContext->SetOutputFileInfo(szBuff);

            szBuff = pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00");
            pContext->SetCurrentTimerInfo(szBuff);

            pContext->m_ElapsedTimeFile = 0L;
            pContext->StartCurrentTimer(250);
            pContext->SetCurrentProgress(0);

            ZeroMemory(&s, sizeof(AftenContext));
            ZeroMemory(&opt, sizeof(AftenOpt));
            InitContext(pContext->pPreset, pContext->api, opt, s);

            nInputFiles = nFileCounter;

            if (Run() == FALSE)
            {
                for (int i = 0; i < (int)pContext->pStatusList->size(); i++)
                {
                    (*pContext->pStatusList)[i] = false;
                }
                pContext->nCount = 0;
                return(FALSE);
            }
            else
            {
                for (int i = 0; i < (int)pContext->pStatusList->size(); i++)
                {
                    (*pContext->pStatusList)[i] = true;
                }
                pContext->nCount = nFileCounter;
            }
        }

        pContext->StopTotalTimer();
        pContext->bTerminate = true;
        pContext->Close();

        return(TRUE);
    }
}
