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

#define DEFAULT_PRESET_NAME (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00207001) : _T("Default"))
#define DEFAULT_TEXT_AUTO (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00207002) : _T("<Auto>"))
#define DEFAULT_TEXT_IGNORED (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00207003) : _T("<Ignored>"))
#define DEFAULT_TEXT_OUTPUT_PATH (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00207004) : _T("<Same as input file path>"))
#define DEFAULT_TEXT_OUTPUT_FILE (theApp.m_Config.HaveLangStrings() ? theApp.m_Config.GetLangString(0x00207005) : _T("<Same as first input file path + output.ac3>"))

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

class CConfiguration
{
public:
    CConfiguration();
    virtual ~CConfiguration();
public:
    LangMap_t * m_Lang;
    LangList_t m_LangLst;
    CString m_szLangFileName = _T("");
    BOOL m_bHaveLang = FALSE;
    int m_nLangId = -1;
public:
    bool m_bIsPortable = true;
    CString m_szPresetsFilePath;
    CString m_szConfigFilePath;
    CString m_szEnginesFilePath;
    CString m_szFilesListFilePath;
    CString m_szLangFilePath;
public:
    bool LoadConfig(CString &szFileName, ConfigList_t &cl);
    bool SaveConfig(CString &szFileName, ConfigList_t &cl);
public:
    void SearchFolderForLang(CString szPath, const bool bRecurse, LangList_t& m_LangLst);
    void CleanLangList(LangList_t& m_LangLst);
    bool LoadLang(CString &szFileName, LangMap_t *lm);
public:
    bool LoadLangConfig(CString &szFileName);
    bool SaveLangConfig(CString &szFileName);
public:
    void LoadLangStrings();
    BOOL HaveLangStrings();
    CString& GetLangString(int id);
};
