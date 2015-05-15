;
; WAV to AC3 Encoder
; Copyright (C) 2007-2014 Wieslaw Soltes <wieslaw.soltes@gmail.com>
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; version 2 of the License.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;

;
; Setup release version
;
#define ReleaseVersion "5.4-AMD64"

;
; Include setup base srcript
;
#define MyAppBaseName "WAV to AC3 Encoder"
#define MyAppName MyAppBaseName + " (AMD64)"
#define MyAppVerName MyAppBaseName + " " + ReleaseVersion
#define MyAppPublisher "Wieslaw Soltes"
#define MyAppURL "http://code.google.com/p/wavtoac3encoder/"
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
LicenseFile=..\doc\Copying.txt
OutputDir=.
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

Source: ..\Release\x64\{#MyAppExeName}; DestDir: {app}; Flags: ignoreversion
Source: ..\ReadMe.md; DestDir: {app}; Flags: ignoreversion
Source: ..\doc\Keyboard.txt; DestDir: {app}; Flags: ignoreversion
Source: ..\doc\Copying.txt; DestDir: {app}; Flags: ignoreversion
Source: ..\doc\Changes.txt; DestDir: {app}; Flags: ignoreversion
Source: ..\doc\Command-Line.txt; DestDir: {app}; Flags: ignoreversion
Source: ..\lang\*.txt; DestDir: {app}\Lang; Flags: ignoreversion
Source: ..\engines\unicode\x64\EncWAVtoAC3.engines; DestDir: {userappdata}\EncWAVtoAC3-AMD64; Flags: ignoreversion uninsneveruninstall
Source: ..\src\aften\windows\output\libaftendll_AMD64\libaften.dll; DestDir: {app}\libaftendll_AMD64; Flags: ignoreversion
Source: ..\src\aften\windows\output\libaftendll_AMD64_SSE2\libaften.dll; DestDir: {app}\libaftendll_AMD64_SSE2; Flags: ignoreversion
Source: ..\src\aften\windows\output\libaftendll_AMD64_SSE3\libaften.dll; DestDir: {app}\libaftendll_AMD64_SSE3; Flags: ignoreversion

[Icons]

Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
Name: {group}\{#MyAppName} ReadMe; Filename: {app}\ReadMe.md
Name: {group}\{#MyAppName} Keyboard; Filename: {app}\Keyboard.txt
Name: {group}\{#MyAppName} License; Filename: {app}\Copying.txt
Name: {group}\{#MyAppName} Changes; Filename: {app}\Changes.txt
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon

[Run]

Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: unchecked nowait postinstall skipifsilent
