#define ProgramName "EncWAVtoAC3"
#define ProgramExe "EncWAVtoAC3.exe"
#define ProgramConfiguration CONFIGURATION
#define ProgramBuild BUILD
#define ProgramVersion VERSION
#define ProgramVersionSuffix SUFFIX
#define ProgramCopyright "Copyright (c) Wieslaw Soltes. All rights reserved."
#define ProgramWebsite "https://github.com/wieslawsoltes/wavtoac3encoder"
#define BasePath ".."

[Setup]

AppId                 ={#ProgramName}-{#ProgramBuild}
AppName               ={#ProgramName}
AppVerName            ={#ProgramName} {#ProgramVersion}{#ProgramVersionSuffix}
AppCopyright          ={#ProgramCopyright}
DefaultDirName        ={pf}\{#ProgramName} ({#ProgramBuild})
OutputDir             ={#BasePath}\artifacts
VersionInfoVersion    ={#ProgramVersion}
DefaultGroupName      ={#ProgramName} ({#ProgramBuild})
AppPublisher          ={#ProgramWebsite}
AppPublisherURL       ={#ProgramWebsite}
AppVersion            ={#ProgramVersion}{#ProgramVersionSuffix}
UsePreviousAppDir     =yes
AllowNoIcons          =yes
Compression           =lzma/max
SolidCompression      =yes
WindowVisible         =no
WindowShowCaption     =no
BackColor             =clBlue
BackColor2            =clBlack
BackColorDirection    =toptobottom
BackSolid             =no
UserInfoPage          =no
Uninstallable         =not IsTaskSelected('portablemode')
CreateUninstallRegKey =not IsTaskSelected('portablemode')

#if "Release" == ProgramConfiguration
OutputBaseFilename ={#ProgramName}-{#ProgramVersion}{#ProgramVersionSuffix}-{#ProgramBuild}
#else
OutputBaseFilename ={#ProgramName}-{#ProgramVersion}{#ProgramVersionSuffix}-{#ProgramBuild}-({#ProgramConfiguration})
#endif

#if "x64" == ProgramBuild
ArchitecturesInstallIn64BitMode=x64
#endif

[Components]

Name: main; Description: Main Program; Types: full compact custom
Name: docs; Description: Documents; Types: full
Name: presets; Description: Presets; Types: full compact
Name: libaften; Description: libaften.dll; Types: full compact
Name: lang; Description: Languages; Types: full

[Tasks]

Name: portablemode; Description: "Portable Mode"; Components: main; Flags: unchecked
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Components: main; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Components: main; Flags: unchecked

[Files]

Source: {#BasePath}\src\bin\{#ProgramConfiguration}\{#ProgramBuild}\{#ProgramExe}; DestDir: {app}; Flags: ignoreversion; Components: main
Source: {#BasePath}\*.txt; DestDir: {app}; Flags: ignoreversion; Components: docs
Source: {#BasePath}\*.md; DestDir: {app}; Flags: ignoreversion; Components: docs
Source: {#BasePath}\config\EncWAVtoAC3.portable; DestDir: {app}; Flags: ignoreversion; Tasks: portablemode; Components: main

#if "x64" == ProgramBuild

Source: {#BasePath}\config\EncWAVtoAC3.presets; DestDir: {userappdata}\{#ProgramName}-x64; Flags: ignoreversion uninsneveruninstall; Tasks: not portablemode; Components: presets
Source: {#BasePath}\config\lang\*.txt; DestDir: {userappdata}\{#ProgramName}-x64\lang; Flags: ignoreversion; Tasks: not portablemode; Components: lang

Source: {#BasePath}\config\EncWAVtoAC3.presets; DestDir: {app}; Flags: ignoreversion uninsneveruninstall; Tasks: portablemode; Components: presets
Source: {#BasePath}\config\lang\*.txt; DestDir: {app}\lang; Flags: ignoreversion; Tasks: portablemode; Components: lang

Source: {#BasePath}\src\aften\windows\output\libaftendll_AMD64\libaften.dll; DestDir: {app}\libaftendll_AMD64; Flags: ignoreversion; Components: libaften
Source: {#BasePath}\src\aften\windows\output\libaftendll_AMD64_SSE2\libaften.dll; DestDir: {app}\libaftendll_AMD64_SSE2; Flags: ignoreversion; Components: libaften
Source: {#BasePath}\src\aften\windows\output\libaftendll_AMD64_SSE3\libaften.dll; DestDir: {app}\libaftendll_AMD64_SSE3; Flags: ignoreversion; Components: libaften

#else

Source: {#BasePath}\config\EncWAVtoAC3.presets; DestDir: {userappdata}\{#ProgramName}-x86; Flags: ignoreversion uninsneveruninstall; Tasks: not portablemode; Components: presets
Source: {#BasePath}\config\lang\*.txt; DestDir: {userappdata}\{#ProgramName}-x86\lang; Flags: ignoreversion; Tasks: not portablemode; Components: lang

Source: {#BasePath}\config\EncWAVtoAC3.presets; DestDir: {app}; Flags: ignoreversion uninsneveruninstall; Tasks: portablemode; Components: presets
Source: {#BasePath}\config\lang\*.txt; DestDir: {app}\lang; Flags: ignoreversion; Tasks: portablemode; Components: lang

Source: {#BasePath}\src\aften\windows\output\libaftendll_x86\libaften.dll; DestDir: {app}\libaftendll_x86; Flags: ignoreversion; Components: libaften
Source: {#BasePath}\src\aften\windows\output\libaftendll_x86_SSE\libaften.dll; DestDir: {app}\libaftendll_x86_SSE; Flags: ignoreversion; Components: libaften
Source: {#BasePath}\src\aften\windows\output\libaftendll_x86_SSE2\libaften.dll; DestDir: {app}\libaftendll_x86_SSE2; Flags: ignoreversion; Components: libaften
Source: {#BasePath}\src\aften\windows\output\libaftendll_x86_SSE3\libaften.dll; DestDir: {app}\libaftendll_x86_SSE3; Flags: ignoreversion; Components: libaften

#endif

[INI]

Filename: {app}\{#ProgramName}.url; Section: InternetShortcut; Key: URL; String: {#ProgramWebsite}; Tasks: not portablemode

[Icons]

Name: {group}\{#ProgramName}; Filename: {app}\{#ProgramExe}; WorkingDir: {app}; Tasks: not portablemode; Components: main
Name: {group}\{#ProgramName} README; Filename: {app}\README.MD; WorkingDir: {app}; Tasks: not portablemode; Components: docs
Name: {group}\{#ProgramName} License; Filename: {app}\COPYING.TXT; WorkingDir: {app}; Tasks: not portablemode; Components: docs
Name: {group}\{#ProgramName} Web Site; Filename: {app}\{#ProgramName}.url; WorkingDir: {app}; Tasks: not portablemode; Components: main
Name: {group}\Uninstall {#ProgramName}; Filename: {uninstallexe}; WorkingDir: {app}; Tasks: not portablemode; Components: main
Name: {userdesktop}\{#ProgramName}; Filename: {app}\{#ProgramExe}; WorkingDir: {app}; Tasks: desktopicon; Components: main
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#ProgramName}; Filename: {app}\{#ProgramExe}; WorkingDir: {app}; Tasks: quicklaunchicon; Components: main

[Run]

Filename: {app}\{#ProgramExe}; Description: {cm:LaunchProgram,{#ProgramName}}; WorkingDir: {app}; Flags: nowait postinstall skipifsilent unchecked; Components: main

[UninstallDelete]

Type: files; Name: {app}\{#ProgramName}.url; Tasks: not portablemode
