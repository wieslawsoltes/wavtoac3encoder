#include "StdAfx.h"
#include "MainApp.h"
#include "AftenAPI.h"

bool OpenAftenAPI(AftenAPI *pAftenApi)
{
    pAftenApi->hLibAften = LoadLibrary(pAftenApi->szLibPath);
    if (pAftenApi->hLibAften == NULL)
        return false;

    pAftenApi->LibAften_aften_get_version = (lpLibAften_aften_get_version)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_get_version);
    if (pAftenApi->LibAften_aften_get_version == NULL)
        return false;

    pAftenApi->LibAften_aften_set_defaults = (lpLibAften_aften_set_defaults)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_set_defaults);
    if (pAftenApi->LibAften_aften_set_defaults == NULL)
        return false;

    pAftenApi->LibAften_aften_encode_init = (lpLibAften_aften_encode_init)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_encode_init);
    if (pAftenApi->LibAften_aften_encode_init == NULL)
        return false;

    pAftenApi->LibAften_aften_encode_frame = (lpLibAften_aften_encode_frame)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_encode_frame);
    if (pAftenApi->LibAften_aften_encode_frame == NULL)
        return false;

    pAftenApi->LibAften_aften_encode_close = (lpLibAften_aften_encode_close)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_encode_close);
    if (pAftenApi->LibAften_aften_encode_close == NULL)
        return false;

    pAftenApi->LibAften_aften_wav_channels_to_acmod = (lpLibAften_aften_wav_channels_to_acmod)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_wav_channels_to_acmod);
    if (pAftenApi->LibAften_aften_wav_channels_to_acmod == NULL)
        return false;

    pAftenApi->LibAften_aften_remap_wav_to_a52 = (lpLibAften_aften_remap_wav_to_a52)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_remap_wav_to_a52);
    if (pAftenApi->LibAften_aften_remap_wav_to_a52 == NULL)
        return false;

    pAftenApi->LibAften_aften_remap_mpeg_to_a52 = (lpLibAften_aften_remap_mpeg_to_a52)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_remap_mpeg_to_a52);
    if (pAftenApi->LibAften_aften_remap_mpeg_to_a52 == NULL)
        return false;

    pAftenApi->LibAften_aften_get_float_type = (lpLibAften_aften_get_float_type)
        GetProcAddress(pAftenApi->hLibAften, pszLibAften_aften_get_float_type);
    if (pAftenApi->LibAften_aften_get_float_type == NULL)
        return false;

    return true;
}

void CloseAftenAPI(AftenAPI *pAftenApi)
{
    if (pAftenApi->hLibAften != NULL)
    {
        FreeLibrary(pAftenApi->hLibAften);

        pAftenApi->szLibPath = _T("");
        pAftenApi->hLibAften = NULL;

        pAftenApi->LibAften_aften_get_version = NULL;
        pAftenApi->LibAften_aften_set_defaults = NULL;
        pAftenApi->LibAften_aften_encode_init = NULL;
        pAftenApi->LibAften_aften_encode_frame = NULL;
        pAftenApi->LibAften_aften_encode_close = NULL;
        pAftenApi->LibAften_aften_wav_channels_to_acmod = NULL;
        pAftenApi->LibAften_aften_remap_wav_to_a52 = NULL;
        pAftenApi->LibAften_aften_remap_mpeg_to_a52 = NULL;
        pAftenApi->LibAften_aften_get_float_type = NULL;
    }
}
