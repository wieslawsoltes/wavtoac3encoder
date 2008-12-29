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

rem Win32

set BasePath=Debug\Win32
rmdir /S /Q "%BasePath%\libaftendll_x86"
rmdir /S /Q "%BasePath%\libaftendll_x86_SSE"
rmdir /S /Q "%BasePath%\libaftendll_x86_SSE2"
rmdir /S /Q "%BasePath%\libaftendll_x86_SSE3"
del "%BasePath%\%Win32FileName%"

set BasePath=Release\Win32
rmdir /S /Q "%BasePath%\libaftendll_x86"
rmdir /S /Q "%BasePath%\libaftendll_x86_SSE"
rmdir /S /Q "%BasePath%\libaftendll_x86_SSE2"
rmdir /S /Q "%BasePath%\libaftendll_x86_SSE3"
del "%BasePath%\%Win32FileName%"

rem x64

set BasePath=Debug\x64
rmdir /S /Q "%BasePath%\libaftendll_AMD64"
rmdir /S /Q "%BasePath%\libaftendll_AMD64_SSE2"
rmdir /S /Q "%BasePath%\libaftendll_AMD64_SSE3"
del "%BasePath%\%Win64FileName%"

set BasePath=Release\x64
rmdir /S /Q "%BasePath%\libaftendll_AMD64"
rmdir /S /Q "%BasePath%\libaftendll_AMD64_SSE2"
rmdir /S /Q "%BasePath%\libaftendll_AMD64_SSE3"
del "%BasePath%\%Win64FileName%"

endlocal