@echo off
rem
rem WAV to AC3 Encoder
rem Copyright (C) 2007-2014 Wieslaw Soltes <wieslaw.soltes@gmail.com>
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

set BasePath=.
set BuildNames=EncoderBuilds.inc

set SolutionName=EncWAVtoAC3

setlocal

rem VS2008
@call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    devenv /rebuild "%%i|Win32" "%BasePath%\%SolutionName%.sln"
)

endlocal

setlocal

rem VS2008
@call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    devenv /rebuild "%%i|x64" "%BasePath%\%SolutionName%.sln"
)

endlocal