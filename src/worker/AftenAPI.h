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

const LPCSTR pszLibAften_aften_get_version = "aften_get_version";
const LPCSTR pszLibAften_aften_set_defaults = "aften_set_defaults";
const LPCSTR pszLibAften_aften_encode_init = "aften_encode_init";
const LPCSTR pszLibAften_aften_encode_frame = "aften_encode_frame";
const LPCSTR pszLibAften_aften_encode_close = "aften_encode_close";
const LPCSTR pszLibAften_aften_wav_channels_to_acmod = "aften_wav_channels_to_acmod";
const LPCSTR pszLibAften_aften_remap_wav_to_a52 = "aften_remap_wav_to_a52";
const LPCSTR pszLibAften_aften_remap_mpeg_to_a52 = "aften_remap_mpeg_to_a52";
const LPCSTR pszLibAften_aften_get_float_type = "aften_get_float_type";

class AftenAPI
{
public:
    AftenAPI() { }
    virtual ~AftenAPI() { }
public:
    CString szLibPath;
    HMODULE hLibAften;
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
        if (hLibAften == NULL)
            return false;

        LibAften_aften_get_version = (lpLibAften_aften_get_version)GetProcAddress(hLibAften, pszLibAften_aften_get_version);
        if (LibAften_aften_get_version == NULL)
            return false;

        LibAften_aften_set_defaults = (lpLibAften_aften_set_defaults)GetProcAddress(hLibAften, pszLibAften_aften_set_defaults);
        if (LibAften_aften_set_defaults == NULL)
            return false;

        LibAften_aften_encode_init = (lpLibAften_aften_encode_init)GetProcAddress(hLibAften, pszLibAften_aften_encode_init);
        if (LibAften_aften_encode_init == NULL)
            return false;

        LibAften_aften_encode_frame = (lpLibAften_aften_encode_frame)GetProcAddress(hLibAften, pszLibAften_aften_encode_frame);
        if (LibAften_aften_encode_frame == NULL)
            return false;

        LibAften_aften_encode_close = (lpLibAften_aften_encode_close)GetProcAddress(hLibAften, pszLibAften_aften_encode_close);
        if (LibAften_aften_encode_close == NULL)
            return false;

        LibAften_aften_wav_channels_to_acmod = (lpLibAften_aften_wav_channels_to_acmod)GetProcAddress(hLibAften, pszLibAften_aften_wav_channels_to_acmod);
        if (LibAften_aften_wav_channels_to_acmod == NULL)
            return false;

        LibAften_aften_remap_wav_to_a52 = (lpLibAften_aften_remap_wav_to_a52)GetProcAddress(hLibAften, pszLibAften_aften_remap_wav_to_a52);
        if (LibAften_aften_remap_wav_to_a52 == NULL)
            return false;

        LibAften_aften_remap_mpeg_to_a52 = (lpLibAften_aften_remap_mpeg_to_a52)GetProcAddress(hLibAften, pszLibAften_aften_remap_mpeg_to_a52);
        if (LibAften_aften_remap_mpeg_to_a52 == NULL)
            return false;

        LibAften_aften_get_float_type = (lpLibAften_aften_get_float_type)GetProcAddress(hLibAften, pszLibAften_aften_get_float_type);
        if (LibAften_aften_get_float_type == NULL)
            return false;

        return true;
    }
    void CloseAftenAPI()
    {
        if (hLibAften != NULL)
        {
            FreeLibrary(hLibAften);
            szLibPath = _T("");
            hLibAften = NULL;
            LibAften_aften_get_version = NULL;
            LibAften_aften_set_defaults = NULL;
            LibAften_aften_encode_init = NULL;
            LibAften_aften_encode_frame = NULL;
            LibAften_aften_encode_close = NULL;
            LibAften_aften_wav_channels_to_acmod = NULL;
            LibAften_aften_remap_wav_to_a52 = NULL;
            LibAften_aften_remap_mpeg_to_a52 = NULL;
            LibAften_aften_get_float_type = NULL;
        }
    }
};
