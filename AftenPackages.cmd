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
setlocal

set PackageVersion=git-20091226-icl11_1_054
set PackageSrc=aften-%PackageVersion%-src
set PackageWin32=aften-%PackageVersion%-win32-bin
set PackageWin64=aften-%PackageVersion%-win64-bin
set Archiver_7Zip=C:\Program Files\7-Zip\7z.exe
set Archiver_WinRAR=c:\Program Files\WinRAR\WinRAR.exe
set Options_WinRAR_zip=a -ep1 -m5 -r -t
set Options_WinRAR_rar=a -ep1 -m5 -r -s -t
set Options_7Zip_7z=a -r0 -t7z -mx=5 -ms=on -mmt=on

FOR /D %%i IN (%PackageSrc%;%PackageWin32%;%PackageWin64%) DO (
  "%Archiver_WinRAR%" %Options_WinRAR_zip% "%%i.zip" "%%i\*"
  rem "%Archiver_WinRAR%" %Options_WinRAR_rar% "%%i.rar" "%%i\*"
  rem "%Archiver_7Zip%" %Options_7Zip_7z% "%%i.7z" "%%i\*"
)

rem TODO: delete output dirs

endlocal