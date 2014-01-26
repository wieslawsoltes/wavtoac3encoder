/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2007 Justin Ruggles
 *               2007 Prakash Punnoor <prakash@punnoor.de>
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
 * @file opts.c
 * Commandline options
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "opts.h"
#include "pcm.h"

typedef struct PrivateOptions {
    int found_output;
    int single_input;
    int multi_input;
    int empty_params;
    int input_mask;
    int arg_index;
} PrivateOptions;

#define OPTION_FLAGS_NONE           0x0
#define OPTION_FLAG_NO_PARAM        0x1
#define OPTION_FLAG_MATCH_PARTIAL   0x2

struct OptionItem;

#define PARSE_PARAMS UNUSED(char *arg), UNUSED(char *param), \
                     UNUSED(const struct OptionItem *item), \
                     UNUSED(CommandOptions *opts), UNUSED(PrivateOptions *priv)

typedef struct OptionItem {
    const char *option_str;
    int flags;
    int min;
    int max;
    int (*parse)(PARSE_PARAMS);
    int offset;
} OptionItem;

static inline int
parse_integer_value(int val, int min, int max, char *name, int *out)
{
    if (val < min || val > max) {
        fprintf(stderr, "invalid parameter -%s %d. must be %d to %d.\n", name,
                val, min, max);
        return 1;
    }
    *out = val;
    return 0;
}

static int
parse_bool_or_int(const char *param, int *val)
{
    if (!strncmp(param, "no", 3) || !strncmp(param, "off", 4)) {
        *val = 0;
        return 0;
    } else if (!strncmp(param, "yes", 4) || !strncmp(param, "on", 3)) {
        *val = 1;
        return 0;
    } else {
        char *endp;
        int v = strtol(param, &endp, 0);
        if (*endp)
            return -1;
        *val = v;
        return 0;
    }
}

static int
parse_simple_int_s(PARSE_PARAMS)
{
    int v=0;
    if (parse_bool_or_int(param, &v)) {
        fprintf(stderr, "invalid integer or boolean value: %s\n", param);
        return 1;
    }
    return parse_integer_value(v, item->min, item->max, arg,
                               (int *)(((uint8_t *)opts->s) + item->offset));
}

static int
parse_simple_int_o(PARSE_PARAMS)
{
    int v=0;
    if (parse_bool_or_int(param, &v)) {
        fprintf(stderr, "invalid integer or boolean value: %s\n", param);
        return 1;
    }
    return parse_integer_value(v, item->min, item->max, arg,
                               (int *)(((uint8_t *)opts) + item->offset));
}

static int
parse_h(PARSE_PARAMS)
{
    return 2;
}

static int
parse_longhelp(PARSE_PARAMS)
{
    return 3;
}

static int
parse_version(PARSE_PARAMS)
{
    return 4;
}

static int
parse_nosimd(PARSE_PARAMS)
{
    int i, j;
    int last = 0;
    char *simd = param;
    AftenSimdInstructions *wanted_simd_instructions =
        &opts->s->system.wanted_simd_instructions;

    for (i = 0; simd[i]; i = j+1) {
        for (j = i; simd[j] != ','; ++j) {
            if (!simd[j]) {
                last = 1;
                break;
            }
        }
        simd[j] = 0;

        if (!strncmp(&simd[i], "mmx", 4))
            wanted_simd_instructions->mmx = 0;
        else if (!strncmp(&simd[i], "sse", 4))
            wanted_simd_instructions->sse = 0;
        else if (!strncmp(&simd[i], "sse2", 5))
            wanted_simd_instructions->sse2 = 0;
        else if (!strncmp(&simd[i], "sse3", 5))
            wanted_simd_instructions->sse3 = 0;
        else if (!strncmp(&simd[i], "altivec", 8))
            wanted_simd_instructions->altivec = 0;
        else {
            fprintf(stderr, "invalid simd instruction set: %s. must be mmx, sse, sse2, sse3 or altivec.\n", &simd[i]);
            return 1;
        }
        if (last)
            break;
    }
    return 0;
}

static int
parse_q(PARSE_PARAMS)
{
    opts->s->params.encoding_mode = AFTEN_ENC_MODE_VBR;
    return parse_integer_value(atoi(param), item->min, item->max, arg,
                               &opts->s->params.quality);
}

static int
parse_chconfig(PARSE_PARAMS)
{
    if (!strncmp(param, "1+1", 4)) {
        opts->s->acmod = 0;
    } else if (strlen(param) >= 3 && param[1] == '/') {
        int front_ch = param[0] - '0';
        int rear_ch = param[2] - '0';
        if (front_ch > 3 || front_ch < 1 || rear_ch < 0 || rear_ch > 2 || (front_ch < 2 && rear_ch != 0))
            goto invalid_config;
        opts->s->acmod = front_ch + 2 * rear_ch;
    } else {
    invalid_config:
        fprintf(stderr, "invalid chconfig: %s\n", param);
        return 1;
    }
    if (!strncmp(&param[3], "+LFE", 5) || !strncmp(&param[3], "+lfe", 5)) {
        opts->s->lfe = 1;
    }
    return 0;
}

/**
 * Channel bitmask values
 * Note that the bitmask used here is not the same as the WAVE channel mask.
 * They are arranged so that any valid channel combination will be in proper
 * AC3 channel order.
 */
#define CHMASK_FL  0x1
#define CHMASK_FC  0x2
#define CHMASK_FR  0x4
#define CHMASK_SL  0x8
#define CHMASK_S   0x10
#define CHMASK_SR  0x20
#define CHMASK_M1  0x40
#define CHMASK_M2  0x80
#define CHMASK_LFE 0x100

static int
validate_input_files(char **infile, int input_mask, int *acmod, int *lfe) {
    int i, j, l;

    // check for valid configurations
    l = !!(input_mask & CHMASK_LFE);
    input_mask &= 0xFF;
    switch (input_mask) {
        case (CHMASK_M1 | CHMASK_M2):
            *acmod = 0;
            break;
        case (CHMASK_FC):
            *acmod = 1;
            break;
        case (CHMASK_FL | CHMASK_FR):
            *acmod = 2;
            break;
        case (CHMASK_FL | CHMASK_FC | CHMASK_FR):
            *acmod = 3;
            break;
        case (CHMASK_FL | CHMASK_FR | CHMASK_S):
            *acmod = 4;
            break;
        case (CHMASK_FL | CHMASK_FC | CHMASK_FR | CHMASK_S):
            *acmod = 5;
            break;
        case (CHMASK_FL | CHMASK_FR | CHMASK_SL | CHMASK_SR):
            *acmod = 6;
            break;
        case (CHMASK_FL | CHMASK_FC | CHMASK_FR | CHMASK_SL | CHMASK_SR):
            *acmod = 7;
            break;
        default:
            return -1;
    }
    *lfe = l;

    // shift used channels, filling NULL gaps in infile array
    for (i = 1; i < A52_NUM_SPEAKERS; i++) {
        for (j = i; j > 0 && infile[j] != NULL && infile[j-1] == NULL; j--) {
            infile[j-1] = infile[j];
            infile[j] = NULL;
        }
    }

    return 0;
}

static int
parse_ch(PARSE_PARAMS)
{
    static const char chstrs[A52_NUM_SPEAKERS][4] = {
        "fl", "fc", "fr", "sl", "s", "sr", "m1", "m2", "lfe" };
    char *chstr = &arg[3];
    int n_chstr, mask_bit;

    if (priv->single_input) {
        fprintf(stderr, "cannot mix single-input syntax and multi-input syntax\n");
        return 1;
    }

    mask_bit = 1;
    for (n_chstr=0; n_chstr<A52_NUM_SPEAKERS; ++n_chstr) {
        if (!strncmp(chstr, chstrs[n_chstr], strlen(chstrs[n_chstr])+1)) {
            if (priv->input_mask & mask_bit) {
                fprintf(stderr, "invalid input channel parameter\n");
                return 1;
            }
            priv->input_mask |= mask_bit;
            opts->infile[n_chstr] = param;
        }
        mask_bit += mask_bit;
    }

    priv->multi_input = 1;
    opts->num_input_files++;
    return 0;
}

static int
parse_raw_fmt(PARSE_PARAMS)
{
    static const char *raw_fmt_strs[8] = {
        "u8", "s8", "s16_", "s20_", "s24_", "s32_", "float_",
        "double_"
    };
    enum PcmSampleFormat j;

    // parse format
    for (j = PCM_SAMPLE_FMT_U8; j <= PCM_SAMPLE_FMT_DBL; j++) {
        if (!strncmp(param, raw_fmt_strs[j], strlen(raw_fmt_strs[j]))) {
            opts->raw_fmt = j;
            break;
        }
    }
    if (j > PCM_SAMPLE_FMT_DBL) {
        fprintf(stderr, "invalid raw_fmt: %s\n", param);
        return 1;
    }

    // parse byte order
    opts->raw_order = PCM_BYTE_ORDER_LE;
    if (strncmp(param, "u8", 3) && strncmp(param, "s8", 3)) {
        if (!strncmp(&param[4], "be", 3)) {
            opts->raw_order = PCM_BYTE_ORDER_BE;
        } else if (strncmp(&param[4], "le", 3)) {
            fprintf(stderr, "invalid raw_fmt: %s\n", param);
            return 1;
        }
    }

    opts->raw_input = 1;
    return 0;
}

static int
parse_raw_option(PARSE_PARAMS)
{
    opts->raw_input = 1;
    return parse_simple_int_o(arg, param, item, opts, priv);
}

static int
parse_xbsi1_opt(PARSE_PARAMS)
{
    opts->s->meta.xbsi1e = 1;
    return parse_simple_int_s(arg, param, item, opts, priv);
}

static int
parse_xbsi2_opt(PARSE_PARAMS)
{
    opts->s->meta.xbsi2e = 1;
    return parse_simple_int_s(arg, param, item, opts, priv);
}

#define OPTION_ITEM_COUNT 43

/**
 * list of commandline options, in alphabetical order.
 */
static const OptionItem options_list[OPTION_ITEM_COUNT] = {
//     NAME         FLAGS                         MIN             MAX   PARSE FUNCTION      OFFSET
//    ------       -------                       -----           ----- ----------------    --------
    { "acmod",      OPTION_FLAGS_NONE,              0,              7,  parse_simple_int_s, offsetof(AftenContext, acmod)                       },
    { "adconvtyp",  OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, meta.adconvtyp)              },
    { "b",          OPTION_FLAGS_NONE,              0,            640,  parse_simple_int_s, offsetof(AftenContext, params.bitrate)              },
    { "bwfilter",   OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, params.use_bw_filter)        },
    { "ch_",        OPTION_FLAG_MATCH_PARTIAL,      0,              0,  parse_ch,           0                                                   },
    { "chconfig",   OPTION_FLAGS_NONE,              0,              0,  parse_chconfig,     0                                                   },
    { "chmap",      OPTION_FLAGS_NONE,              0,              2,  parse_simple_int_o, offsetof(CommandOptions, chmap)                     },
    { "cmix",       OPTION_FLAGS_NONE,              0,              2,  parse_simple_int_s, offsetof(AftenContext, meta.cmixlev)                },
    { "dcfilter",   OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, params.use_dc_filter)        },
    { "dheadphon",  OPTION_FLAGS_NONE,              0,              2,  parse_xbsi2_opt,    offsetof(AftenContext, meta.dheadphonmod)           },
    { "dmixmod",    OPTION_FLAGS_NONE,              0,              2,  parse_xbsi1_opt,    offsetof(AftenContext, meta.dmixmod)                },
    { "dnorm",      OPTION_FLAGS_NONE,              0,             31,  parse_simple_int_s, offsetof(AftenContext, meta.dialnorm)               },
    { "dsur",       OPTION_FLAGS_NONE,              0,              2,  parse_simple_int_s, offsetof(AftenContext, meta.dsurmod)                },
    { "dsurexmod",  OPTION_FLAGS_NONE,              0,              2,  parse_xbsi2_opt,    offsetof(AftenContext, meta.dsurexmod)              },
    { "dynrng",     OPTION_FLAGS_NONE,              0,              5,  parse_simple_int_s, offsetof(AftenContext, params.dynrng_profile)       },
    { "exps",       OPTION_FLAGS_NONE,              1,             32,  parse_simple_int_s, offsetof(AftenContext, params.expstr_search)        },
    { "fba",        OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, params.bitalloc_fast)        },
    { "h",          OPTION_FLAG_NO_PARAM,           0,              0,  parse_h,            0                                                   },
    { "lfe",        OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, lfe)                         },
    { "lfefilter",  OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, params.use_lfe_filter)       },
    { "longhelp",   OPTION_FLAG_NO_PARAM,           0,              0,  parse_longhelp,     0                                                   },
    { "lorocmix",   OPTION_FLAGS_NONE,              0,              7,  parse_xbsi1_opt,    offsetof(AftenContext, meta.lorocmixlev)            },
    { "lorosmix",   OPTION_FLAGS_NONE,              0,              7,  parse_xbsi1_opt,    offsetof(AftenContext, meta.lorosmixlev)            },
    { "ltrtcmix",   OPTION_FLAGS_NONE,              0,              7,  parse_xbsi1_opt,    offsetof(AftenContext, meta.ltrtcmixlev)            },
    { "ltrtsmix",   OPTION_FLAGS_NONE,              0,              7,  parse_xbsi1_opt,    offsetof(AftenContext, meta.ltrtsmixlev)            },
    { "m",          OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, params.use_rematrixing)      },
    { "nosimd",     OPTION_FLAGS_NONE,              0,              0,  parse_nosimd,       0                                                   },
    { "pad",        OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_o, offsetof(CommandOptions, pad_start)                 },
    { "q",          OPTION_FLAGS_NONE,              0,           1023,  parse_q,            0                                                   },
    { "raw_ch",     OPTION_FLAGS_NONE,              1,              6,  parse_raw_option,   offsetof(CommandOptions, raw_ch)                    },
    { "raw_fmt",    OPTION_FLAGS_NONE,              0,              0,  parse_raw_fmt,      0                                                   },
    { "raw_sr",     OPTION_FLAGS_NONE,              1,          48000,  parse_raw_option,   offsetof(CommandOptions, raw_sr)                    },
    { "readtoeof",  OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_o, offsetof(CommandOptions, read_to_eof)               },
    { "s",          OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, params.use_block_switching)  },
    { "smix",       OPTION_FLAGS_NONE,              0,              2,  parse_simple_int_s, offsetof(AftenContext, meta.surmixlev)              },
    { "threads",    OPTION_FLAGS_NONE,              0,MAX_NUM_THREADS,  parse_simple_int_s, offsetof(AftenContext, system.n_threads)            },
    { "v",          OPTION_FLAGS_NONE,              0,              2,  parse_simple_int_s, offsetof(AftenContext, verbose)                     },
    { "version",    OPTION_FLAG_NO_PARAM,           0,              0,  parse_version,      0                                                   },
    { "w",          OPTION_FLAGS_NONE,             -2,             60,  parse_simple_int_s, offsetof(AftenContext, params.bwcode)               },
    { "wmax",       OPTION_FLAGS_NONE,              0,             60,  parse_simple_int_s, offsetof(AftenContext, params.max_bwcode)           },
    { "wmin",       OPTION_FLAGS_NONE,              0,             60,  parse_simple_int_s, offsetof(AftenContext, params.min_bwcode)           },
    { "xbsi1",      OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, meta.xbsi1e)                 },
    { "xbsi2",      OPTION_FLAGS_NONE,              0,              1,  parse_simple_int_s, offsetof(AftenContext, meta.xbsi2e)                 },
};

int
parse_commandline(int argc, char **argv, CommandOptions *opts)
{
    int i;
    PrivateOptions priv;

    if (argc < 2) {
        return 1;
    }

    memset(&priv, 0, sizeof(PrivateOptions));
    opts->chmap = 0;
    opts->num_input_files = 0;
    memset(opts->infile, 0, A52_NUM_SPEAKERS * sizeof(char *));
    opts->outfile = NULL;
    opts->pad_start = 1;
    opts->read_to_eof = 0;
    opts->raw_input = 0;
    opts->raw_fmt = PCM_SAMPLE_FMT_S16;
    opts->raw_order = PCM_BYTE_ORDER_LE;
    opts->raw_sr = 48000;
    opts->raw_ch = 1;

    for (priv.arg_index = 1; priv.arg_index < argc; priv.arg_index++) {
        if (argv[priv.arg_index][0] == '-' && argv[priv.arg_index][1] != '\0') {
            int j;
            for (j = 0; j < OPTION_ITEM_COUNT; j++) {
                char *arg = &argv[priv.arg_index][1];
                const OptionItem *item = &options_list[j];
                int mc = strlen(item->option_str) + 1;
                if (item->flags & OPTION_FLAG_MATCH_PARTIAL)
                    mc--;
                if (!strncmp(arg, item->option_str, mc)) {
                    int ret;
                    char *param = NULL;
                    if (!(item->flags & OPTION_FLAG_NO_PARAM)) {
                        priv.arg_index++;
                        if (priv.arg_index >= argc) return 1;
                        param = argv[priv.arg_index];
                    }
                    ret = item->parse(arg, param, item, opts, &priv);
                    if (ret) return ret;
                    break;
                }
            }
            if (j >= OPTION_ITEM_COUNT) {
                fprintf(stderr, "invalid option: %s\n", argv[priv.arg_index]);
                return 1;
            }
        } else {
            // empty parameter can be single input file or output file
            if (priv.arg_index >= argc) return 1;
            priv.empty_params++;
            if (priv.empty_params == 1) {
                opts->outfile = argv[priv.arg_index];
                priv.found_output = 1;
            } else if (priv.empty_params == 2) {
                if (priv.multi_input) {
                    fprintf(stderr, "cannot mix single-input syntax and multi-input syntax\n");
                    return 1;
                }
                priv.single_input = 1;
                opts->infile[0] = opts->outfile;
                opts->outfile = argv[priv.arg_index];
                opts->num_input_files = 1;
            } else {
                fprintf(stderr, "too many empty parameters\n");
                return 1;
            }
        }
    }

    // check that proper input file syntax is used
    if (!(priv.single_input ^ priv.multi_input)) {
        fprintf(stderr, "cannot mix single-input syntax and multi-input syntax\n");
        return 1;
    }

    // check that the number of input files is valid
    if (opts->num_input_files < 1 || opts->num_input_files > 6) {
        fprintf(stderr, "invalid number of input channels. must be 1 to 6.\n");
        return 1;
    }

    // check that an output file has been specified
    if (!priv.found_output) {
        fprintf(stderr, "no output file specified.\n");
        return 1;
    }

    // check the channel configuration
    if (priv.multi_input) {
        if (validate_input_files(opts->infile, priv.input_mask, &opts->s->acmod, &opts->s->lfe)) {
            fprintf(stderr, "invalid input channel configuration\n");
            return 1;
        }
        opts->chmap = 1;
    }

    // disallow piped input with multiple files
    if (opts->num_input_files > 1) {
        for (i = 0; i < opts->num_input_files; i++) {
            if (!strncmp(opts->infile[i], "-", 2)) {
                fprintf(stderr, "cannot use piped input with multiple files\n");
                return 1;
            }
        }
    }

    // disallow infile & outfile with same name except with piping
    for (i = 0; i < opts->num_input_files; i++) {
        if (strncmp(opts->infile[i], "-", 2) && strncmp(opts->outfile, "-", 2)) {
            if (!strcmp(opts->infile[i], opts->outfile)) {
                fprintf(stderr, "output filename cannot match any input filename\n");
                return 1;
            }
        }
    }

    return 0;
}
