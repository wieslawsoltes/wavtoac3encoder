#include "StdAfx.h"
#include "Worker.h"

namespace worker
{
    void CWorker::SetInfo(CState& state)
    {
        if (state.bAvisynthInput == false)
        {
            CAtlString szInputInfo = _T("");

            for (int i = 0; i < state.nInputFiles; i++)
            {
                PcmFile *pf_info = &state.pf.pcm_file[i];
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

            switch (state.infoAVS.nAudioChannels)
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
                state.infoAVS.nSamplesPerSecond, chan.c_str());
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

            szOutputInfo.Format(_T("\tAC3 %d Hz %s"), state.s.samplerate, acmod_str[state.s.acmod].c_str());
            if (state.s.lfe)
                szOutputInfo += _T(" + LFE");
            std::wstring szOutputInfoStr = szOutputInfo;
            pContext->SetOutputTypeInfo(szOutputInfoStr);
        }
    }

    bool CWorker::InitEngine(CState& state)
    {
        if (state.api.OpenAftenAPI(state.engine->szPath) == false)
        {
            this->pContext->pConfig->Log->Log(L"[Error] Failed to load '" + state.engine->szName + L"' library: " + state.engine->szPath);
            return false;
        }
        else
        {
            const char *version = state.api.LibAften_aften_get_version();
            std::wstring szVersion = util::StringHelper::Convert(version);
            this->pContext->pConfig->Log->Log( L"[Info] Loaded '" + state.engine->szName + L"' library" + L", version " + szVersion + L": " + state.engine->szPath);
        }

        state.api.LibAften_aften_set_defaults(&state.s);

        state.s.system.wanted_simd_instructions.mmx = state.engine->nUsedSIMD.at(0);
        state.s.system.wanted_simd_instructions.sse = state.engine->nUsedSIMD.at(1);
        state.s.system.wanted_simd_instructions.sse2 = state.engine->nUsedSIMD.at(2);
        state.s.system.wanted_simd_instructions.sse3 = state.engine->nUsedSIMD.at(3);
        state.s.system.n_threads = state.engine->nThreads;

        state.s.params.encoding_mode = state.preset->nMode;
        state.s.params.bitrate = state.preset->nBitrate;
        state.s.params.quality = state.preset->nQuality;

        if (state.preset->m_RawInput.nRawSampleFormat != 0)
        {
            switch (state.preset->m_RawInput.nRawSampleFormat)
            {
            case 1: state.opt.raw_fmt = PCM_SAMPLE_FMT_U8; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 2: state.opt.raw_fmt = PCM_SAMPLE_FMT_S8; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 3: state.opt.raw_fmt = PCM_SAMPLE_FMT_S16; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 4: state.opt.raw_fmt = PCM_SAMPLE_FMT_S16; state.opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 5: state.opt.raw_fmt = PCM_SAMPLE_FMT_S20; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 6: state.opt.raw_fmt = PCM_SAMPLE_FMT_S20; state.opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 7: state.opt.raw_fmt = PCM_SAMPLE_FMT_S24; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 8: state.opt.raw_fmt = PCM_SAMPLE_FMT_S24; state.opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 9: state.opt.raw_fmt = PCM_SAMPLE_FMT_S32; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 10: state.opt.raw_fmt = PCM_SAMPLE_FMT_S32; state.opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 11: state.opt.raw_fmt = PCM_SAMPLE_FMT_FLT; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 12: state.opt.raw_fmt = PCM_SAMPLE_FMT_FLT; state.opt.raw_order = PCM_BYTE_ORDER_BE; break;
            case 13: state.opt.raw_fmt = PCM_SAMPLE_FMT_DBL; state.opt.raw_order = PCM_BYTE_ORDER_LE; break;
            case 14: state.opt.raw_fmt = PCM_SAMPLE_FMT_DBL; state.opt.raw_order = PCM_BYTE_ORDER_BE; break;
            };
            state.opt.raw_input = 1;
        }

        if (state.preset->m_RawInput.nRawSampleRate != 0)
        {
            state.opt.raw_sr = state.preset->m_RawInput.nRawSampleRate;
            state.opt.raw_input = 1;
        }

        if (state.preset->m_RawInput.nRawChannels != 0)
        {
            state.opt.raw_ch = state.preset->m_RawInput.nRawChannels;
            state.opt.raw_input = 1;
        }

        #define SetSetting(set, type) \
            nOption++; \
            { \
                auto& option = pContext->pConfig->m_EncoderOptions.m_Options[nOption]; \
                int nOptionValue = state.preset->nOptions.at(nOption); \
                if(option.nIgnoreValue != nOptionValue) \
                { \
                    int nValue = option.m_Values[nOptionValue].second; \
                    (set) = (type) nValue; \
                } \
            }

        int nOption = -1;

        SetSetting(state.s.params.bitalloc_fast, int)
        SetSetting(state.s.params.expstr_search, int)
        SetSetting(state.opt.pad_start, int)
        SetSetting(state.s.params.bwcode, int)
        SetSetting(state.s.params.min_bwcode, int)
        SetSetting(state.s.params.max_bwcode, int)
        SetSetting(state.s.params.use_rematrixing, int)
        SetSetting(state.s.params.use_block_switching, int)
        SetSetting(state.s.meta.cmixlev, int)
        SetSetting(state.s.meta.surmixlev, int)
        SetSetting(state.s.meta.dsurmod, int)
        SetSetting(state.s.meta.dialnorm, int)
        SetSetting(state.s.params.dynrng_profile, DynRngProfile)
        SetSetting(state.s.acmod, int)
        SetSetting(state.s.lfe, int)

        nOption++;
        {
            auto& option = pContext->pConfig->m_EncoderOptions.m_Options[nOption];
            int nOptionValue = state.preset->nOptions.at(nOption);
            if (option.nIgnoreValue != nOptionValue)
            {
                int nValue = option.m_Values[nOptionValue].second;
                auto& channelConfig = pContext->pConfig->m_EncoderOptions.m_ChannelConfig[nValue];
                state.s.acmod = channelConfig.acmod;
                state.s.lfe = channelConfig.lfe;
            }
        }

        SetSetting(state.opt.chmap, int)
        SetSetting(state.opt.read_to_eof, int)
        SetSetting(state.s.params.use_bw_filter, int)
        SetSetting(state.s.params.use_dc_filter, int)
        SetSetting(state.s.params.use_lfe_filter, int)
        SetSetting(state.s.meta.xbsi1e, int)
        SetSetting(state.s.meta.dmixmod, int)
        SetSetting(state.s.meta.ltrtcmixlev, int)
        SetSetting(state.s.meta.ltrtsmixlev, int)
        SetSetting(state.s.meta.lorocmixlev, int)
        SetSetting(state.s.meta.lorosmixlev, int)
        SetSetting(state.s.meta.xbsi2e, int)
        SetSetting(state.s.meta.dsurexmod, int)
        SetSetting(state.s.meta.dheadphonmod, int)
        SetSetting(state.s.meta.adconvtyp, int)

        #undef SetSetting

        return true;
    }

    bool CWorker::EncoderError(CState& state, const std::wstring szMessage)
    {
        if (state.fwav)
            free(state.fwav);

        if (state.frame)
            free(state.frame);

        if (state.bAvisynthInput == false)
        {
            pcm_close(&state.pf);
            for (int i = 0; i < state.nInputFiles; i++)
            {
                if (state.ifp[i])
                    fclose(state.ifp[i]);
            }
        }
        else
        {
            state.decoderAVS.CloseAvisynth();
        }

        if (state.ofp)
            fclose(state.ofp);

        state.api.LibAften_aften_encode_close(&state.s);

        pContext->StopCurrentTimer();
        pContext->SetCurrentTimerInfo(pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00"));
        pContext->m_ElapsedTimeFile = 0L;

        this->pContext->pConfig->Log->Log(szMessage);
        return false;
    }

    bool CWorker::Encode(CState& state)
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

        state.frame = nullptr;
        state.fwav = nullptr;

        for (int i = 0; i < state.nInputFiles; i++)
            state.ifp[i] = nullptr;

        state.ofp = nullptr;

        state.bAvisynthInput = false;
        std::wstring szExt = util::Utilities::GetFileExtension(state.szInPath[0]);
        if (util::StringHelper::TowLower(szExt) == L"avs")
            state.bAvisynthInput = true;

        state.nInTotalSize = 0;

        memset(state.ifp, 0, 6 * sizeof(FILE *));

        if (state.bAvisynthInput == true)
        {
            std::string szInputFileAVS = util::StringHelper::Convert(state.szInPath[0]);
            if (state.decoderAVS.OpenAvisynth(szInputFileAVS.c_str()) == false)
            {
                this->pContext->pConfig->Log->Log(L"[Error] Failed to initialize Avisynth: " + state.szInPath[0]);
                return false;
            }
            state.infoAVS = state.decoderAVS.GetInputInfo();
            state.nInTotalSize = state.infoAVS.nAudioSamples * state.infoAVS.nBytesPerChannelSample * state.infoAVS.nAudioChannels;
            this->pContext->pConfig->Log->Log(L"[Info] Avisynth initialized successfully: " + state.szInPath[0]);
        }
        else
        {
            for (int i = 0; i < state.nInputFiles; i++)
            {
                errno_t error = _tfopen_s(&state.ifp[i], state.szInPath[i].c_str(), _T("rb"));
                if (error != 0)
                {
                    this->pContext->pConfig->Log->Log(L"[Error] Failed to open input file: " + state.szInPath[i]);
                    pContext->StopCurrentTimer();
                    pContext->SetCurrentTimerInfo(pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00"));
                    pContext->m_ElapsedTimeFile = 0L;
                    return false;
                }
                state.nInTotalSize += util::Utilities::GetFileSizeInt64(state.ifp[i]);
                this->pContext->pConfig->Log->Log(L"[Infio] Input file: " + state.szInPath[i]);
            }
        }

        errno_t error = _tfopen_s(&state.ofp, state.szOutPath.c_str(), _T("wb"));
        if (error != 0)
        {
            pContext->SetCurrentTimerInfo(pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00"));
            pContext->StopCurrentTimer();
            pContext->m_ElapsedTimeFile = 0L;
            for (int i = 0; i < state.nInputFiles; i++)
            {
                if (state.ifp[i])
                    fclose(state.ifp[i]);
            }
            this->pContext->pConfig->Log->Log(L"[Error] Failed to create output file: " + state.szOutPath);
            return false;
        }

        this->pContext->pConfig->Log->Log(L"[Info] Output file: " + state.szOutPath);

#ifdef CONFIG_DOUBLE
        read_format = PCM_SAMPLE_FMT_DBL;
#else
        read_format = PCM_SAMPLE_FMT_FLT;
#endif

        input_file_format = PCM_FORMAT_UNKNOWN;

        if ((state.opt.raw_input) || (state.bAvisynthInput == true))
        {
            input_file_format = PCM_FORMAT_RAW;
        }
        else
        {
            input_file_format = pContext->pConfig->GetSupportedInputFormat(util::Utilities::GetFileExtension(state.szInPath[0]));
        }

        if (state.bAvisynthInput == false)
        {
            if (pcm_init(&state.pf, state.nInputFiles, state.ifp, read_format, input_file_format))
            {
                return this->EncoderError(state, L"[Error] Failed to initialize PCM library.");
            }

            if (state.opt.read_to_eof)
                pcm_set_read_to_eof(&state.pf, 1);

            if (state.opt.raw_input)
            {
                pcm_set_source_params(&state.pf, state.opt.raw_ch, state.opt.raw_fmt, state.opt.raw_order, state.opt.raw_sr);
            }
        }

        if (state.bAvisynthInput == true)
        {
            state.statusAVS.nStart = 0;
            state.statusAVS.nSamples = state.infoAVS.nAudioSamples;
            state.statusAVS.nSamplesLeft = state.infoAVS.nAudioSamples;
            state.statusAVS.nSamplesToRead = A52_SAMPLES_PER_FRAME;
            state.pf.samples = state.infoAVS.nAudioSamples;
            state.pf.sample_rate = state.infoAVS.nSamplesPerSecond;
            state.pf.channels = state.infoAVS.nAudioChannels;
            state.pf.ch_mask = 0xFFFFFFFF;
        }

        if (state.s.acmod >= 0)
        {
            static const int acmod_to_ch[8] = { 2, 1, 2, 3, 3, 4, 4, 5 };
            int ch = acmod_to_ch[state.s.acmod];
            if (ch == state.pf.channels)
            {
                if (state.s.lfe < 0)
                {
                    state.s.lfe = 0;
                }
                else
                {
                    if (state.s.lfe != 0)
                    {
                        return this->EncoderError(state, L"[Error] Invalid channel configuration.");
                    }
                }
            }
            else if (ch == (state.pf.channels - 1))
            {
                if (state.s.lfe < 0)
                {
                    state.s.lfe = 1;
                }
                else
                {
                    if (state.s.lfe != 1)
                    {
                        return this->EncoderError(state, L"[Error] Invalid channel configuration.");
                    }
                }
            }
            else
            {
                return this->EncoderError(state, L"[Error] Invalid channel configuration.");
            }
        }
        else
        {
            int ch = state.pf.channels;
            if (state.s.lfe >= 0)
            {
                if (state.s.lfe == 0 && ch == 6)
                {
                    return this->EncoderError(state, L"[Error] Invalid channel configuration.");
                }
                else if (state.s.lfe == 1 && ch == 1)
                {
                    return this->EncoderError(state, L"[Error] Invalid channel configuration.");
                }

                if (state.s.lfe)
                {
                    state.pf.ch_mask |= 0x08;
                }
            }

            if (state.api.LibAften_aften_wav_channels_to_acmod(ch, state.pf.ch_mask, &state.s.acmod, &state.s.lfe))
            {
                return this->EncoderError(state, L"[Error] Invalid channel configuration.");
            }
        }

        if (state.bAvisynthInput == false)
        {
#ifdef CONFIG_DOUBLE
            state.s.sample_format = A52_SAMPLE_FMT_DBL;
#else
            state.s.sample_format = A52_SAMPLE_FMT_FLT;
#endif
            state.s.channels = state.pf.channels;
            state.s.samplerate = state.pf.sample_rate;
        }
        else
        {
            state.s.sample_format = A52_SAMPLE_FMT_FLT;
            state.s.channels = state.infoAVS.nAudioChannels;
            state.s.samplerate = state.infoAVS.nSamplesPerSecond;
        }
        state.frame = (uint8_t *)calloc(A52_MAX_CODED_FRAME_SIZE, 1);
        state.fwav = (FLOAT *)calloc(A52_SAMPLES_PER_FRAME * state.s.channels, sizeof(FLOAT));
        if (state.frame == nullptr || state.fwav == nullptr)
        {
            return this->EncoderError(state, L"[Error] Failed to allocate samples memory.");
        }

        samplecount = bytecount = t0 = t1 = percent = 0;
        qual = bw = 0.0;
        last_frame = 0;
        frame_cnt = 0;
        done = 0;
        fs = 0;
        nr = 0;

        if (state.opt.chmap == 0)
            aften_remap = state.api.LibAften_aften_remap_wav_to_a52;
        else if (state.opt.chmap == 2)
            aften_remap = state.api.LibAften_aften_remap_mpeg_to_a52;

        if (!state.opt.pad_start)
        {
            int diff;

            if (state.bAvisynthInput == false)
            {
                nr = pcm_read_samples(&state.pf, state.fwav, 256);
            }
            else
            {
                state.statusAVS.nSamplesToRead = 256;
                nr = state.decoderAVS.GetAudio(state.fwav, &state.statusAVS);
            }

            diff = 256 - nr;
            if (diff > 0)
            {
                memmove(state.fwav + diff * state.s.channels, state.fwav, nr);
                memset(state.fwav, 0, diff * state.s.channels * sizeof(FLOAT));
            }

            if (aften_remap)
                aften_remap(state.fwav + diff, nr, state.s.channels, state.s.sample_format, state.s.acmod);

            state.s.initial_samples = state.fwav;
        }

        if (state.api.LibAften_aften_encode_init(&state.s))
        {
            return this->EncoderError(state, L"[Error] Failed to initialize aften encoder.");
        }

        this->SetInfo(state);

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
                    fs = state.api.LibAften_aften_encode_frame(&state.s, state.frame, nullptr, 0);
                    if (fs > 0)
                        fwrite(state.frame, 1, fs, state.ofp);
                }
                return this->EncoderError(state, L"[Info] User terminated encoding.");
            }

            if (state.bAvisynthInput == false)
            {
                nr = pcm_read_samples(&state.pf, state.fwav, A52_SAMPLES_PER_FRAME);
            }
            else
            {
                state.statusAVS.nSamplesToRead = A52_SAMPLES_PER_FRAME;
                nr = state.decoderAVS.GetAudio(state.fwav, &state.statusAVS);
            }

            if (aften_remap)
                aften_remap(state.fwav, nr, state.s.channels, state.s.sample_format, state.s.acmod);

            fs = state.api.LibAften_aften_encode_frame(&state.s, state.frame, state.fwav, nr);

            if (fs < 0)
            {
                return this->EncoderError(state, L"[Error] Failed to encode frame.");
            }
            else
            {
                if (fs > 0)
                {
                    samplecount += A52_SAMPLES_PER_FRAME;
                    bytecount += fs;
                    qual += state.s.status.quality;
                    bw += state.s.status.bwcode;
                }

                t1 = samplecount / state.pf.sample_rate;
                if (frame_cnt > 0 && (t1 > t0 || samplecount >= state.pf.samples))
                {
                    kbps = (bytecount * FCONST(8.0) * state.pf.sample_rate) / (FCONST(1000.0) * samplecount);
                    percent = 0;
                    if (state.pf.samples > 0)
                    {
                        percent = (uint32_t)((samplecount * FCONST(100.0)) / state.pf.samples);
                        percent = CLIP(percent, 0, 100);
                    }

                    if (state.bAvisynthInput == false)
                    {
                        if (pContext->pConfig->bMultiMonoInput == false)
                        {
                            nCurPos = _ftelli64(state.ifp[0]);
                        }
                        else
                        {
                            for (int i = 0; i < state.nInputFiles; i++)
                            {
                                nCurPos += _ftelli64(state.ifp[i]);
                            }
                        }
                    }
                    else
                    {
                        nCurPos = samplecount * state.infoAVS.nBytesPerChannelSample * state.infoAVS.nAudioChannels;
                    }

                    if (pContext->bCanUpdateWindow == true)
                    {
                        pContext->bCanUpdateWindow = false;
                        nInPrevCurPos = nCurPos;
                        pContext->bCanUpdateWindow = true;
                    }

                    percent = (100 * nCurPos) / state.nInTotalSize;

                    if (pContext->bCanUpdateWindow == true)
                    {
                        pContext->bCanUpdateWindow = false;
                        pContext->SetCurrentProgress(percent);
                        pContext->bCanUpdateWindow = true;
                    }

                    nCurTotalPos = (100 * (pContext->nTotalSizeCounter + nCurPos)) / pContext->nTotalSize;

                    if (pContext->bCanUpdateWindow == true)
                    {
                        pContext->bCanUpdateWindow = false;
                        pContext->SetTotalProgress(nCurTotalPos);
                        pContext->bCanUpdateWindow = true;
                    }
                }
                t0 = t1;

                fwrite(state.frame, 1, fs, state.ofp);

                if (pContext->bCanUpdateWindow == true)
                {
                    pContext->bCanUpdateWindow = false;
                    nOutPrevCurPos = _ftelli64(state.ofp);
                    pContext->bCanUpdateWindow = true;
                }

                frame_cnt++;
                last_frame = nr;
            }
        } while (nr > 0 || fs > 0 || !frame_cnt);

        if (state.fwav)
            free(state.fwav);

        if (state.frame)
            free(state.frame);

        if (state.bAvisynthInput == false)
        {
            pcm_close(&state.pf);
        }
        else
        {
            state.decoderAVS.CloseAvisynth();
        }

        if (state.bAvisynthInput == false)
        {
            if (pContext->pConfig->bMultiMonoInput == false)
            {
                pContext->nTotalSizeCounter += _ftelli64(state.ifp[0]);
            }
            else
            {
                for (int i = 0; i < state.nInputFiles; i++)
                {
                    pContext->nTotalSizeCounter += _ftelli64(state.ifp[i]);
                }
            }
        }
        else
        {
            pContext->nTotalSizeCounter += state.nInTotalSize;
        }

        for (int i = 0; i < state.nInputFiles; i++)
        {
            if (state.ifp[i])
                fclose(state.ifp[i]);
        }

        if (state.ofp)
            fclose(state.ofp);

        state.api.LibAften_aften_encode_close(&state.s);

        pContext->StopCurrentTimer();
        pContext->SetCurrentTimerInfo(pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00"));
        pContext->m_ElapsedTimeFile = 0L;

        return true;
    }

    bool CWorker::Run()
    {
        pContext->SetCurrentProgressRange(0, 100);
        pContext->SetTotalProgressRange(0, 100);
        pContext->SetCurrentProgress(0);
        pContext->SetTotalProgress(0);
        pContext->StopCurrentTimer();
        pContext->m_ElapsedTimeTotal = 0L;
        pContext->SetTotalTimerInfo(pContext->pConfig->GetString(0x00A01006) + std::wstring(L" 00:00:00"));
        pContext->StartTotalTimer(250);
        pContext->nTotalSizeCounter = 0;

        int nFileCounter = 0;
        int nTotalFiles = (int)pContext->m_Files.size();

        if (pContext->pConfig->bMultiMonoInput == false)
        {
            for (int i = 0; i < (int)pContext->m_Files.size(); i++)
            {
                CState state;
                state.preset = pContext->pPreset;
                state.engine = pContext->pEngine;

                state.szInPath[0] = L"-";
                state.szInPath[1] = L"-";
                state.szInPath[2] = L"-";
                state.szInPath[3] = L"-";
                state.szInPath[4] = L"-";
                state.szInPath[5] = L"-";
                state.szOutPath = L"";

                state.szInPath[0] = pContext->m_Files[i];
                state.szOutPath = state.szInPath[0];

                std::wstring szExt = util::Utilities::GetFileExtension(state.szOutPath);
                state.szOutPath =
                    state.szOutPath.substr(0, state.szOutPath.length() - szExt.length() - 1) +
                    L"." + 
                    pContext->pConfig->m_EncoderOptions.szSupportedOutputExt[0];

                if (pContext->pConfig->bUseOutputPath == true)
                {
                    std::wstring szFile = util::Utilities::GetFileName(state.szOutPath);
                    state.szOutPath = util::Utilities::CombinePath(pContext->pConfig->szOutputPath, szFile);
                }

                CAtlString szTitle;
                szTitle.Format(pContext->pConfig->GetString(0x00A0100C).c_str(), nFileCounter + 1, nTotalFiles);
                std::wstring szTitleStr = szTitle;
                pContext->SetTitleInfo(szTitleStr);

                pContext->SetInputFileInfo(0, pContext->pConfig->GetString(0x00A01003) + L"\t" + state.szInPath[0]);
                pContext->SetOutputFileInfo(pContext->pConfig->GetString(0x00A01004) + L"\t" + state.szOutPath);
                pContext->SetCurrentTimerInfo(pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00"));
                pContext->m_ElapsedTimeFile = 0L;
                pContext->StartCurrentTimer(250);
                pContext->SetCurrentProgress(0);

                ZeroMemory(&state.s, sizeof(AftenContext));
                ZeroMemory(&state.opt, sizeof(AftenOpt));
                if (this->InitEngine(state) == false)
                {
                    return false;
                }

                state.nInputFiles = 1;

                if (this->Encode(state) == false)
                {
                    pContext->m_Status[i] = false;
                    state.api.CloseAftenAPI();
                    return false;
                }

                state.api.CloseAftenAPI();

                pContext->m_Status[i] = true;
                nFileCounter++;
                pContext->nCount = nFileCounter;

                if (pContext->bTerminate == true)
                {
                    this->pContext->pConfig->Log->Log(L"[Info] User terminated encoding.");
                    break;
                }
            }
        }
        else
        {
            CState state;
            state.preset = pContext->pPreset;
            state.engine = pContext->pEngine;

            state.szInPath[0] = L"-";
            state.szInPath[1] = L"-";
            state.szInPath[2] = L"-";
            state.szInPath[3] = L"-";
            state.szInPath[4] = L"-";
            state.szInPath[5] = L"-";
            state.szOutPath = L"";

            nFileCounter = (int)pContext->m_Files.size();

            for (int i = 0; i < nFileCounter; i++)
            {
                state.szInPath[i] = pContext->m_Files[i];
            }

            state.szOutPath = state.szInPath[0];

            std::wstring szExt = util::Utilities::GetFileExtension(state.szOutPath);
            state.szOutPath =
                state.szOutPath.substr(0, state.szOutPath.length() - szExt.length() - 1) +
                L"." + 
                pContext->pConfig->m_EncoderOptions.szSupportedOutputExt[0];

            if (pContext->pConfig->bUseOutputPath == true)
                state.szOutPath = pContext->pConfig->szOutputPath;

            CAtlString szTitle;
            szTitle.Format(pContext->pConfig->GetString(0x00A0100D).c_str(),
                nTotalFiles);
            std::wstring szTitleStr = szTitle;
            pContext->SetTitleInfo(szTitleStr);

            pContext->SetInputFileInfo(0, pContext->pConfig->GetString(0x00A01003) + L"\t" + state.szInPath[0]);

            for (int i = 1; i < nFileCounter; i++)
                pContext->SetInputFileInfo(i, L"\t" + state.szInPath[i]);

            pContext->SetOutputFileInfo(pContext->pConfig->GetString(0x00A01004) + L"\t" + state.szOutPath);
            pContext->SetCurrentTimerInfo(pContext->pConfig->GetString(0x00A01005) + std::wstring(L" 00:00:00"));

            pContext->m_ElapsedTimeFile = 0L;
            pContext->StartCurrentTimer(250);
            pContext->SetCurrentProgress(0);

            ZeroMemory(&state.s, sizeof(AftenContext));
            ZeroMemory(&state.opt, sizeof(AftenOpt));
            if (this->InitEngine(state) == false)
            {
                return false;
            }

            state.nInputFiles = nFileCounter;

            if (this->Encode(state) == false)
            {
                for (int i = 0; i < (int)pContext->m_Status.size(); i++)
                {
                    pContext->m_Status[i] = false;
                }
                pContext->nCount = 0;
                state.api.CloseAftenAPI();
                return false;
            }
            else
            {
                state.api.CloseAftenAPI();

                for (int i = 0; i < (int)pContext->m_Status.size(); i++)
                {
                    pContext->m_Status[i] = true;
                }
                pContext->nCount = nFileCounter;
            }
        }

        pContext->StopTotalTimer();

        return true;
    }
}
