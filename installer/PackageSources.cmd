@echo off
rem
rem WAV to AC3 Encoder
rem Copyright (C) 2007, 2008, 2009 Wieslaw Soltes <wieslaw.soltes@gmail.com>
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

set varVersion=3.0
set varOutput=EncWAVtoAC3-%varVersion%-src
set varBase=..

mkdir "%varOutput%"

copy "%varBase%\*.sln" "%varOutput%"
copy "%varBase%\*.vcproj" "%varOutput%"
copy "%varBase%\*.cmd" "%varOutput%"
copy "%varBase%\*.inc" "%varOutput%"


set varPath=doc
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

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

copy "%varBase%\%varPath%\*.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.base" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.version" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.Win32.iss" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.x64.iss" "%varOutput%\%varPath%"

set varPath=src
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.rc" "%varOutput%\%varPath%"

set varPath=src\aften
mkdir "%varOutput%\%varPath%"

set varPath=src\aften\windows
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

set varPath=src\aften\windows\help
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

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