#pragma once

#define ENCWAVTOAC3_VERSION _T(VER_FILE_VERSION_SHORT_STR)
#define ENCWAVTOAC3_URL_HOME _T("https://github.com/wieslawsoltes/wavtoac3encoder/")

#define DEFAULT_LANG_FILE_NAME _T("EncWAVtoAC3.lang")
#define DEFAULT_PORTABLE_FILE_NAME _T("EncWAVtoAC3.portable")
#define DEFAULT_CONFIG_FILE_NAME _T("EncWAVtoAC3.config")
#define DEFAULT_PRESETS_FILE_NAME _T("EncWAVtoAC3.presets")
#define DEFAULT_FILES_FILE_NAME _T("EncWAVtoAC3.files")
#define DEFAULT_FILES_FORMAT 0

#ifdef _M_X64
#define DEFAULT_ENGINES_FILE_NAME _T("EncWAVtoAC3-x64.engines")
#else
#define DEFAULT_ENGINES_FILE_NAME _T("EncWAVtoAC3-x86.engines")
#endif

#ifdef _M_X64
#define DEFAULT_CONFIG_DIRECTORY _T("EncWAVtoAC3-x64")
#else
#define DEFAULT_CONFIG_DIRECTORY _T("EncWAVtoAC3-x86")
#endif

#define DEFAULT_PRESET_NAME (theApp.HaveLangStrings() ? theApp.GetLangString(0x00207001) : _T("Default"))
#define DEFAULT_TEXT_AUTO (theApp.HaveLangStrings() ? theApp.GetLangString(0x00207002) : _T("<Auto>"))
#define DEFAULT_TEXT_IGNORED (theApp.HaveLangStrings() ? theApp.GetLangString(0x00207003) : _T("<Ignored>"))
#define DEFAULT_TEXT_OUTPUT_PATH (theApp.HaveLangStrings() ? theApp.GetLangString(0x00207004) : _T("<Same as input file path>"))
#define DEFAULT_TEXT_OUTPUT_FILE (theApp.HaveLangStrings() ? theApp.GetLangString(0x00207005) : _T("<Same as first input file path + output.ac3>"))

typedef struct TConfigEntry
{
    CString szKey;
    CString szValue;
} ConfigEntry, *PConfigEntry;

typedef CList<ConfigEntry, ConfigEntry&> ConfigList_t;

typedef CMap<int, int, CString, CString&> LangMap_t;

typedef struct TLang
{
    CString szFileName;
    CString szEnglishName;
    CString szTargetName;
    LangMap_t *lm;
} Lang;

typedef CList<Lang, Lang&> LangList_t;
