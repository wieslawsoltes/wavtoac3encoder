/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file wavinfo.c
 * Console WAV File Info Utility
 */

#include "common.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "pcm.h"

static char *
get_format_name(int id)
{
    if (id < 0x0000 || id > 0xFFFF)
        return NULL;
    switch (id) {
        case 0x0000: return "Microsoft Unknown Wave Format";
        case 0x0001: return "Microsoft PCM";
        case 0x0002: return "Microsoft ADPCM";
        case 0x0003: return "IEEE Float";
        case 0x0004: return "Compaq Computer VSELP";
        case 0x0005: return "IBM CVSD";
        case 0x0006: return "Microsoft A-Law";
        case 0x0007: return "Microsoft mu-Law";
        case 0x0008: return "Microsoft DTS";
        case 0x0009: return "Microsoft DRM Encrypted Audio";
        case 0x000A: return "Windows Media Audio 9 Voice";
        case 0x000B: return "Windows Media RT Voice";

        case 0x0010: return "OKI ADPCM";
        case 0x0011: return "Intel DVI/IMA ADPCM";
        case 0x0012: return "Videologic MediaSpace ADPCM";
        case 0x0013: return "Sierra ADPCM";
        case 0x0014: return "Antex G.723 ADPCM";
        case 0x0015: return "DSP Solutions DigiSTD";
        case 0x0016: return "DSP Solutions DigiFIX";
        case 0x0017: return "Dialogic OKI ADPCM";
        case 0x0018: return "MediaVision ADPCM";
        case 0x0019: return "Hewlett-Packard CU";

        case 0x0020: return "Yamaha ADPCM";
        case 0x0021: return "Speech Compression Sonarc";
        case 0x0022: return "DSP Group TrueSpeech";
        case 0x0023: return "Echo Speech EchoSC1";
        case 0x0024: return "Audiofile AF36";
        case 0x0025: return "Audio Processing Technology APTX";
        case 0x0026: return "AudioFile AF10";
        case 0x0027: return "Prosody 1612";
        case 0x0028: return "LRC";

        case 0x0030: return "Dolby AC2";
        case 0x0031: return "Microsoft GSM 6.10";
        case 0x0032: return "MSNAudio";
        case 0x0033: return "Antex ADPCME";
        case 0x0034: return "Control Resources VQLPC";
        case 0x0035: return "DigiREAL";
        case 0x0036: return "DigiADPCM";
        case 0x0037: return "Control Resources CR10";
        case 0x0038: return "Natural MicroSystems VBXADPCM";
        case 0x0039: return "Crystal IMA ADPCM";
        case 0x003A: return "EchoSC3";
        case 0x003B: return "Rockwell ADPCM";
        case 0x003C: return "Rockwell Digit LK";
        case 0x003D: return "Xebec";

        case 0x0040: return "Antex Electronics G.721 ADPCM";
        case 0x0041: return "G.728 CELP";
        case 0x0042: return "MS G.723";
        case 0x0043: return "MS G.723.1";
        case 0x0044: return "MS G.729";
        case 0x0045: return "SP G.726";

        case 0x0050: return "MPEG Layer-2 or Layer-1";

        case 0x0052: return "RT24";
        case 0x0053: return "PAC";

        case 0x0055: return "MPEG Layer-3";

        case 0x0059: return "Lucent G.723";

        case 0x0060: return "Cirrus";
        case 0x0061: return "ESPCM";
        case 0x0062: return "Voxware";
        case 0x0063: return "Canopus Atrac";
        case 0x0064: return "G.726 ADPCM";
        case 0x0065: return "G.722 ADPCM";
        case 0x0066: return "DSAT";
        case 0x0067: return "DSAT Display";

        case 0x0069: return "Voxware Byte Aligned";

        case 0x0070: return "Voxware AC8";
        case 0x0071: return "Voxware AC10";
        case 0x0072: return "Voxware AC16";
        case 0x0073: return "Voxware AC20";
        case 0x0074: return "Voxware MetaVoice";
        case 0x0075: return "Voxware MetaSound";
        case 0x0076: return "Voxware RT29HW";
        case 0x0077: return "Voxware VR12";
        case 0x0078: return "Voxware VR18";
        case 0x0079: return "Voxware TQ40";

        case 0x0080: return "Softsound";
        case 0x0081: return "Voxware TQ60";
        case 0x0082: return "MSRT24";
        case 0x0083: return "G.729A";
        case 0x0084: return "MVI MV12";
        case 0x0085: return "DF G.726";
        case 0x0086: return "DF GSM610";

        case 0x0088: return "ISIAudio";
        case 0x0089: return "Onlive";

        case 0x0091: return "SBC24";
        case 0x0092: return "Dolby AC3 SPDIF";
        case 0x0093: return "MediaSonic G.723";
        case 0x0094: return "Aculab PLC  Prosody 8kbps";

        case 0x0097: return "ZyXEL ADPCM";
        case 0x0098: return "Philips LPCBB";
        case 0x0099: return "Packed";

        case 0x00FF: return "AAC";

        case 0x0100: return "Rhetorex ADPCM";
        case 0x0101: return "IBM mu-law";
        case 0x0102: return "IBM A-law";
        case 0x0103: return "IBM AVC ADPCM";

        case 0x0111: return "Vivo G.723";
        case 0x0112: return "Vivo Siren";

        case 0x0123: return "Digital G.723";

        case 0x0125: return "Sanyo LD ADPCM";

        case 0x0130: return "Sipro Lab Telecom ACELP NET / RealAudio 4.0/5.0)";
        case 0x0131: return "Sipro Lab Telecom ACELP 4800";
        case 0x0132: return "Sipro Lab Telecom ACELP 8V3";
        case 0x0133: return "Sipro Lab Telecom G.729";
        case 0x0134: return "Sipro Lab Telecom G.729A";
        case 0x0135: return "Sipro Lab Telecom Kelvin";

        case 0x0140: return "Windows Media Video V8";

        case 0x0150: return "Qualcomm PureVoice";
        case 0x0151: return "Qualcomm HalfRate";

        case 0x0155: return "Ring Zero Systems TUB GSM";

        case 0x0160: return "Microsoft Audio 1";
        case 0x0161: return "Windows Media 7/8/9";
        case 0x0162: return "Windows Media 9 Professional";
        case 0x0163: return "Windows Media 9 Lossless";
        case 0x0164: return "Windows Media Professional over S/PDIF";

        case 0x0180: return "MPEG-2 AAC";

        case 0x0190: return "DTS";

        case 0x0200: return "Creative Labs ADPCM";

        case 0x0202: return "Creative Labs FastSpeech8";
        case 0x0203: return "Creative Labs FastSpeech10";

        case 0x0210: return "UHER Informatic GmbH ADPCM";

        case 0x0215: return "Ulead DV Audio NTSC";
        case 0x0216: return "Ulead DV Audio PAL";

        case 0x0220: return "Quarterdeck";

        case 0x0230: return "I-link Worldwide VC";

        case 0x0240: return "Aureal RAW Sport";

        case 0x0250: return "Interactive Products HSX";
        case 0x0251: return "Interactive Products RPELP";

        case 0x0260: return "Consistent Software CS2";

        case 0x0270: return "Sony SCX / RealAudio 8.0";
        case 0x0271: return "Sony SCY";
        case 0x0272: return "Sony ATRAC3";
        case 0x0273: return "Sony SPC";

        case 0x0300: return "Fujitsu FM Towns Snd";

        case 0x0400: return "BTV Digital";
        case 0x0401: return "Intel Music Coder";

        case 0x0450: return "QDesign Music";

        case 0x0680: return "VME VMPCM";
        case 0x0681: return "AT&T Labs TPC";

        case 0x08AE: return "ClearJump LiteWave";

        case 0x1000: return "Olivetti GSM";
        case 0x1001: return "Olivetti ADPCM";
        case 0x1002: return "Olivetti CELP";
        case 0x1003: return "Olivetti SBC";
        case 0x1004: return "Olivetti OPR";

        case 0x1100: return "L&H Codec";
        case 0x1101: return "L&H CELP";
        case 0x1102: return "L&H SBC 0x1102";
        case 0x1103: return "L&H SBC 0x1103";
        case 0x1104: return "L&H SBC 0x1104";

        case 0x1400: return "Norris";
        case 0x1401: return "AT&T ISIAudio";

        case 0x1500: return "Soundspace Music Compression";

        case 0x181C: return "VoxWare RT24 Speech";

        case 0x1971: return "Sonic Foundry Perfect Clarity Audio (PCA)";

        case 0x1FC4: return "NCT Soft ALF2CD";

        case 0x2000: return "Dolby AC3";
        case 0x2001: return "Dolby DTS";
        case 0x2002: return "RealAudio 1.0 (14.4K)";
        case 0x2003: return "RealAudio 2.0 (28.8K)";
        case 0x2004: return "RealAudio G2 (Cook)";
        case 0x2005: return "RealAudio 3.0 (DolbyNet AC3)";
        case 0x2006: return "RealAudio 10.0 (LC-AAC)";
        case 0x2007: return "RealAudio 10.0 (HE-AAC)";

        case 0x2048: return "Sonic";

        case 0x4143: return "Divio AAC";

        case 0x4201: return "Nokia AMR";

        case 0x566F: return "Vorbis";

        case 0x5756: return "WavPack";

        case 0x674F: return "Ogg Vorbis 1";
        case 0x6750: return "Ogg Vorbis 2";
        case 0x6751: return "Ogg Vorbis 3";
        case 0x676F: return "Ogg Vorbis 1+";
        case 0x6770: return "Ogg Vorbis 2+";
        case 0x6771: return "Ogg Vorbis 3+";

        case 0x7A21: return "Adaptive Multirate";
        case 0x7A22: return "Adaptive Multirate w/ silence detection";

        case 0x706D: return "AAC";

        case 0x77A1: return "TTA";

        case 0xA106: return "MPEG-4 AAC";

        case 0xA109: return "Speex";

        case 0xF1AC: return "FLAC";

        case 0xFFFE: return "{ Extensible }";
        case 0xFFFF: return "{ Development }";

        default:     return NULL;
    }
}

typedef struct WavInfo {
    char *fname;
    PcmFile pf;
} WavInfo;

static void
wavinfo_print(WavInfo *wi)
{
    char *type;
    int64_t leftover;
    float playtime;
    PcmFile *wf = &wi->pf;

    type = get_format_name(wf->internal_fmt);

    printf("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    printf("File:\n");
    printf("   Name:          %s\n", wi->fname);
    if (wf->seekable) {
        printf("   File Size:     %"PRIu64"\n", wf->file_size);
    } else {
        printf("   File Size:     unknown\n");
    }
    printf("Format:\n");
    if (type == NULL) {
        printf("   Type:          unknown - 0x%04X\n", wf->internal_fmt);
    } else {
        printf("   Type:          %s\n", type);
    }
    printf("   Channels:      %d\n", wf->channels);
    printf("   Sample Rate:   %d Hz\n", wf->sample_rate);
    printf("   Avg bytes/sec: %d\n", wf->block_align * wf->sample_rate);
    printf("   Block Align:   %d bytes\n", wf->block_align);
    printf("   Bit Width:     %d\n", wf->bit_width);
    if (wf->ch_mask > 0) {
        printf("   Channel Mask:  0x%03"PRIX32"\n", wf->ch_mask);
    }
    printf("Data:\n");
    printf("   Start:         %"PRIu64"\n", wf->data_start);
    printf("   Data Size:     %"PRIu64"\n", wf->data_size);
    leftover = ((int64_t)(wf->file_size)) - wf->data_size - wf->data_start;
    if (leftover < 0) {
        if (!wf->seekable) {
            printf("   [ warning! unable to verify true data size ]\n");
        } else {
            printf("   [ warning! reported data size is larger than file size ]\n");
        }
    } else if (leftover > 0) {
        printf("   Leftover:  %"PRId64" bytes\n", leftover);
    }
    if (wf->internal_fmt == 0x0001 || wf->internal_fmt == 0x0003) {
        playtime = (float)wf->samples / (float)wf->sample_rate;
        printf("   Samples:       %"PRIu64"\n", wf->samples);
        printf("   Playing Time:  %0.2f sec\n", playtime);
    } else {
        printf("   Samples:       unknown\n");
        printf("   Playing Time:  unknown\n");
    }
    printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");
}

static void
wavfile_error(char *msg)
{
    if (msg != NULL) {
        fprintf(stderr, "%s\n", msg);
    }
    exit(1);
}

int
main(int argc, char **argv)
{
    FILE *fp;
    WavInfo wi;
    PcmFile *pf = &wi.pf;

    /* initialize WavInfo to zero */
    memset(&wi, 0, sizeof(WavInfo));

    /* open file */
    if (argc > 2) {
        wavfile_error("\nusage: wavinfo [test.wav]\n");
    }
    if (argc == 2) {
        wi.fname = argv[1];
        fp = fopen(wi.fname, "rb");
    } else {
        wi.fname = "[stdin]";
#ifdef _WIN32
        _setmode(_fileno(stdin), _O_BINARY);
#endif
        fp = stdin;
    }
    if (!fp)
        wavfile_error("cannot open file");

    pcmfile_init(pf, fp, PCM_SAMPLE_FMT_UNKNOWN, PCM_FORMAT_WAVE);

    /* print info */
    wavinfo_print(&wi);

    pcmfile_close(pf);
    fclose(fp);

    return 0;
}
