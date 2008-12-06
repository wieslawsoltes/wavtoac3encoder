@echo off
rem
rem WAV to AC3 Encoder
rem Copyright (C) 2007-2008 Wieslaw Soltes <wieslaw.soltes@gmail.com>
rem
rem This program is free software; you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation; version 2 of the License.
rem
rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program; if not, write to the Free Software
rem Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
rem
rem $Id$
setlocal

set varWinRAR=c:\Program Files (x86)\WinRAR\WinRAR.exe

set varVersion=2.3
set varOutput=EncWAVtoAC3-%varVersion%-src
set varBase=..

mkdir "%varOutput%"

copy "%varBase%\EncWAVtoAC3.sln" "%varOutput%"
copy "%varBase%\EncWAVtoAC3.vcproj" "%varOutput%"
copy "%varBase%\AftenBuild.cmd" "%varOutput%"
copy "%varBase%\AftenBuilds.inc" "%varOutput%"
copy "%varBase%\AftenClean.cmd" "%varOutput%"
copy "%varBase%\AftenPackages.cmd" "%varOutput%"
copy "%varBase%\AftenRelease.cmd" "%varOutput%"
copy "%varBase%\EncoderBuild.cmd" "%varOutput%"
copy "%varBase%\EncoderBuilds.inc" "%varOutput%"
copy "%varBase%\EncoderClean.cmd" "%varOutput%"
copy "%varBase%\EncoderPackages.cmd" "%varOutput%"
copy "%varBase%\EncoderRelease.cmd" "%varOutput%"
copy "%varBase%\EnginesClean.cmd" "%varOutput%"
copy "%varBase%\EnginesCopy.cmd" "%varOutput%"

set varPath=doc
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\Changes.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\Copying.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\ReadMe.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\Version.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\Command-Line.txt" "%varOutput%\%varPath%"

set varPath=engines
mkdir "%varOutput%\%varPath%"

set varPath=engines\ansi
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.win32" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.win64" "%varOutput%\%varPath%"

set varPath=engines\unicode
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.win32" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.win64" "%varOutput%\%varPath%"

set varPath=installer
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\PackageBinaries.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\PackageInstallers.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\PackageSources.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\SetupScript.base" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\SetupScript.version" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\SetupScript.Win32.iss" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\SetupScript.x64.iss" "%varOutput%\%varPath%"

set varPath=src
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\AboutDlg.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\AboutDlg.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\AftenAPI.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\AftenAPI.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncoderOptions.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncoderOptions.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.rc" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3Dlg.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3Dlg.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3MuxDlg.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3MuxDlg.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3WorkDlg.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3WorkDlg.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWorkThread.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWorkThread.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyLog.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyLog.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyButton.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyButton.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyComboBox.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyComboBox.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyComboBoxEdit.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyComboBoxEdit.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyEdit.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyEdit.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyFile.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyFile.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyListCtrl.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyListCtrl.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MySliderCtrl.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MySliderCtrl.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyStatic.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\MyStatic.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\ProgramConfig.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\ProgramConfig.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\ResizeDialog.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\ResizeDialog.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\resource.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\stdafx.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\stdafx.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\Utilities.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\Utilities.h" "%varOutput%\%varPath%"

set varPath=src\aften
mkdir "%varOutput%\%varPath%"

set varPath=src\aften\windows
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\benchmark_win32.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\benchmark_win64.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\build_intel.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\build_win32_debug.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\build_win32_release.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\build_win64_debug.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\build_win64_release.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\feedback.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\feedback.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\readme.txt" "%varOutput%\%varPath%"

set varPath=src\aften\windows\help
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\help_aften.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\help_long_aften.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\help_wavfilter.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\help_wavinfo.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\help_wavrms.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\readme.txt" "%varOutput%\%varPath%"

set varPath=src\aften\windows\output
mkdir "%varOutput%\%varPath%"

set varPath=src\res
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.bmp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.ico" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.rc2" "%varOutput%\%varPath%"

set varPath=UnicoWS
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\unicows.dll" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\unicows.lib" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\unicows.pdb" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\license.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\redist.txt" "%varOutput%\%varPath%"

set varPath=Debug

mkdir "%varOutput%\%varPath%"
mkdir "%varOutput%\%varPath%\Win32"
mkdir "%varOutput%\%varPath%\x64"

set varPath=Release

mkdir "%varOutput%\%varPath%"
mkdir "%varOutput%\%varPath%\Win32"
mkdir "%varOutput%\%varPath%\x64"

rem "%varWinRAR%" a -ep1 -m5 -r -s -t "%varOutput%.rar" "%varOutput%\*"
"%varWinRAR%" a -ep1 -m5 -r -t "%varOutput%.zip" "%varOutput%\*"
rmdir /S /Q "%varOutput%"

endlocal