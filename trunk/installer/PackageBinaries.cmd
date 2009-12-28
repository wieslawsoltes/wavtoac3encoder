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

set varWinRAR=c:\Program Files\WinRAR\WinRAR.exe

set varVersion=4.3
set varOutputVer=EncWAVtoAC3-%varVersion%
set varBase=..
set varPathAftenBin=src\aften\windows\output

set varOutput=%varOutputVer%
mkdir "%varOutput%"
mkdir "%varOutput%\libaftendll_x86"
mkdir "%varOutput%\libaftendll_x86_SSE"
mkdir "%varOutput%\libaftendll_x86_SSE2"
mkdir "%varOutput%\libaftendll_x86_SSE3"

set varPath=doc
copy "%varBase%\%varPath%\Changes.txt" "%varOutput%"
copy "%varBase%\%varPath%\Copying.txt" "%varOutput%"
copy "%varBase%\%varPath%\ReadMe.txt" "%varOutput%"
copy "%varBase%\%varPath%\Command-Line.txt" "%varOutput%"

copy "%varBase%\Release\EncWAVtoAC3.exe" "%varOutput%"
copy "%varBase%\UnicoWS\unicows.dll" "%varOutput%"
copy "%varBase%\%varPathAftenBin%\libaftendll_x86\libaften.dll" "%varOutput%\libaftendll_x86\libaften.dll"
copy "%varBase%\%varPathAftenBin%\libaftendll_x86_SSE\libaften.dll" "%varOutput%\libaftendll_x86_SSE\libaften.dll"
copy "%varBase%\%varPathAftenBin%\libaftendll_x86_SSE2\libaften.dll" "%varOutput%\libaftendll_x86_SSE2\libaften.dll"
copy "%varBase%\%varPathAftenBin%\libaftendll_x86_SSE3\libaften.dll" "%varOutput%\libaftendll_x86_SSE3\libaften.dll"
copy "%varBase%\engines\unicode\EncWAVtoAC3.engines" "%varOutput%"

rem "%varWinRAR%" a -ep1 -m5 -r -s -t "%varOutput%.rar" "%varOutput%\*"
"%varWinRAR%" a -ep1 -m5 -r -t "%varOutput%.zip" "%varOutput%\*"
rmdir /S /Q "%varOutput%"

endlocal