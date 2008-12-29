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

set BasePath=.
set BuildNames=EncoderBuilds.inc

@del /Q "%BasePath%\*.aps"
@del /Q "%BasePath%\*.ncb"
@del /Q "%BasePath%\src\*.aps"
@del /Q /AH "%BasePath%\*.suo"
@del /Q "%BasePath%\*.user"

FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    @del "%BasePath%\%%i\Win32\EncWAVtoAC3.exe"
    @del "%BasePath%\%%i\Win32\*.htm"
    @del "%BasePath%\%%i\Win32\*.idb"
    @del "%BasePath%\%%i\Win32\*.ilk"
    @del "%BasePath%\%%i\Win32\*.obj"
    @del "%BasePath%\%%i\Win32\*.pch"
    @del "%BasePath%\%%i\Win32\*.pdb"
    @del "%BasePath%\%%i\Win32\*.res"
    @del "%BasePath%\%%i\Win32\*.manifest"
    @del "%BasePath%\%%i\Win32\*.dep"
    @del "%BasePath%\%%i\Win32\*.config
    @del "%BasePath%\%%i\Win32\*.dll
    @del "%BasePath%\%%i\Win32\*.files
    @del "%BasePath%\%%i\Win32\*.presets
    @del "%BasePath%\%%i\Win32\*.log
)

FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    @del "%BasePath%\%%i\x64\EncWAVtoAC3.exe"
    @del "%BasePath%\%%i\x64\*.htm"
    @del "%BasePath%\%%i\x64\*.idb"
    @del "%BasePath%\%%i\x64\*.ilk"
    @del "%BasePath%\%%i\x64\*.obj"
    @del "%BasePath%\%%i\x64\*.pch"
    @del "%BasePath%\%%i\x64\*.pdb"
    @del "%BasePath%\%%i\x64\*.res"
    @del "%BasePath%\%%i\x64\*.manifest"
    @del "%BasePath%\%%i\x64\*.dep"
    @del "%BasePath%\%%i\x64\*.config
    @del "%BasePath%\%%i\x64\*.dll
    @del "%BasePath%\%%i\x64\*.files
    @del "%BasePath%\%%i\x64\*.presets
    @del "%BasePath%\%%i\x64\*.log
)

endlocal