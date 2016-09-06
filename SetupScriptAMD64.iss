;
; Setup release version
;
#define ReleaseVersion VERSION + "-AMD64"

;
; Include setup base srcript
;
#define MyAppBaseName "WAV to AC3 Encoder"
#define MyAppName MyAppBaseName + " (AMD64)"
#define MyAppVerName MyAppBaseName + " " + ReleaseVersion
#define MyAppPublisher "Wieslaw Soltes"
#define MyAppURL "https://github.com/wieslawsoltes/wavtoac3encoder"
#define MyAppExeName "EncWAVtoAC3.exe"

[Setup]

AppName={#MyAppName}
AppVerName={#MyAppVerName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=.\COPYING.TXT
OutputDir=.\artifacts
OutputBaseFilename=EncWAVtoAC3-{#ReleaseVersion}
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Languages]

Name: english; MessagesFile: compiler:Default.isl

[Tasks]

Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]

Source: src\bin\Release\x64\{#MyAppExeName}; DestDir: {app}; Flags: ignoreversion
Source: README.md; DestDir: {app}; Flags: ignoreversion
Source: COPYING.TXT; DestDir: {app}; Flags: ignoreversion
Source: CHANGELOG.md; DestDir: {app}; Flags: ignoreversion
Source: lang\*.txt; DestDir: {app}\Lang; Flags: ignoreversion
Source: engines\unicode\Win64\EncWAVtoAC3.engines; DestDir: {userappdata}\EncWAVtoAC3-AMD64; Flags: ignoreversion uninsneveruninstall
Source: src\aften\windows\output\libaftendll_AMD64\libaften.dll; DestDir: {app}\libaftendll_AMD64; Flags: ignoreversion
Source: src\aften\windows\output\libaftendll_AMD64_SSE2\libaften.dll; DestDir: {app}\libaftendll_AMD64_SSE2; Flags: ignoreversion
Source: src\aften\windows\output\libaftendll_AMD64_SSE3\libaften.dll; DestDir: {app}\libaftendll_AMD64_SSE3; Flags: ignoreversion

[Icons]

Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
Name: {group}\{#MyAppName} ReadMe; Filename: {app}\README.md
Name: {group}\{#MyAppName} License; Filename: {app}\COPYING.TXT
Name: {group}\{#MyAppName} Changes; Filename: {app}\CHANGELOG.md
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon

[Run]

Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: unchecked nowait postinstall skipifsilent
