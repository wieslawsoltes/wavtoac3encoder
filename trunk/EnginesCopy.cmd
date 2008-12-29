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

set Win32FileName=EncWAVtoAC3.win32
set Win64FileName=EncWAVtoAC3.win64
set BasePathAftenBin=src\aften\windows\output
set BasePathAnsi=engines\ansi
set BasePathUnicode=engines\unicode
set LibAftenDll=libaften.dll

rem Win32

set BasePath=Debug\Win32
mkdir "%BasePath%\libaftendll_x86"
mkdir "%BasePath%\libaftendll_x86_SSE"
mkdir "%BasePath%\libaftendll_x86_SSE2"
mkdir "%BasePath%\libaftendll_x86_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_x86\%LibAftenDll%" "%BasePath%\libaftendll_x86\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicode%\%Win32FileName%" "%BasePath%\%Win32FileName%"

set BasePath=Release\Win32
mkdir "%BasePath%\libaftendll_x86"
mkdir "%BasePath%\libaftendll_x86_SSE"
mkdir "%BasePath%\libaftendll_x86_SSE2"
mkdir "%BasePath%\libaftendll_x86_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_x86\%LibAftenDll%" "%BasePath%\libaftendll_x86\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicode%\%Win32FileName%" "%BasePath%\%Win32FileName%"

rem x64

set BasePath=Debug\x64
mkdir "%BasePath%\libaftendll_AMD64"
mkdir "%BasePath%\libaftendll_AMD64_SSE2"
mkdir "%BasePath%\libaftendll_AMD64_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64\%LibAftenDll%" "%BasePath%\libaftendll_AMD64\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicode%\%Win64FileName%" "%BasePath%\%Win64FileName%"

set BasePath=Release\x64
mkdir "%BasePath%\libaftendll_AMD64"
mkdir "%BasePath%\libaftendll_AMD64_SSE2"
mkdir "%BasePath%\libaftendll_AMD64_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64\%LibAftenDll%" "%BasePath%\libaftendll_AMD64\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicode%\%Win64FileName%" "%BasePath%\%Win64FileName%"

endlocal