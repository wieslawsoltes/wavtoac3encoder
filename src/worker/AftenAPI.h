#pragma once

#pragma warning(disable:4005)
#include "common.h"
#pragma warning(default:4005)
#include "libaften/aften.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "pcm.h"
#if defined(__cplusplus)
}
#endif

typedef const char *(*lpLibAften_aften_get_version)(void);
typedef void(*lpLibAften_aften_set_defaults)(AftenContext *s);
typedef int(*lpLibAften_aften_encode_init)(AftenContext *s);
typedef int(*lpLibAften_aften_encode_frame)(AftenContext *s, unsigned char *frame_buffer, const void *samples, int count);
typedef void(*lpLibAften_aften_encode_close)(AftenContext *s);
typedef int(*lpLibAften_aften_wav_channels_to_acmod)(int ch, unsigned int chmask, int *acmod, int *lfe);
typedef void(*lpLibAften_aften_remap_wav_to_a52)(void *samples, int n, int ch, A52SampleFormat fmt, int acmod);
typedef void(*lpLibAften_aften_remap_mpeg_to_a52)(void *samples, int n, int ch, A52SampleFormat fmt, int acmod);
typedef FloatType(*lpLibAften_aften_get_float_type)(void);

class AftenAPI
{
public:
    AftenAPI()
    {
    }
    virtual ~AftenAPI()
    {
        CloseAftenAPI();
    }
public:
    CString szLibPath;
    HMODULE hLibAften;
public:
    lpLibAften_aften_get_version LibAften_aften_get_version;
    lpLibAften_aften_set_defaults LibAften_aften_set_defaults;
    lpLibAften_aften_encode_init LibAften_aften_encode_init;
    lpLibAften_aften_encode_frame LibAften_aften_encode_frame;
    lpLibAften_aften_encode_close LibAften_aften_encode_close;
    lpLibAften_aften_wav_channels_to_acmod LibAften_aften_wav_channels_to_acmod;
    lpLibAften_aften_remap_wav_to_a52 LibAften_aften_remap_wav_to_a52;
    lpLibAften_aften_remap_mpeg_to_a52 LibAften_aften_remap_mpeg_to_a52;
    lpLibAften_aften_get_float_type LibAften_aften_get_float_type;
public:
    bool OpenAftenAPI()
    {
        hLibAften = LoadLibrary(szLibPath);
        if (hLibAften == nullptr)
            return false;

        LibAften_aften_get_version = (lpLibAften_aften_get_version)GetProcAddress(hLibAften, "aften_get_version");
        if (LibAften_aften_get_version == nullptr)
            return false;

        LibAften_aften_set_defaults = (lpLibAften_aften_set_defaults)GetProcAddress(hLibAften, "aften_set_defaults");
        if (LibAften_aften_set_defaults == nullptr)
            return false;

        LibAften_aften_encode_init = (lpLibAften_aften_encode_init)GetProcAddress(hLibAften, "aften_encode_init");
        if (LibAften_aften_encode_init == nullptr)
            return false;

        LibAften_aften_encode_frame = (lpLibAften_aften_encode_frame)GetProcAddress(hLibAften, "aften_encode_frame");
        if (LibAften_aften_encode_frame == nullptr)
            return false;

        LibAften_aften_encode_close = (lpLibAften_aften_encode_close)GetProcAddress(hLibAften, "aften_encode_close");
        if (LibAften_aften_encode_close == nullptr)
            return false;

        LibAften_aften_wav_channels_to_acmod = (lpLibAften_aften_wav_channels_to_acmod)GetProcAddress(hLibAften, "aften_wav_channels_to_acmod");
        if (LibAften_aften_wav_channels_to_acmod == nullptr)
            return false;

        LibAften_aften_remap_wav_to_a52 = (lpLibAften_aften_remap_wav_to_a52)GetProcAddress(hLibAften, "aften_remap_wav_to_a52");
        if (LibAften_aften_remap_wav_to_a52 == nullptr)
            return false;

        LibAften_aften_remap_mpeg_to_a52 = (lpLibAften_aften_remap_mpeg_to_a52)GetProcAddress(hLibAften, "aften_remap_mpeg_to_a52");
        if (LibAften_aften_remap_mpeg_to_a52 == nullptr)
            return false;

        LibAften_aften_get_float_type = (lpLibAften_aften_get_float_type)GetProcAddress(hLibAften, "aften_get_float_type");
        if (LibAften_aften_get_float_type == nullptr)
            return false;

        return true;
    }
    void CloseAftenAPI()
    {
        if (hLibAften == nullptr)
            return;

        FreeLibrary(hLibAften);
        hLibAften = nullptr;

        LibAften_aften_get_version = nullptr;
        LibAften_aften_set_defaults = nullptr;
        LibAften_aften_encode_init = nullptr;
        LibAften_aften_encode_frame = nullptr;
        LibAften_aften_encode_close = nullptr;
        LibAften_aften_wav_channels_to_acmod = nullptr;
        LibAften_aften_remap_wav_to_a52 = nullptr;
        LibAften_aften_remap_mpeg_to_a52 = nullptr;
        LibAften_aften_get_float_type = nullptr;
    }
    bool IsAftenOpen()
    {
        return hLibAften != nullptr;
    }
};

class AftenOpt
{
public:
    int pad_start;
    int chmap;
    int read_to_eof;
    int raw_input;
    int raw_sr;
    int raw_ch;
    enum PcmSampleFormat raw_fmt;
    int raw_order;
};
