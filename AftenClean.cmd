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

set BasePathAftenBin=src\aften\windows\output

rmdir /S /Q "%BasePathAftenBin%\aften_x86"
rmdir /S /Q "%BasePathAftenBin%\aften_x86_SSE"
rmdir /S /Q "%BasePathAftenBin%\aften_x86_SSE2"
rmdir /S /Q "%BasePathAftenBin%\aften_x86_SSE3"

rmdir /S /Q "%BasePathAftenBin%\libaftendll_x86"
rmdir /S /Q "%BasePathAftenBin%\libaftendll_x86_SSE"
rmdir /S /Q "%BasePathAftenBin%\libaftendll_x86_SSE2"
rmdir /S /Q "%BasePathAftenBin%\libaftendll_x86_SSE3"
rmdir /S /Q "%BasePathAftenBin%\wavfilter_x86"
rmdir /S /Q "%BasePathAftenBin%\wavinfo_x86"
rmdir /S /Q "%BasePathAftenBin%\wavrms_x86"

rmdir /S /Q "%BasePathAftenBin%\aften_AMD64"
rmdir /S /Q "%BasePathAftenBin%\aften_AMD64_SSE2"
rmdir /S /Q "%BasePathAftenBin%\aften_AMD64_SSE3"
rmdir /S /Q "%BasePathAftenBin%\libaftendll_AMD64"
rmdir /S /Q "%BasePathAftenBin%\libaftendll_AMD64_SSE2"
rmdir /S /Q "%BasePathAftenBin%\libaftendll_AMD64_SSE3"

rmdir /S /Q "%BasePathAftenBin%\wavfilter_AMD64"
rmdir /S /Q "%BasePathAftenBin%\wavinfo_AMD64"
rmdir /S /Q "%BasePathAftenBin%\wavrms_AMD64"

rmdir /S /Q "%BasePathAftenBin%\COPYING"
rmdir /S /Q "%BasePathAftenBin%\aften.h"

endlocal